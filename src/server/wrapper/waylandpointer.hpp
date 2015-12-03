#ifndef WAYLANDPOINTER_HPP
#define WAYLANDPOINTER_HPP

#include <wayland-server.h>
#include <memory>
#include "waylandshell.hpp"
#include "waylandresource.hpp"
#include "../surface.hpp"

class WaylandPointer : public WaylandResource<WaylandPointer, wl_pointer_interface,
        struct wl_pointer_interface>
{
public:
    WaylandPointer(): WaylandResource(&sInterface) {}
    virtual ~WaylandPointer() {}

protected:
    virtual void setCursor(wl_client* /*client*/, wl_resource* /*resource*/, uint32_t /*serial*/,
                   wl_resource* /*surface*/, int32_t /*hotspotX*/, int32_t /*hotspotY*/) { }

    virtual void release(wl_client* /*client*/, wl_resource* /*resource*/) { }

private:
    static struct wl_pointer_interface sInterface;

    static void hookSetCursor(wl_client* client, wl_resource* resource, uint32_t serial,
                              wl_resource* surface, int32_t hotspot_x, int32_t hotspot_y) {
        getThis(resource)->setCursor(client, resource, serial, surface, hotspot_x, hotspot_y);
    }

    static void hookRelease(wl_client* client, wl_resource* resource) {
        getThis(resource)->release(client, resource);
    }
};

#endif // WAYLANDPOINTER_HPP
