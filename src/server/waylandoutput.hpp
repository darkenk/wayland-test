#ifndef WAYLANDOUTPUT_HPP
#define WAYLANDOUTPUT_HPP

#include <wayland-server.h>
#include "../utils/logger.hpp"

class WaylandOutput
{
public:
    WaylandOutput() { }

    ~WaylandOutput() { }

    void bind(wl_client* client, uint32_t version, uint32_t id) {
        LOGVP();
        wl_resource* resource = wl_resource_create(client, &wl_output_interface, version, id);
        if (not resource) {
            wl_client_post_no_memory(client);
            return;
        }
        // wl_output has no requests, only events, thus no implementation
    }

private:
};

#endif // WAYLANDOUTPUT_HPP
