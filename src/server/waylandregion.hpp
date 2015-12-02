#ifndef WAYLANDREGION_HPP
#define WAYLANDREGION_HPP

#include <wayland-server.h>
#include "../utils/logger.hpp"
#include "waylandresource.hpp"

class WaylandRegion : public WaylandResource<WaylandRegion, wl_region_interface,
        struct wl_region_interface>
{
public:
    WaylandRegion(wl_client* client, uint32_t id): WaylandResource(this, &sInterface, client, id) {
    }

private:
    static struct wl_region_interface sInterface;

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
#endif // WAYLANDREGION_HPP
