#ifndef SEAT_HPP
#define SEAT_HPP

#include "wrapper/waylandseat.hpp"
#include <vector>
#include "pointer.hpp"
#include <memory>

class PointerListener
{
public:
    virtual void move(int x, int y) = 0;
    virtual ~PointerListener() {}
};

class Seat : public WaylandSeat, public PointerListener
{
public:
    Seat() {}

    void move(int x, int y) {
        for (auto& p : mPointerList) {
            p->move(x, y);
        }
    }

protected:
    virtual void getPointer(wl_client* client, wl_resource* /*resource*/, uint32_t id) {
        auto p = WaylandResourceFactory::create<Pointer>(client, id);
        mPointerList.push_back(std::unique_ptr<Pointer>(p));
    }

private:
    std::vector<std::unique_ptr<Pointer>> mPointerList;
};

#endif // SEAT_HPP
