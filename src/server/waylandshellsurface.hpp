#ifndef WAYLANDSHELLSURFACE_HPP
#define WAYLANDSHELLSURFACE_HPP

#include <wayland-server.h>

class WaylandShellSurface
{
public:
    WaylandShellSurface(wl_client* client, uint32_t id) {
        wl_resource* resource = wl_resource_create(client, &wl_shell_surface_interface, 1, id);
        wl_resource_set_implementation(resource, &sInterface, this, nullptr);
    }

private:
    static struct wl_shell_surface_interface sInterface;

    static void hookPong(wl_client* /*client*/, wl_resource* /*resource*/, uint32_t /*serial*/) {
    }

    static void hookMove(wl_client* /*client*/, wl_resource* /*resource*/, wl_resource* /*seat*/,
                         uint32_t /*serial*/) {
    }

    static void hookResize(wl_client* /*client*/, wl_resource* /*resource*/, wl_resource* /*seat*/,
                           uint32_t /*serial*/, uint32_t /*edges*/) {
    }

    static void hookSetToplevel(wl_client* /*client*/, wl_resource* /*resource*/) {
    }

    static void hookSetTransient(wl_client* /*client*/, wl_resource* /*resource*/,
                                 wl_resource* /*parent*/, int32_t /*x*/, int32_t /*y*/,
                                 uint32_t /*flags*/) {
    }

    static void hookSetFullscreen(wl_client* /*client*/, wl_resource* /*resource*/,
                                  uint32_t /*method*/, uint32_t /*framerate*/,
                                  wl_resource* /*output*/) {
    }

    static void hookSetPopup(wl_client* /*client*/, wl_resource* /*resource*/,
                             wl_resource* /*seat*/, uint32_t /*serial*/, wl_resource* /*parent*/,
                             int32_t /*x*/, int32_t /*y*/, uint32_t /*flags*/) {
    }

    static void hookSetMaximized(wl_client* /*client*/, wl_resource* /*resource*/,
                                 wl_resource* /*output*/) {
    }

    static void hookSetTitle(wl_client* /*client*/, wl_resource* /*resource*/,
                             const char* /*title*/) {
    }

    static void hookSetClass(wl_client* /*client*/, wl_resource* /*resource*/,
                             const char* /*class_*/) {
    }

};

#endif // WAYLANDSHELLSURFACE_HPP
