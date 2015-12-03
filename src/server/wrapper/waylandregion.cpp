#include "waylandregion.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc99-extensions"

struct wl_region_interface WaylandRegion::sInterface = {
    .destroy = WaylandRegion::hookRegionDestroy,
    .add = WaylandRegion::hookRegionAdd,
    .subtract = WaylandRegion::hookRegionSubtract
};

#pragma GCC diagnostic pop
