#ifndef WAYLANDSEAT_HPP
#define WAYLANDSEAT_HPP

#include "waylandglobalobject.hpp"

class WaylandSeat
    : public WaylandGlobalObject<WaylandSeat, wl_seat_interface, struct wl_seat_interface>
{
public:
    WaylandSeat() : WaylandGlobalObject(&sInterface) {}

protected:
    virtual void getPointer(wl_client* /*client*/, wl_resource* /*resource*/, uint32_t /*id*/) {}

private:
    static const struct wl_seat_interface sInterface;

    static void hookGetPointer(wl_client* client, wl_resource* resource, uint32_t id) {
        getThis(resource)->getPointer(client, resource, id);
    }

    static void hookGetKeyboard(wl_client* /*client*/, wl_resource* /*resource*/, uint32_t /*id*/) {
    }

    static void hookGetTouch(wl_client* /*client*/, wl_resource* /*resource*/, uint32_t /*id*/) {}
};

#endif  // WAYLANDSEAT_HPP
