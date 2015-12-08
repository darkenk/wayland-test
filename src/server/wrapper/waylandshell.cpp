#include "waylandshell.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc99-extensions"

const struct wl_shell_interface WaylandShell::sInterface = {.get_shell_surface =
                                                                hookGetShellSurface};

#pragma GCC diagnostic pop
