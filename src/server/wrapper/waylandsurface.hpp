#ifndef WAYLANDSURFACE_HPP
#define WAYLANDSURFACE_HPP

#include "waylandresource.hpp"

class WaylandSurface : public WaylandResource<WaylandSurface, wl_surface_interface,
        struct wl_surface_interface>
{
public:
    WaylandSurface(): WaylandResource(&sInterface) { }

protected:
    virtual void surfaceDestroy(wl_client* /*client*/, wl_resource* /*resource*/) { }

    virtual void surfaceAttach(wl_client* /*client*/, wl_resource* /*resource*/, wl_resource* /*buffer*/,
                              int32_t /*sx*/, int32_t /*sy*/) {
    }

    virtual void surfaceFrame(wl_client* /*client*/, wl_resource* /*resource*/, uint32_t /*callback*/) {
    }

    virtual void surfaceCommit(wl_client* /*client*/, wl_resource* /*resource*/) { }

private:
    static struct wl_surface_interface sInterface;

    static void hookSurfaceDestroy(wl_client* client, wl_resource* resource) {
        getThis(resource)->surfaceDestroy(client, resource);
    }

    static void hookSurfaceAttach(wl_client* client, wl_resource* resource, wl_resource* buffer,
                              int32_t sx, int32_t sy) {
        getThis(resource)->surfaceAttach(client, resource, buffer, sx, sy);
    }

    static void surfaceDamage(wl_client* /*client*/, wl_resource* /*resource*/, int32_t /*x*/, int32_t /*y*/,
                              int32_t /*width*/, int32_t /*height*/) {
    }

    static void hookSurfaceFrame(wl_client* client, wl_resource* resource, uint32_t callback) {
        getThis(resource)->surfaceFrame(client, resource, callback);
    }

    static void surfaceSetOpaqueRegion(wl_client* /*client*/, wl_resource* /*resource*/,
                                       wl_resource* /*regionResource*/) {
    }

    static void surfaceSetInputRegion(wl_client* /*client*/, wl_resource* /*resource*/,
                                      wl_resource* /*regionResource*/) {
    }

    static void hookSurfaceCommit(wl_client* client, wl_resource* resource) {
        getThis(resource)->surfaceCommit(client, resource);
    }

    static void surfaceSetBufferTransform(wl_client* /*client*/, wl_resource* /*resource*/, int /*transform*/) {
    }

    static void surfaceSetBufferScale(wl_client* /*client*/, wl_resource* /*resource*/, int32_t /*scale*/) {
    }
};

#endif // WAYLANDSURFACE_HPP
