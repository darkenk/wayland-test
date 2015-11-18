#ifndef WAYLANDCOMPOSITOR_HPP
#define WAYLANDCOMPOSITOR_HPP

#include <wayland-server.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <cstring>
#include "x11backend.hpp"
#include "../utils/make_unique.hpp"
#include "waylandsurface.hpp"
#include "waylandregion.hpp"

class WaylandCompositor : public DestroyListener<WaylandSurface>
{
public:
    WaylandCompositor(wl_display* display, std::shared_ptr<X11Backend> output) {
        mOutput = output;
        mDisplayWidth = mOutput->getWidth();
        mDisplayHeight = mOutput->getHeight();
        // repaint handler
        wl_event_loop* loop = wl_display_get_event_loop(display);
        mRepaintTimer = wl_event_loop_add_timer(loop, WaylandCompositor::hookRepaintHandler, this);
        wl_event_source_timer_update(mRepaintTimer, REPAINT_DELAY);
        mDisplayBuffer = std::make_unique<uint8_t[]>(mDisplayWidth * mDisplayHeight * 4);
    }

    ~WaylandCompositor() {
        free(mRepaintTimer);
    }

    void createSurface(wl_client* client, wl_resource* resource, uint32_t id) {
        mSurfacesList.push_back(std::make_unique<WaylandSurface>(client, resource, id, this));
    }

    void createRegion(wl_client* client, wl_resource* resource, uint32_t id) {
        LOGVP("Regions are not used :(");
        mRegions.push_back(std::make_unique<WaylandRegion>(client, resource, id));
    }

    int repaint() {
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

    void relayoutSurfaces() {
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

    void bind(wl_client* client, uint32_t version, uint32_t id) {
        LOGVP();
        wl_resource* resource = wl_resource_create(client, &wl_compositor_interface, version, id);
        if (not resource) {
            wl_client_post_no_memory(client);
            return;
        }
        wl_resource_set_implementation(resource, &sInterface, this,
                                       WaylandCompositor::hookClientDisconnects);
    }

    void notify(WaylandSurface* surface) {
        mSurfacesList.erase(std::remove_if(mSurfacesList.begin(), mSurfacesList.end(), [surface]
                       (std::unique_ptr<WaylandSurface>& s){ return surface == s.get();}), mSurfacesList.end());
    }

private:
    static struct wl_compositor_interface sInterface;
    std::vector<std::unique_ptr<WaylandSurface>> mSurfacesList;
    wl_event_source* mRepaintTimer;
    std::shared_ptr<X11Backend> mOutput;
    int32_t mDisplayWidth;
    int32_t mDisplayHeight;
    std::unique_ptr<uint8_t[]> mDisplayBuffer;
    std::vector<std::unique_ptr<WaylandRegion>> mRegions;
    static constexpr int REPAINT_DELAY = 16;
    static constexpr int BYTES_PER_PIXEL = 4;

    static void hookCreateSurface(wl_client* client, wl_resource* resource, uint32_t id) {
        LOGVP();
        WaylandCompositor* wc = reinterpret_cast<WaylandCompositor*>(
                    wl_resource_get_user_data(resource));
        wc->createSurface(client, resource, id);
    }

    static void hookCreateRegion(wl_client* client, wl_resource* resource, uint32_t id) {
        LOGVP();
        WaylandCompositor* wc = reinterpret_cast<WaylandCompositor*>(
                    wl_resource_get_user_data(resource));
        wc->createRegion(client, resource, id);
    }

    static int hookRepaintHandler(void* data) {
        WaylandCompositor* wc = reinterpret_cast<WaylandCompositor*>(data);
        return wc->repaint();
    }

    static void hookClientDisconnects(struct wl_resource *resource) {
        WaylandCompositor* wc = reinterpret_cast<WaylandCompositor*>(
                    wl_resource_get_user_data(resource));
        wc->clientDisconnects(wl_resource_get_client(resource));
    }

    void clientDisconnects(wl_client* client) {
        LOGVP();
        std::remove_if(mSurfacesList.begin(), mSurfacesList.end(), [client]
                       (std::unique_ptr<WaylandSurface>& s){ return client == s->client();});
    }

    void clearDisplay() {
        memset(mDisplayBuffer.get(), 0, mDisplayWidth * mDisplayHeight * BYTES_PER_PIXEL);
    }

    void copyShmBufferToDisplay(wl_shm_buffer* b, int posY = 0, int posX = 0) {
        uint8_t* data = reinterpret_cast<uint8_t*>(wl_shm_buffer_get_data(b));
        int32_t w = wl_shm_buffer_get_width(b);
        int32_t h = wl_shm_buffer_get_height(b);
        int32_t lineSize = mDisplayWidth - posX;
        lineSize = w < lineSize ? w * BYTES_PER_PIXEL : lineSize * BYTES_PER_PIXEL;
        int32_t nrOfLines = (posY + h) < mDisplayHeight ? (posY + h) : mDisplayHeight;
        int32_t offsetSrc = 0;
        if (posX < 0 ) {
            offsetSrc -= posX * BYTES_PER_PIXEL;
            lineSize -= offsetSrc;
            posX = 0;
        }
        int32_t offsetDst = (posY * mDisplayWidth + posX) * BYTES_PER_PIXEL;
        for (int32_t y = posY; y < nrOfLines; y++) {
            blitLine(mDisplayBuffer.get() + offsetDst, data + offsetSrc, lineSize);
            offsetDst += mDisplayWidth * BYTES_PER_PIXEL;
            offsetSrc += w * BYTES_PER_PIXEL;
        }
    }

    void blitLine(uint8_t* dst, uint8_t* src, uint32_t size) {
        for (uint32_t i = 0; i < size; i += 4) {
            alphaBlending(dst + i, src + i);
        }
    }

    void alphaBlending(u_int8_t* bg, u_int8_t* fg) {
        float alphaBg = (255 - *(fg + 3))/255.f;
        float alphaFg = 1.f - alphaBg;
        for (int i = 0; i < 3; i++) {
            bg[i] = static_cast<u_int8_t>(bg[i] * alphaBg + fg[i] * alphaFg);
        }
    }
};

#endif // WAYLANDCOMPOSITOR_HPP
