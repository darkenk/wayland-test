#ifndef POINTER_HPP
#define POINTER_HPP

#include "wrapper/waylandpointer.hpp"

class Pointer : public WaylandPointer
{
public:
    Pointer() : WaylandPointer() {}

    void move(int x, int y) {
        if (not mSurface) {
            return;
        }
        mSurface->setX(x - mHotspotX);
        mSurface->setY(y - mHotspotY);
    }

protected:
    virtual void setCursor(wl_client* /*client*/, wl_resource* /*resource*/, uint32_t /*serial*/,
                           wl_resource* surface, int32_t hotspotX, int32_t hotspotY) {
        mSurface = reinterpret_cast<Surface*>(wl_resource_get_user_data(surface));
        mHotspotX = hotspotX;
        mHotspotY = hotspotY;
    }

private:
    Surface* mSurface;
    int32_t mHotspotX;
    int32_t mHotspotY;
};

#endif  // POINTER_HPP
