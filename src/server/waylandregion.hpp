#ifndef WAYLANDREGION_HPP
#define WAYLANDREGION_HPP

#include <wayland-server.h>
#include "../utils/logger.hpp"

class WaylandRegion
{
public:
    WaylandRegion(wl_client* client, wl_resource* /*resource*/, uint32_t id) {
        mResource = wl_resource_create(client, &wl_region_interface, 1, id);
        wl_resource_set_implementation(mResource, &sInterface, this, nullptr);
    }

private:
    static struct wl_region_interface sInterface;
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
#endif // WAYLANDREGION_HPP
