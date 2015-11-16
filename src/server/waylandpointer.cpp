#include "waylandpointer.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc99-extensions"

struct wl_pointer_interface WaylandPointer::sInterface = {
    .set_cursor = hookSetCursor,
    .release = hookRelease
};

#pragma GCC diagnostic pop
