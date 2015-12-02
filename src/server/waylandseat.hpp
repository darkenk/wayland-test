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

class WaylandSeat : public WaylandGlobalObject<WaylandSeat, wl_seat_interface,
        struct wl_seat_interface>, public PointerListener
{
public:
    WaylandSeat(): WaylandGlobalObject(&sInterface) {}

    void move(int x, int y) {
        for (auto& p : mPointerList) {
            p->move(x, y);
        }
    }

private:
    static const struct wl_seat_interface sInterface;
    std::vector<std::unique_ptr<WaylandPointer>> mPointerList;

    void getPointer(wl_client* client, wl_resource* /*resource*/, uint32_t id) {
        mPointerList.push_back(std::make_unique<WaylandPointer>(client, id));
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
