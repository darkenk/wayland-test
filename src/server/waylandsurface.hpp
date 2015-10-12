#ifndef WAYLANDSURFACE_HPP
#define WAYLANDSURFACE_HPP

#include <wayland-server.h>
#include "../utils/logger.hpp"

class WaylandSurface
{
public:
    WaylandSurface(wl_client* client, wl_resource* resource, uint32_t id) {
        mResource = wl_resource_create(client, &wl_surface_interface,
                                       wl_resource_get_version(resource), id);
        if (not mResource) {
            wl_resource_post_no_memory(resource);
            return;
        }
        wl_resource_set_implementation(mResource, &sInterface, this, nullptr);
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
    static struct wl_surface_interface sInterface;
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

#endif // WAYLANDSURFACE_HPP
