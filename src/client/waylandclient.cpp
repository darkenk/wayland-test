#include "waylandclient.hpp"

using namespace client;
using namespace std;

WaylandClient::WaylandClient(const string& socketName)
    : mDisplay(nullptr), mCompositor(nullptr), mSharedMemory(nullptr), mRegistry(nullptr),
      mSurface(nullptr) {
    mRegistryListener.global = WaylandClient::registryHandler;
    mRegistryListener.global_remove = WaylandClient::registryRemover;
    if (socketName.empty()) {
        mDisplay = wl_display_connect(nullptr);
    } else {
        mDisplay = wl_display_connect(socketName.c_str());
    }
    if (not mDisplay) {
        throw WLException("Error opening display");
    }

    mRegistry = wl_display_get_registry(mDisplay);
    wl_registry_add_listener(mRegistry, &mRegistryListener, this);

    wl_display_dispatch(mDisplay);
    wl_display_roundtrip(mDisplay);  // wait synchronously for event

    if (not(mCompositor && mSharedMemory)) {
        throw WLException("cannot find wl_compositor or wl_shm");
    }
}

WaylandClient::~WaylandClient() {
    wl_shell_destroy(mShell);
    wl_shm_destroy(mSharedMemory);
    wl_compositor_destroy(mCompositor);
    wl_registry_destroy(mRegistry);
    wl_display_disconnect(mDisplay);
}

void WaylandClient::createSurface() {
    mSurface = std::make_unique<WaylandSurface>(mCompositor, mSharedMemory);
}

void WaylandClient::run() {
    createSurface();
    while (wl_display_dispatch(mDisplay)) {
    };
}

wl_display *WaylandClient::getDisplay() {
    return mDisplay;
}

wl_shm* WaylandClient::getSharedMemory()
{
    return mSharedMemory;
}

wl_compositor* WaylandClient::getCompositor()
{
    return mCompositor;
}

void WaylandClient::registryHandler(wl_registry *registry, uint32_t id, const char *interface,
                                    uint32_t) {
    LOGVP("Got a registry event for %s id %d\n", interface, id);
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        mCompositor = reinterpret_cast<wl_compositor *>(
            wl_registry_bind(registry, id, &wl_compositor_interface, 1));
    } else if (strcmp(interface, wl_shm_interface.name) == 0) {
        mSharedMemory =
            reinterpret_cast<wl_shm *>(wl_registry_bind(registry, id, &wl_shm_interface, 1));
    } else if (strcmp(interface, wl_shell_interface.name) == 0) {
        mShell =
            reinterpret_cast<wl_shell *>(wl_registry_bind(registry, id, &wl_shell_interface, 1));
    }
}
