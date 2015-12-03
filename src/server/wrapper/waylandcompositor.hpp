#ifndef WAYLANDCOMPOSITOR_HPP
#define WAYLANDCOMPOSITOR_HPP

#include "waylandglobalobject.hpp"

class WaylandCompositor : public WaylandGlobalObject<WaylandCompositor, wl_compositor_interface,
        struct wl_compositor_interface>
{
public:
    WaylandCompositor(): WaylandGlobalObject(&sInterface) {}

    virtual ~WaylandCompositor() { }

protected:
    virtual void createSurface(wl_client* /*client*/, wl_resource* /*resource*/, uint32_t /*id*/) { }

    virtual void createRegion(wl_client* /*client*/, wl_resource* /*resource*/, uint32_t /*id*/) { }

private:
    static const struct wl_compositor_interface sInterface;

    static void hookCreateSurface(wl_client* client, wl_resource* resource, uint32_t id) {
        getThis(resource)->createSurface(client, resource, id);
    }

    static void hookCreateRegion(wl_client* client, wl_resource* resource, uint32_t id) {
        getThis(resource)->createRegion(client, resource, id);
    }
};

#endif // WAYLANDCOMPOSITOR_HPP
