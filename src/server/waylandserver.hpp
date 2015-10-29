#ifndef WAYLANDSERVER_HPP
#define WAYLANDSERVER_HPP

#include <wayland-server.h>
#include <exception>
#include <memory>
#include "../utils/make_unique.hpp"
#include "x11backend.hpp"
#include "waylandcompositor.hpp"
#include "waylandshell.hpp"

class WaylandServer
{
public:
    WaylandServer() {
        mDisplay = wl_display_create();
        if (not mDisplay) {
            throw std::exception();
        }
        mOutput = new X11Backend(480, 360, mDisplay);
        mCompositor = std::make_unique<WaylandCompositor>(mDisplay, mOutput);
        mShell = std::make_unique<WaylandShell>();
        const char* socketName = wl_display_add_socket_auto(mDisplay);
        LOGVP("Socket Name %s", socketName);
        if (not wl_global_create(mDisplay, &wl_compositor_interface, 3, mCompositor.get(),
                                 WaylandServer::bindCompositor)) {
            throw std::exception();
        }
        wl_display_init_shm(mDisplay);
        if (not wl_global_create(mDisplay, &wl_shell_interface, 1, mShell.get(),
                                 WaylandServer::bindShell)) {
            throw std::exception();
        }
    }

    ~WaylandServer() {
        wl_display_terminate(mDisplay);
        wl_display_destroy(mDisplay);
        mDisplay = nullptr;
        delete mOutput;
        mOutput = nullptr;
    }

    void run() {
        wl_display_run(mDisplay);
    }

private:
    wl_display* mDisplay;
    std::unique_ptr<WaylandCompositor> mCompositor;
    X11Backend* mOutput;
    std::unique_ptr<WaylandShell> mShell;

    static void bindCompositor(wl_client* client, void* data, uint32_t version, uint32_t id) {
        LOGVP();
        WaylandCompositor* compositor = reinterpret_cast<WaylandCompositor*>(data);
        compositor->bind(client, version, id);
    }

    static void bindShell(wl_client* client, void* data, uint32_t version, uint32_t id) {
        LOGVP();
        WaylandShell* shell = reinterpret_cast<WaylandShell*>(data);
        shell->bind(client, version, id);
    }
};

#endif // WAYLANDSERVER_HPP
