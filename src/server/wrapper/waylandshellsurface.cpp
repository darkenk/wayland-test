#include "waylandshellsurface.hpp"

// clang format off
struct wl_shell_surface_interface WaylandShellSurface::sInterface = {
    hookPong,
    hookMove,
    hookResize,
    hookSetToplevel,
    hookSetTransient,
    hookSetFullscreen,
    hookSetPopup,
    hookSetMaximized,
    hookSetTitle,
    hookSetClass
};
// clang format on

