#ifndef WAYLANDSHELL_HPP
#define WAYLANDSHELL_HPP

#include "dk_utils/make_unique.hpp"
#include "waylandglobalobject.hpp"
#include "waylandshellsurface.hpp"
#include "waylandsurface.hpp"
#include <wayland-server.h>

class WaylandShell
    : public WaylandGlobalObject<WaylandShell, wl_shell_interface, struct wl_shell_interface>
{
public:
    WaylandShell() : WaylandGlobalObject(&sInterface) {}
    ~WaylandShell() {}

protected:
    virtual void getShellSurface(wl_client* /*client*/, wl_resource* /*resource*/, uint32_t /*id*/,
                                 wl_resource* /*surface*/) {}

private:
    const static struct wl_shell_interface sInterface;

    static void hookGetShellSurface(wl_client* client, wl_resource* resource, uint32_t id,
                                    wl_resource* surface) {
        getThis(resource)->getShellSurface(client, resource, id, surface);
    }
};

#endif  // WAYLANDSHELL_HPP
