#include "waylandshell.hpp"

const struct wl_shell_interface WaylandShell::sInterface = {hookGetShellSurface};
