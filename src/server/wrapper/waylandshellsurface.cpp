#include "waylandshellsurface.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc99-extensions"

struct wl_shell_surface_interface WaylandShellSurface::sInterface = {
    .pong = hookPong,
    .move = hookMove,
    .resize = hookResize,
    .set_toplevel = hookSetToplevel,
    .set_transient = hookSetTransient,
    .set_fullscreen = hookSetFullscreen,
    .set_popup = hookSetPopup,
    .set_maximized = hookSetMaximized,
    .set_title = hookSetTitle,
    .set_class = hookSetClass

};

#pragma GCC diagnostic pop
