#include "waylandpointer.hpp"

// clang-format off
struct wl_pointer_interface WaylandPointer::sInterface = {
    hookSetCursor,
    hookRelease
};

// clang-format on
