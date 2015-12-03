#ifndef WAYLANDOUTPUT_HPP
#define WAYLANDOUTPUT_HPP

#include <wayland-server.h>
#include "waylandglobalobject.hpp"

class WaylandOutput: public WaylandGlobalObject<WaylandOutput,
        wl_output_interface, struct wl_output_interface>
{
public:
    WaylandOutput(): WaylandGlobalObject(nullptr) { }

};

#endif // WAYLANDOUTPUT_HPP
