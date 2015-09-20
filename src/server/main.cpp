#include <wayland-server.h>
#include <exception>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "../utils/logger.hpp"

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
        mInterface.frame = WaylandSurface::surfaceFrame;
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
    }

    void surfaceCommit(wl_client* client, wl_resource* resource) {
        wl_shm_buffer* b = wl_shm_buffer_get(mBuffer);
        uint32_t* color = (uint32_t*)wl_shm_buffer_get_data(b);
        LOGVP("Color %d", *color);
    }

    void surfaceAttach(wl_client* client, wl_resource* resource, wl_resource* buffer,
                       int32_t sx, int32_t sy) {
        mBuffer = buffer;
    }

private:
    struct wl_surface_interface mInterface;
    wl_resource* mResource;
    wl_resource* mBuffer;
    int32_t mWidth;
    int32_t mHeight;

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

    static void surfaceFrame(wl_client* client, wl_resource* resource, uint32_t callback) {
        LOGVP();
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

class WaylandShell
{
public:
    WaylandShell() {
        mInterface.get_shell_surface = WaylandShell::getShellSurface;
    }

    struct wl_shell_interface& getInterface() {
        return mInterface;
    }

private:
    struct wl_shell_interface mInterface;

    static void getShellSurface(wl_client *client, wl_resource *resource, uint32_t id,
                                wl_resource *surface_resource) {
        LOGVP();
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

    WaylandShell& getShell() {
        return mShell;
    }

    WaylandSurface* createSurface(wl_client* client, wl_resource* resource, uint32_t id) {
        return new WaylandSurface(client, resource, id);
    }

    WaylandRegion* createRegion(wl_client* client, wl_resource* resource, uint32_t id) {
        return new WaylandRegion(client, resource, id);
    }

private:
    struct wl_compositor_interface mInterface;
    WaylandShell mShell;

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
        if (not wl_global_create(mDisplay, &wl_compositor_interface, 3, &mCompositor,
                                 WaylandServer::bindCompositor)) {
            throw exception();
        }

        if (not wl_global_create(mDisplay, &wl_shell_interface, 1, &mCompositor.getShell(),
                                 WaylandServer::bindShell)) {
            throw exception();
        }

        wl_display_init_shm(mDisplay);
    }

    ~WaylandServer() {
        wl_display_terminate(mDisplay);
        wl_display_destroy(mDisplay);
        mDisplay = nullptr;
    }

    void run() {
        wl_display_run(mDisplay);
    }

private:
    wl_display* mDisplay;
    wl_event_loop* mLoop;
    WaylandCompositor mCompositor;

    static int idleHandle(void* data) {
        LOGVP();
        return 1;
    }

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
