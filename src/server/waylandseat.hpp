#ifndef WAYLANDSEAT_HPP
#define WAYLANDSEAT_HPP

#include <wayland-server.h>
#include "../utils/logger.hpp"
#include "../utils/make_unique.hpp"
#include <vector>
#include "waylandpointer.hpp"

class PointerListener
{
public:
    virtual void move(int x, int y) = 0;
    virtual ~PointerListener() {}
};

class WaylandSeat : public PointerListener,
        public WaylandGlobalObject<WaylandSeat, wl_seat_interface, struct wl_seat_interface>
{
public:
    WaylandSeat(wl_display* display): WaylandGlobalObject(display) {}

    void move(int x, int y) {
        for (auto& p : mPointerList) {
            p->move(x, y);
        }
    }

protected:
    static const struct wl_seat_interface* getInterface() {
        return &sInterface;
    }

private:
    static const struct wl_seat_interface sInterface;
    friend WaylandGlobalObject<WaylandSeat, wl_seat_interface, struct wl_seat_interface>;
    std::vector<std::unique_ptr<WaylandPointer>> mPointerList;

    void getPointer(wl_client* client, wl_resource* resource, uint32_t id) {
        mPointerList.push_back(std::make_unique<WaylandPointer>(client, resource, id));
    }

    static void hookGetPointer(wl_client* client, wl_resource* resource, uint32_t id) {
        reinterpret_cast<WaylandSeat*>(wl_resource_get_user_data(resource))->
                getPointer(client, resource, id);
    }

    static void hookGetKeyboard(wl_client* /*client*/, wl_resource* /*resource*/, uint32_t /*id*/) {

    }

    static void hookGetTouch(wl_client* /*client*/, wl_resource* /*resource*/, uint32_t /*id*/) {

    }
};

#endif // WAYLANDSEAT_HPP
