#ifndef WAYLANDOUTPUT_HPP
#define WAYLANDOUTPUT_HPP

#include "waylandglobalobject.hpp"
#include <wayland-server.h>

class WaylandOutput
    : public WaylandGlobalObject<WaylandOutput, wl_output_interface, struct wl_output_interface>
{
public:
    WaylandOutput() : WaylandGlobalObject(nullptr) {}
};

#endif  // WAYLANDOUTPUT_HPP
