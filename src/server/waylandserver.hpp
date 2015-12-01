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
#include "waylandseat.hpp"
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
        const char* socketName = wl_display_add_socket_auto(mDisplay);
        LOGVP("Socket Name %s", socketName);

        FactoryWaylandGlobalObject f(mDisplay);
        mCompositor = f.create<WaylandCompositor>(mDisplay, mBackend);
        mShell = f.create<WaylandShell>();
        mOutput = f.create<WaylandOutput>();
        mSeat = f.create<WaylandSeat>();
        wl_display_init_shm(mDisplay);
        mBackend->setPointerListener(std::static_pointer_cast<PointerListener>(mSeat));
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
    std::shared_ptr<WaylandSeat> mSeat;
};

#endif // WAYLANDSERVER_HPP
