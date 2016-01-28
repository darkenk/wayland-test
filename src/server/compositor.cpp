#include "compositor.hpp"

Compositor::Compositor(wl_display* display, std::shared_ptr<IDisplayBackend> output) {
    mOutput = output;
    mDisplayWidth = mOutput->getWidth();
    mDisplayHeight = mOutput->getHeight();
    // repaint handler
    wl_event_loop* loop = wl_display_get_event_loop(display);
    mRepaintTimer = wl_event_loop_add_timer(loop, hookRepaintHandler, this);
    wl_event_source_timer_update(mRepaintTimer, REPAINT_DELAY);
    mDisplayBuffer = std::make_unique<uint8_t[]>(mDisplayWidth * mDisplayHeight * 4);
}

Compositor::~Compositor() {
    free(mRepaintTimer);
}

int Compositor::repaint() {
    relayoutSurfaces();
    clearDisplay();
    for (auto& surface : mSurfacesList) {
        if (surface && surface->isReady()) {
            wl_shm_buffer* b = surface->lockBuffer();
            if (b) {
                copyShmBufferToDisplay(b, surface->y(), surface->x());
            }
            surface->unlockBuffer();
        }
    }
    mOutput->drawBuffer(mDisplayBuffer.get());
    wl_event_source_timer_update(mRepaintTimer, REPAINT_DELAY);
    return 0;
}

void Compositor::relayoutSurfaces() {
    int count = 0;
    for (auto& s : mSurfacesList) {
        if (s->isShellSurface()) {
            count++;
        }
    }
    if (not count) {
        return;
    }
    int height = mDisplayHeight / count;
    int y = 0;
    for (auto& s : mSurfacesList) {
        if (s->isShellSurface()) {
            s->setY(y);
            y += height;
        }
    }
}
