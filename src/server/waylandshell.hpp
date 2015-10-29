#ifndef WAYLANDSHELL_HPP
#define WAYLANDSHELL_HPP

#include <wayland-server.h>
#include "../utils/logger.hpp"
#include "../utils/make_unique.hpp"
#include "waylandshellsurface.hpp"

class WaylandShell
{
public:
    WaylandShell() {}

    void getShellSurface(wl_client* client, wl_resource* /*resource*/, uint32_t id,
                         wl_resource* /*surface*/) {
        LOGVP();
        mShellSurface = std::make_unique<WaylandShellSurface>(client, id);
    }

    void bind(wl_client* client, uint32_t version, uint32_t id) {
        LOGVP();
        wl_resource* resource = wl_resource_create(client, &wl_shell_interface, version, id);
        if (not resource) {
            wl_client_post_no_memory(client);
            return;
        }
        wl_resource_set_implementation(resource, &sInterface, this, nullptr);
    }

private:
    static struct wl_shell_interface sInterface;
    std::unique_ptr<WaylandShellSurface> mShellSurface;

    static void hookGetShellSurface(wl_client* client, wl_resource* resource, uint32_t id,
                             wl_resource* surface) {
        WaylandShell* ws = reinterpret_cast<WaylandShell*>(wl_resource_get_user_data(resource));
        ws->getShellSurface(client, resource, id, surface);
    }
};

#endif // WAYLANDSHELL_HPP
