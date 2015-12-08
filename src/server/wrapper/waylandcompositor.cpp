#include "waylandcompositor.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc99-extensions"

const struct wl_compositor_interface WaylandCompositor::sInterface = {
    .create_surface = WaylandCompositor::hookCreateSurface,
    .create_region = WaylandCompositor::hookCreateRegion};

#pragma GCC diagnostic pop
