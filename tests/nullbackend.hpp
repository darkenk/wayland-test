#ifndef NULLBACKEND_HPP
#define NULLBACKEND_HPP

#include <wayland-server.h>
#include "server/backend/idisplaybackend.hpp"

class NullBackend : public IDisplayBackend
{
public:
    NullBackend(unsigned int width, unsigned int height, int killTime = 1)
        : IDisplayBackend(width, height), mDisplay(nullptr), mKillTimer(nullptr),
          mKillTime(killTime) {}

    virtual ~NullBackend(){};

    virtual void init(wl_display* display) {
        mDisplay = display;
    }

    virtual void drawBuffer(uint8_t* /*buffer*/) {}

    void finish() {
        wl_event_loop* loop = wl_display_get_event_loop(mDisplay);
        mKillTimer = wl_event_loop_add_timer(loop, hookKillTime, this);
        wl_event_source_timer_update(mKillTimer, mKillTime);
    }

private:
    wl_display* mDisplay;
    wl_event_source* mKillTimer;
    int mKillTime;

    void kill() {
        wl_display_terminate(mDisplay);
    }

    static int hookKillTime(void* data) {
        auto p = reinterpret_cast<NullBackend*>(data);
        p->kill();
        return 0;
    }
};

#endif  // NULLBACKEND_HPP
