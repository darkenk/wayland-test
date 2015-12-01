#ifndef WAYLANDSHELL_HPP
#define WAYLANDSHELL_HPP

#include <wayland-server.h>
#include "../utils/logger.hpp"
#include "../utils/make_unique.hpp"
#include "waylandshellsurface.hpp"
#include "waylandsurface.hpp"
#include "waylandglobalobject.hpp"

class WaylandShell : public WaylandGlobalObject<WaylandShell, wl_shell_interface,
        struct wl_shell_interface>
{
public:
    WaylandShell(wl_display* display): WaylandGlobalObject(display) {

    }

protected:
    static const struct wl_shell_interface* getInterface() {
        return &sInterface;
    }

private:
    const static struct wl_shell_interface sInterface;
    friend WaylandGlobalObject<WaylandShell, wl_shell_interface, struct wl_shell_interface>;

    void getShellSurface(wl_client* client, wl_resource* /*resource*/, uint32_t id,
                         wl_resource* surface) {
        LOGVP();
        WaylandSurface* ws = reinterpret_cast<WaylandSurface*>(wl_resource_get_user_data(surface));
        ws->setShellSurface(std::make_unique<WaylandShellSurface>(client, id));
    }

    static void hookGetShellSurface(wl_client* client, wl_resource* resource, uint32_t id,
                             wl_resource* surface) {
        WaylandShell* ws = getThis(resource);
        ws->getShellSurface(client, resource, id, surface);
    }
};

#endif // WAYLANDSHELL_HPP
