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

class WaylandSeat : public PointerListener
{
public:
    WaylandSeat() {}

    void bind(wl_client* client, uint32_t version, uint32_t id) {
        LOGVP();
        wl_resource* resource = wl_resource_create(client, &wl_seat_interface, version, id);
        if (not resource) {
            wl_client_post_no_memory(client);
            return;
        }
        wl_resource_set_implementation(resource, &sInterface, this, nullptr);

        wl_seat_send_capabilities(resource, WL_SEAT_CAPABILITY_POINTER);
        wl_seat_send_name(resource, "DK_seat");
    }

    void move(int x, int y) {
        for (auto& p : mPointerList) {
            p->move(x, y);
        }
    }

private:
    static struct wl_seat_interface sInterface;
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
