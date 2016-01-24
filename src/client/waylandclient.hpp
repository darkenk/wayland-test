#ifndef WAYLANDCLIENT_HPP
#define WAYLANDCLIENT_HPP

#include <wayland-client.h>
#include "dk_utils/exceptions.hpp"
#include "dk_utils/logger.hpp"
#include "dk_utils/make_unique.hpp"
#include "waylandsurface.hpp"

namespace client {

class WaylandClient
{
public:
    using WLException = Exception<WaylandClient>;

    WaylandClient(const std::string& socketName = "");
    virtual ~WaylandClient();

    void createSurface();
    void run();

protected:
    wl_display *getDisplay();

private:
    wl_display *mDisplay;
    wl_compositor *mCompositor;
    wl_shm *mSharedMemory;
    wl_shell *mShell;
    wl_registry_listener mRegistryListener;
    std::unique_ptr<WaylandSurface> mSurface;

    void registryHandler(wl_registry *registry, uint32_t id, const char *interface,
                         uint32_t /*version*/);

    static void registryHandler(void *data, wl_registry *registry, uint32_t id,
                                const char *interface, uint32_t version) {
        reinterpret_cast<WaylandClient *>(data)->registryHandler(registry, id, interface, version);
    }

    static void registryRemover(void * /*data*/, struct wl_registry * /*registry*/, uint32_t id) {
        LOGVP("Got a registry losing event for %d\n", id);
    }
};
}

#endif  // WAYLANDCLIENT_HPP
