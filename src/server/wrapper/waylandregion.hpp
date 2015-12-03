#ifndef WAYLANDREGION_HPP
#define WAYLANDREGION_HPP

#include <wayland-server.h>
#include "waylandresource.hpp"

class WaylandRegion : public WaylandResource<WaylandRegion, wl_region_interface,
        struct wl_region_interface>
{
public:
    WaylandRegion(): WaylandResource(&sInterface) {}

private:
    static struct wl_region_interface sInterface;

    static void hookRegionDestroy(wl_client* /*client*/, wl_resource* /*resource*/) { }

    static void hookRegionAdd(wl_client* /*client*/, wl_resource* /*resource*/,
                              int32_t /*x*/, int32_t /*y*/, int32_t /*width*/, int32_t /*height*/) {
    }

    static void hookRegionSubtract(wl_client* /*client*/, wl_resource* /*resource*/,
                               int32_t /*x*/, int32_t /*y*/, int32_t /*width*/, int32_t /*height*/) {
    }
};
#endif // WAYLANDREGION_HPP
