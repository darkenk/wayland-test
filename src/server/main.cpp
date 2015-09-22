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

using namespace std;

class WaylandRegion
{
public:
    WaylandRegion(wl_client* client, wl_resource* resource, uint32_t id) {
        mInterface.destroy = WaylandRegion::hookRegionDestroy;
        mInterface.add = WaylandRegion::hookRegionAdd;
        mInterface.subtract = WaylandRegion::hookRegionSubtract;

        mResource = wl_resource_create(client, &wl_region_interface, 1, id);
        wl_resource_set_implementation(mResource, &mInterface, this, nullptr);
    }


private:
    struct wl_region_interface mInterface;
    wl_resource* mResource;

    static void hookRegionDestroy(wl_client* client, wl_resource* resource) {
        LOGVP();
        wl_resource_destroy(resource);
    }

    static void hookRegionAdd(wl_client* client, wl_resource* resource,
                              int32_t x, int32_t y, int32_t width, int32_t height) {
        LOGVP();
    }

    static void hookRegionSubtract(wl_client* client, wl_resource* resource,
                               int32_t x, int32_t y, int32_t width, int32_t height) {
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
        // TODO: dumb hardcoding!
        mOutput = new X11Backend(480, 360);
    }

    void surfaceCommit(wl_client* client, wl_resource* resource) {
        wl_shm_buffer* buf = wl_shm_buffer_get(mBuffer);
        wl_shm_buffer_begin_access(buf);
        uint32_t* color = (uint32_t*)wl_shm_buffer_get_data(buf);
        mOutput->drawBuffer(color);
        wl_resource_queue_event(mBuffer, WL_BUFFER_RELEASE);
        wl_callback_send_done(mCallbackDone, 0 /*dumb timestamp*/);
        wl_shm_buffer_end_access(buf);
    }

    void surfaceAttach(wl_client* client, wl_resource* resource, wl_resource* buffer,
                       int32_t sx, int32_t sy) {
        mBuffer = buffer;
        wl_shm_buffer* buf = wl_shm_buffer_get(buffer);
        wl_shm_buffer_begin_access(buf);
        mHeight = wl_shm_buffer_get_height(buf);
        mWidth = wl_shm_buffer_get_width(buf);
        mOutput->setSize(mWidth, mHeight);
        wl_shm_buffer_end_access(buf);
    }

    void surfaceFrame(wl_client* client, wl_resource* resource, uint32_t callback) {
        mCallbackDone = wl_resource_create(client, &wl_callback_interface, 1, callback);
    }

private:
    struct wl_surface_interface mInterface;
    wl_resource* mResource;
    wl_resource* mBuffer;
    wl_resource* mCallbackDone;
    int32_t mWidth;
    int32_t mHeight;
    X11Backend* mOutput;

    static void surfaceDestroy(wl_client* client, wl_resource* resource) {
        LOGVP();
        wl_resource_destroy(resource);
    }

    static void hookSurfaceAttach(wl_client* client, wl_resource* resource, wl_resource* buffer,
                              int32_t sx, int32_t sy) {
        LOGVP();
        getSurface(resource)->surfaceAttach(client, resource, buffer, sx, sy);
    }

    static void surfaceDamage(wl_client* client, wl_resource* resource, int32_t x, int32_t y,
                              int32_t width, int32_t height) {
        LOGVP();
    }

    static void hookSurfaceFrame(wl_client* client, wl_resource* resource, uint32_t callback) {
        LOGVP();
        getSurface(resource)->surfaceFrame(client, resource, callback);
    }

    static void surfaceSetOpaqueRegion(wl_client* client, wl_resource* resource,
                                       wl_resource* regionResource) {
        LOGVP();
    }

    static void surfaceSetInputRegion(wl_client* client, wl_resource* resource,
                                      wl_resource* regionResource) {
        LOGVP();
    }

    static void hookSurfaceCommit(wl_client* client, wl_resource* resource) {
        LOGVP();
        getSurface(resource)->surfaceCommit(client, resource);
    }

    static void surfaceSetBufferTransform(wl_client* client, wl_resource* resource, int transform) {
        LOGVP();
    }

    static void surfaceSetBufferScale(wl_client* client, wl_resource* resource, int32_t scale) {
        LOGVP();
    }

    static inline WaylandSurface* getSurface(wl_resource* resource) {
        return reinterpret_cast<WaylandSurface*>(wl_resource_get_user_data(resource));
    }
};

class WaylandCompositor
{
public:
    WaylandCompositor() {
        mInterface.create_surface = WaylandCompositor::hookCreateSurface;
        mInterface.create_region = WaylandCompositor::hookCreateRegion;
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
        if (mRegion) {
            LOGVE("Only one region is currently supported");
            return;
        }
        mRegion = make_unique<WaylandRegion>(client, resource, id);
    }

private:
    struct wl_compositor_interface mInterface;
    unique_ptr<WaylandSurface> mSurface;
    unique_ptr<WaylandRegion> mRegion;

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
};

class WaylandServer
{
public:
    WaylandServer() {
        mDisplay = wl_display_create();
        if (not mDisplay) {
            throw exception();
        }
        mLoop = wl_display_get_event_loop(mDisplay);
        const char* socketName = wl_display_add_socket_auto(mDisplay);
        LOGVP("Socket Name %s", socketName);
        mWlCompositor = wl_global_create(mDisplay, &wl_compositor_interface, 3, &mCompositor,
                                         WaylandServer::bindCompositor);
        if (not mWlCompositor) {
            throw exception();
        }
        wl_display_init_shm(mDisplay);
    }

    ~WaylandServer() {
        wl_display_terminate(mDisplay);
        wl_display_destroy(mDisplay);
        mDisplay = nullptr;
        wl_global_destroy(mWlCompositor);
        mWlCompositor = nullptr;
    }

    void run() {
        wl_display_run(mDisplay);
    }

private:
    wl_display* mDisplay;
    wl_event_loop* mLoop;
    wl_global* mWlCompositor;
    WaylandCompositor mCompositor;

    static void bindCompositor(wl_client* client, void* data, uint32_t version, uint32_t id) {
        LOGVP();
        WaylandCompositor* compositor = reinterpret_cast<WaylandCompositor*>(data);
        wl_resource* resource = wl_resource_create(client, &wl_compositor_interface, version, id);
        if (not resource) {
            wl_client_post_no_memory(client);
            return;
        }
        wl_resource_set_implementation(resource, &compositor->getInterface(), compositor,
                                       nullptr);
    }

    static void bindShell(wl_client* client, void* data, uint32_t version, uint32_t id) {
        LOGVP();
    }
};

int main(int argc, char *argv[])
{
    LOGVP("hello world!");
    WaylandServer().run();
    return 0;
}
