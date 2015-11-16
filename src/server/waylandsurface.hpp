#ifndef WAYLANDSURFACE_HPP
#define WAYLANDSURFACE_HPP

#include <wayland-server.h>
#include <memory>
#include <utility>
#include "../utils/logger.hpp"
#include "../utils/make_unique.hpp"
#include "waylandshellsurface.hpp"

class SurfaceState
{
public:
    SurfaceState(): buffer(nullptr), x(0), y(0), callbackDone(nullptr), isReady(false) {}

    wl_resource* buffer;
    int32_t x;
    int32_t y;
    wl_resource* callbackDone;
    bool isReady;
};


class WaylandSurface
{
public:
    WaylandSurface(wl_client* client, wl_resource* resource, uint32_t id):
        mX(0), mY(0) {
        mResource = wl_resource_create(client, &wl_surface_interface,
                                       wl_resource_get_version(resource), id);
        if (not mResource) {
            wl_resource_post_no_memory(resource);
            return;
        }
        wl_resource_set_implementation(mResource, &sInterface, this, nullptr);
        mClient = client;
        mFront = std::make_unique<SurfaceState>();
        mBack = std::make_unique<SurfaceState>();
    }

    void surfaceCommit(wl_client* /*client*/, wl_resource* /*resource*/) {
        if (mBack->buffer) {
            mBack->isReady = true;
            mBuffersFilled++;
            if (mBuffersFilled < MAX_BUFFERS && mBack->callbackDone) {
                wl_callback_send_done(mBack->callbackDone, 0 /*dumb timestamp*/);
                mBack->callbackDone = nullptr;
            }
            swapBuffers();
        }
    }

    void surfaceAttach(wl_client* /*client*/, wl_resource* /*resource*/, wl_resource* buffer,
                       int32_t x, int32_t y) {
        wl_shm_buffer* buf = wl_shm_buffer_get(buffer);
        if (not buf) {
            LOGVP("Why buffer is empty?");
            return;
        }
        mBack->x = x;
        mBack->y = y;
        mBack->buffer = buffer;
    }

    void surfaceFrame(wl_client* client, wl_resource* /*resource*/, uint32_t callback) {
        mBack->callbackDone = wl_resource_create(client, &wl_callback_interface, 1, callback);
    }

    bool isReady() {
        return mFront->isReady;
    }

    wl_shm_buffer* lockBuffer() {
        if (not mFront->isReady) {
            return nullptr;
        }
        wl_shm_buffer* buf = wl_shm_buffer_get(mFront->buffer);
        wl_shm_buffer_begin_access(buf);
        return buf;
    }

    void unlockBuffer() {
        if (not mFront->buffer) {
            return;
        }
        wl_shm_buffer* buf = wl_shm_buffer_get(mFront->buffer);
        wl_resource_queue_event(mFront->buffer, WL_BUFFER_RELEASE);
        wl_shm_buffer_end_access(buf);
        mFront->buffer = nullptr;
        if (mFront->callbackDone) {
            wl_callback_send_done(mFront->callbackDone, 0 /*dumb timestamp*/);
            mFront->callbackDone = nullptr;
        }
        mFront->isReady = false;
        mBuffersFilled--;
    }

    wl_client* client() const {
        return mClient;
    }

    int x() {
        return mX;
    }

    void setX(int x) {
        mX = x;
    }

    int y() {
        return mY;
    }

    void setY(int y) {
        mY = y;
    }

    void setShellSurface(std::unique_ptr<WaylandShellSurface> shellSurface) {
        mShellSurface = std::move(shellSurface);
    }

    bool isShellSurface() {
        return mShellSurface != nullptr;
    }

private:
    static struct wl_surface_interface sInterface;
    wl_resource* mResource;
    std::unique_ptr<SurfaceState> mFront;
    std::unique_ptr<SurfaceState> mBack;
    uint32_t mBuffersFilled = 0;
    static constexpr uint32_t MAX_BUFFERS  = 2;
    wl_client* mClient;
    int mX;
    int mY;
    std::unique_ptr<WaylandShellSurface> mShellSurface;

    void swapBuffers() {
        mFront.swap(mBack);
    }

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

#endif // WAYLANDSURFACE_HPP
