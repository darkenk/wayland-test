#ifndef WAYLANDSERVER_HPP
#define WAYLANDSERVER_HPP

#include <wayland-server.h>
#include <exception>
#include <memory>
#include "../utils/make_unique.hpp"
#include "x11backend.hpp"
#include "waylandcompositor.hpp"
#include "waylandshell.hpp"
#include "waylandoutput.hpp"
#include <utility>

class WaylandServer
{
public:
    WaylandServer(std::unique_ptr<X11Backend> backend) {
        mDisplay = wl_display_create();
        if (not mDisplay) {
            throw std::exception();
        }
        mBackend = std::move(backend);
        mBackend->init(mDisplay);
        mCompositor = std::make_unique<WaylandCompositor>(mDisplay, mBackend);
        mShell = std::make_unique<WaylandShell>();
        mOutput = std::make_unique<WaylandOutput>();
        const char* socketName = wl_display_add_socket_auto(mDisplay);
        LOGVP("Socket Name %s", socketName);
        if (not wl_global_create(mDisplay, &wl_compositor_interface, 3, mCompositor.get(),
                                 WaylandServer::hookBind<WaylandCompositor>)) {
            throw std::exception();
        }
        wl_display_init_shm(mDisplay);
        if (not wl_global_create(mDisplay, &wl_shell_interface, 1, mShell.get(),
                                 WaylandServer::hookBind<WaylandShell>)) {
            throw std::exception();
        }

        if (not wl_global_create(mDisplay, &wl_output_interface, 1, mOutput.get(),
                                 WaylandServer::hookBind<WaylandOutput>)) {
            throw std::exception();
        }
    }

    ~WaylandServer() {
        wl_display_terminate(mDisplay);
        wl_display_destroy(mDisplay);
        mDisplay = nullptr;
        mBackend.reset();
    }

    void run() {
        wl_display_run(mDisplay);
    }

private:
    wl_display* mDisplay;
    std::shared_ptr<X11Backend> mBackend;
    std::unique_ptr<WaylandCompositor> mCompositor;
    std::unique_ptr<WaylandShell> mShell;
    std::unique_ptr<WaylandOutput> mOutput;

    template<class T>
    static void hookBind(wl_client* client, void* data, uint32_t version, uint32_t id) {
        T* object = reinterpret_cast<T*>(data);
        object->bind(client, version, id);
    }
};

#endif // WAYLANDSERVER_HPP
