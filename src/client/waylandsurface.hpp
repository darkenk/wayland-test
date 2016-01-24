#ifndef WAYLANDSURFACE_HPP
#define WAYLANDSURFACE_HPP

#include <cstring>
#include <wayland-client.h>
#include "dk_utils/exceptions.hpp"
#include "dk_utils/logger.hpp"
#include "dk_utils/make_unique.hpp"
#include "waylandbuffer.hpp"

namespace client {

class WaylandSurface
{
public:
    WaylandSurface(wl_compositor* compositor, wl_shm* sharedMemory)
        : mSharedMemory(sharedMemory), mWidth(480), mHeight(360) {
        mFrameListener.done = hookFrameDone;
        mSurface = wl_compositor_create_surface(compositor);
        mBuffer = std::make_unique<WaylandBuffer>(mSharedMemory, mWidth, mHeight);
        redraw(nullptr, 0);
    }

    virtual ~WaylandSurface() {}

    void redraw(wl_callback* callback, uint32_t /*time*/) {
        if (not mBuffer->readyToDraw()) {
            return;
        }
        // just paint something white
        static uint8_t color = 0x00;
        color++;
        uint8_t* data = mBuffer->data();
        memset(data, color, mWidth * mHeight * 4);

        wl_surface_attach(mSurface, mBuffer->buffer(), 0, 0);
        wl_surface_damage(mSurface, 0, 0, mWidth, mHeight);
        if (callback) {
            wl_callback_destroy(callback);
        }

        mFrameCallback = wl_surface_frame(mSurface);
        wl_callback_add_listener(mFrameCallback, &mFrameListener, this);
        wl_surface_commit(mSurface);
        mBuffer->setReadyToDraw(false);
    }

private:
    wl_shm* mSharedMemory;
    wl_surface* mSurface;
    wl_callback* mFrameCallback;
    wl_callback_listener mFrameListener;
    std::unique_ptr<WaylandBuffer> mBuffer;
    int mWidth;
    int mHeight;

    static void hookFrameDone(void* data, wl_callback* callback, uint32_t time) {
        reinterpret_cast<WaylandSurface*>(data)->redraw(callback, time);
    }
};
}

#endif  // WAYLANDSURFACE_HPP
