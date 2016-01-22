#include "waylandcompositor.hpp"

const struct wl_compositor_interface WaylandCompositor::sInterface = {
    WaylandCompositor::hookCreateSurface,
    WaylandCompositor::hookCreateRegion};
