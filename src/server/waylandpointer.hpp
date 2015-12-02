#ifndef WAYLANDPOINTER_HPP
#define WAYLANDPOINTER_HPP

#include <wayland-server.h>
#include "../utils/logger.hpp"
#include "../utils/make_unique.hpp"
#include <memory>
#include "waylandshell.hpp"
#include "waylandresource.hpp"

class WaylandPointer : public WaylandResource<WaylandPointer, wl_pointer_interface,
        struct wl_pointer_interface>
{
public:
    WaylandPointer(wl_client* client, uint32_t id) :
        WaylandResource(this, &sInterface, client, id),
        mSurface(nullptr), mHotspotX(0), mHotspotY(0) {
    }

    void move(int x, int y) {
        if (not mSurface) {
            return;
        }
        mSurface->setX(x - mHotspotX);
        mSurface->setY(y - mHotspotY);
    }

private:
    static struct wl_pointer_interface sInterface;
    WaylandSurface* mSurface;
    int32_t mHotspotX;
    int32_t mHotspotY;

    void setCursor(wl_client* /*client*/, wl_resource* /*resource*/, uint32_t /*serial*/,
                   wl_resource* surface, int32_t hotspotX, int32_t hotspotY) {
        mSurface = reinterpret_cast<WaylandSurface*>(wl_resource_get_user_data(surface));
        mHotspotX = hotspotX;
        mHotspotY = hotspotY;

    }

    static void hookSetCursor(wl_client* client, wl_resource* resource, uint32_t serial,
                              wl_resource* surface, int32_t hotspot_x, int32_t hotspot_y) {
        LOGVP();
        WaylandPointer* wp = reinterpret_cast<WaylandPointer*>(wl_resource_get_user_data(resource));
        wp->setCursor(client, resource, serial, surface, hotspot_x, hotspot_y);
    }

    static void hookRelease(wl_client* /*client*/, wl_resource* /*resource*/) {

    }
};

#endif // WAYLANDPOINTER_HPP
