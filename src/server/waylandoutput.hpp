#ifndef WAYLANDOUTPUT_HPP
#define WAYLANDOUTPUT_HPP

#include <wayland-server.h>
#include "../utils/logger.hpp"
#include "waylandglobalobject.hpp"

class WaylandOutput: public WaylandGlobalObject<WaylandOutput,
        wl_output_interface, struct wl_output_interface>
{
public:
    WaylandOutput(wl_display* display): WaylandGlobalObject(display) { }

protected:
    static const struct wl_output_interface* getInterface() {
        return nullptr;
    }

private:
    friend WaylandGlobalObject<WaylandOutput, wl_output_interface, struct wl_output_interface>;
};

#endif // WAYLANDOUTPUT_HPP
