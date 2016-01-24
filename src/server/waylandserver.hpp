#ifndef WAYLANDSERVER_HPP
#define WAYLANDSERVER_HPP

#include <exception>
#include <memory>
#include <utility>
#include <wayland-server.h>
#include "backend/idisplaybackend.hpp"
#include "compositor.hpp"
#include "dk_utils/make_unique.hpp"
#include "seat.hpp"
#include "shell.hpp"
#include "wrapper/waylandoutput.hpp"

class WaylandServer
{
public:
    WaylandServer(std::unique_ptr<IDisplayBackend> backend, const std::string& socketName = "") {
        mDisplay = wl_display_create();
        if (not mDisplay) {
            throw std::exception();
        }
        mBackend = std::move(backend);
        mBackend->init(mDisplay);
        if (socketName.empty()) {
            wl_display_add_socket_auto(mDisplay);
        } else {
            wl_display_add_socket(mDisplay, socketName.c_str());
        }
        FactoryWaylandGlobalObject f(mDisplay);
        mCompositor = f.create<Compositor>(mDisplay, mBackend);
        mShell = f.create<Shell>();
        mOutput = f.create<WaylandOutput>();
        mSeat = f.create<Seat>();
        wl_display_init_shm(mDisplay);
        // mBackend->setPointerListener(std::static_pointer_cast<PointerListener>(mSeat));
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
    std::shared_ptr<IDisplayBackend> mBackend;
    std::unique_ptr<Compositor> mCompositor;
    std::unique_ptr<Shell> mShell;
    std::unique_ptr<WaylandOutput> mOutput;
    std::shared_ptr<Seat> mSeat;
};

#endif  // WAYLANDSERVER_HPP
