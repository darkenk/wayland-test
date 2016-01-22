#include "waylandregion.hpp"

struct wl_region_interface WaylandRegion::sInterface = {
    WaylandRegion::hookRegionDestroy,
    WaylandRegion::hookRegionAdd,
    WaylandRegion::hookRegionSubtract};
