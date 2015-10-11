#include <wayland-server.h>
#include <exception>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "../utils/logger.hpp"
#include "x11backend.hpp"
#include <unistd.h>
#include <memory>
#include "../utils/make_unique.hpp"
#include <cstring>
#include <vector>

using namespace std;

class WaylandRegion
{
public:
    WaylandRegion(wl_client* client, wl_resource* /*resource*/, uint32_t id) {
        mInterface.destroy = WaylandRegion::hookRegionDestroy;
        mInterface.add = WaylandRegion::hookRegionAdd;
        mInterface.subtract = WaylandRegion::hookRegionSubtract;

        mResource = wl_resource_create(client, &wl_region_interface, 1, id);
        wl_resource_set_implementation(mResource, &mInterface, this, nullptr);
    }

private:
    struct wl_region_interface mInterface;
    wl_resource* mResource;

    static void hookRegionDestroy(wl_client* /*client*/, wl_resource* resource) {
        LOGVP();
        wl_resource_destroy(resource);
    }

    static void hookRegionAdd(wl_client* /*client*/, wl_resource* /*resource*/,
                              int32_t /*x*/, int32_t /*y*/, int32_t /*width*/, int32_t /*height*/) {
        LOGVP();
    }

    static void hookRegionSubtract(wl_client* /*client*/, wl_resource* /*resource*/,
                               int32_t /*x*/, int32_t /*y*/, int32_t /*width*/, int32_t /*height*/) {
        LOGVP();
    }
};

class WaylandSurface
{
public:
    WaylandSurface(wl_client* client, wl_resource* resource, uint32_t id) {
        mInterface.destroy = WaylandSurface::surfaceDestroy;
        mInterface.attach = WaylandSurface::hookSurfaceAttach;
        mInterface.damage = WaylandSurface::surfaceDamage;
        mInterface.frame = WaylandSurface::hookSurfaceFrame;
        mInterface.set_opaque_region = WaylandSurface::surfaceSetOpaqueRegion;
        mInterface.set_input_region = WaylandSurface::surfaceSetInputRegion;
        mInterface.commit = WaylandSurface::hookSurfaceCommit;
        mInterface.set_buffer_transform = WaylandSurface::surfaceSetBufferTransform;
        mInterface.set_buffer_scale = WaylandSurface::surfaceSetBufferScale;

        mResource = wl_resource_create(client, &wl_surface_interface,
                                       wl_resource_get_version(resource), id);
        if (not mResource) {
            wl_resource_post_no_memory(resource);
            return;
        }
        wl_resource_set_implementation(mResource, &mInterface, this, nullptr);
        mIsReady = false;
    }

    void surfaceCommit(wl_client* /*client*/, wl_resource* /*resource*/) {
        mIsReady = true;
    }

    void surfaceAttach(wl_client* /*client*/, wl_resource* /*resource*/, wl_resource* buffer,
                       int32_t /*sx*/, int32_t /*sy*/) {
        mBuffer = buffer;
        wl_shm_buffer* buf = wl_shm_buffer_get(buffer);
        wl_shm_buffer_begin_access(buf);
        mHeight = wl_shm_buffer_get_height(buf);
        mWidth = wl_shm_buffer_get_width(buf);
        wl_shm_buffer_end_access(buf);
    }

    void surfaceFrame(wl_client* client, wl_resource* /*resource*/, uint32_t callback) {
        mCallbackDone = wl_resource_create(client, &wl_callback_interface, 1, callback);
    }

    bool isReady() {
        return mIsReady;
    }

    wl_shm_buffer* lockBuffer() {
        wl_shm_buffer* buf = wl_shm_buffer_get(mBuffer);
        wl_shm_buffer_begin_access(buf);
        return buf;
    }

    void unlockBuffer() {
        wl_shm_buffer* buf = wl_shm_buffer_get(mBuffer);
        wl_resource_queue_event(mBuffer, WL_BUFFER_RELEASE);
        wl_callback_send_done(mCallbackDone, 0 /*dumb timestamp*/);
        wl_shm_buffer_end_access(buf);
        mIsReady = false;
    }

private:
    struct wl_surface_interface mInterface;
    wl_resource* mResource;
    wl_resource* mBuffer;
    wl_resource* mCallbackDone;
    int32_t mWidth;
    int32_t mHeight;
    bool mIsReady;

    static void surfaceDestroy(wl_client* /*client*/, wl_resource* resource) {
        LOGVP();
        wl_resource_destroy(resource);
    }

    static void hookSurfaceAttach(wl_client* client, wl_resource* resource, wl_resource* buffer,
                              int32_t sx, int32_t sy) {
        LOGVP();
        getSurface(resource)->surfaceAttach(client, resource, buffer, sx, sy);
    }

    static void surfaceDamage(wl_client* /*client*/, wl_resource* /*resource*/, int32_t /*x*/, int32_t /*y*/,
                              int32_t /*width*/, int32_t /*height*/) {
        LOGVP();
    }

    static void hookSurfaceFrame(wl_client* client, wl_resource* resource, uint32_t callback) {
        LOGVP();
        getSurface(resource)->surfaceFrame(client, resource, callback);
    }

    static void surfaceSetOpaqueRegion(wl_client* /*client*/, wl_resource* /*resource*/,
                                       wl_resource* /*regionResource*/) {
        LOGVP();
    }

    static void surfaceSetInputRegion(wl_client* /*client*/, wl_resource* /*resource*/,
                                      wl_resource* /*regionResource*/) {
        LOGVP();
    }

    static void hookSurfaceCommit(wl_client* client, wl_resource* resource) {
        LOGVP();
        getSurface(resource)->surfaceCommit(client, resource);
    }

    static void surfaceSetBufferTransform(wl_client* /*client*/, wl_resource* /*resource*/, int /*transform*/) {
        LOGVP();
    }

    static void surfaceSetBufferScale(wl_client* /*client*/, wl_resource* /*resource*/, int32_t /*scale*/) {
        LOGVP();
    }

    static inline WaylandSurface* getSurface(wl_resource* resource) {
        return reinterpret_cast<WaylandSurface*>(wl_resource_get_user_data(resource));
    }
};

class WaylandCompositor
{
public:
    WaylandCompositor(wl_display* display, X11Backend* output) : mClient(nullptr) {
        mInterface.create_surface = WaylandCompositor::hookCreateSurface;
        mInterface.create_region = WaylandCompositor::hookCreateRegion;
        mOutput = output;
        mDisplayWidth = mOutput->getWidth();
        mDisplayHeight = mOutput->getHeight();
        // repaint handler
        wl_event_loop* loop = wl_display_get_event_loop(display);
        mRepaintTimer = wl_event_loop_add_timer(loop, WaylandCompositor::hookRepaintHandler, this);
        wl_event_source_timer_update(mRepaintTimer, REPAINT_DELAY);
        mDisplayBuffer = make_unique<uint8_t[]>(mDisplayWidth * mDisplayHeight * 4);
    }

    ~WaylandCompositor() {
        free(mRepaintTimer);
    }

    struct wl_compositor_interface& getInterface() {
        return mInterface;
    }

    void createSurface(wl_client* client, wl_resource* resource, uint32_t id) {
        if (mSurface) {
            LOGVE("Only one surface is currently supported");
            return;
        }
        mSurface = make_unique<WaylandSurface>(client, resource, id);
    }

    void createRegion(wl_client* client, wl_resource* resource, uint32_t id) {
        LOGVP("Regions are not used :(");
        mRegions.push_back(make_unique<WaylandRegion>(client, resource, id));
    }

    int repaint() {
        //LOGVP();
        if (mSurface && mSurface->isReady()) {
            wl_shm_buffer* b = mSurface->lockBuffer();
            if (b) {
                copyShmBufferToDisplay(b);
                mOutput->drawBuffer(mDisplayBuffer.get());
            }
            mSurface->unlockBuffer();
        }
        wl_event_source_timer_update(mRepaintTimer, REPAINT_DELAY);
        return 0;
    }

    void bind(wl_client* client, uint32_t version, uint32_t id) {
        LOGVP();
        if (mClient) {
            LOGVP("Only one client is supported");
            return;
        }
        wl_resource* resource = wl_resource_create(client, &wl_compositor_interface, version, id);
        if (not resource) {
            wl_client_post_no_memory(client);
            return;
        }
        wl_resource_set_implementation(resource, &getInterface(), this,
                                       WaylandCompositor::hookClientDisconnects);
    }

private:
    struct wl_compositor_interface mInterface;
    unique_ptr<WaylandSurface> mSurface;
    unique_ptr<WaylandRegion> mRegion;
    wl_event_source* mRepaintTimer;
    X11Backend* mOutput;
    wl_client* mClient;
    int32_t mDisplayWidth;
    int32_t mDisplayHeight;
    unique_ptr<uint8_t[]> mDisplayBuffer;
    vector<unique_ptr<WaylandRegion>> mRegions;
    static constexpr int REPAINT_DELAY = 16;

    static void hookCreateSurface(wl_client* client, wl_resource* resource, uint32_t id) {
        LOGVP();
        WaylandCompositor* wc = reinterpret_cast<WaylandCompositor*>(
                    wl_resource_get_user_data(resource));
        wc->createSurface(client, resource, id);
    }

    static void hookCreateRegion(wl_client* client, wl_resource* resource, uint32_t id) {
        LOGVP();
        WaylandCompositor* wc = reinterpret_cast<WaylandCompositor*>(
                    wl_resource_get_user_data(resource));
        wc->createRegion(client, resource, id);
    }

    static int hookRepaintHandler(void* data) {
        WaylandCompositor* wc = reinterpret_cast<WaylandCompositor*>(data);
        return wc->repaint();
    }

    static void hookClientDisconnects(struct wl_resource *resource) {
        WaylandCompositor* wc = reinterpret_cast<WaylandCompositor*>(
                    wl_resource_get_user_data(resource));
        wc->clientDisconnects(wl_resource_get_client(resource));
    }

    void clientDisconnects(wl_client* /*client*/) {
        LOGVP();
        mClient = nullptr;
        mSurface.reset();
        mRegion.reset();
    }

    void copyShmBufferToDisplay(wl_shm_buffer* b) {
        uint8_t* data = reinterpret_cast<uint8_t*>(wl_shm_buffer_get_data(b));
        int32_t w = wl_shm_buffer_get_width(b);
        int32_t h = wl_shm_buffer_get_height(b);
        int32_t lineSize = w < mDisplayWidth ? w * 4 : mDisplayWidth * 4;
        int32_t nrOfLines = h < mDisplayHeight ? h : mDisplayHeight;
        int32_t offsetDst = 0;
        int32_t offsetSrc = 0;

        for (int32_t y = 0; y < nrOfLines; y++) {
            memcpy(mDisplayBuffer.get() + offsetSrc, data + offsetDst, lineSize);
            offsetSrc += mDisplayWidth * 4;
            offsetDst += w * 4;
        }
    }
};

class WaylandServer
{
public:
    WaylandServer() {
        mDisplay = wl_display_create();
        if (not mDisplay) {
            throw exception();
        }
        mOutput = new X11Backend(480, 360, mDisplay);
        mCompositor = make_unique<WaylandCompositor>(mDisplay, mOutput);
        const char* socketName = wl_display_add_socket_auto(mDisplay);
        LOGVP("Socket Name %s", socketName);
        if (not wl_global_create(mDisplay, &wl_compositor_interface, 3, mCompositor.get(),
                                 WaylandServer::bindCompositor)) {
            throw exception();
        }
        wl_display_init_shm(mDisplay);
    }

    ~WaylandServer() {
        wl_display_terminate(mDisplay);
        wl_display_destroy(mDisplay);
        mDisplay = nullptr;
        delete mOutput;
        mOutput = nullptr;
    }

    void run() {
        wl_display_run(mDisplay);
    }

private:
    wl_display* mDisplay;
    unique_ptr<WaylandCompositor> mCompositor;
    X11Backend* mOutput;

    static void bindCompositor(wl_client* client, void* data, uint32_t version, uint32_t id) {
        LOGVP();
        WaylandCompositor* compositor = reinterpret_cast<WaylandCompositor*>(data);
        compositor->bind(client, version, id);
    }
};

int main()
{
    LOGVP("hello world!");
    WaylandServer().run();
    return 0;
}
