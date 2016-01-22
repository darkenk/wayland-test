#include "dk_utils/exceptions.hpp"
#include "dk_utils/logger.hpp"
#include "dk_utils/make_unique.hpp"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client.h>

using namespace std;

class WaylandBuffer
{
public:
    using WLException = Exception<WaylandBuffer>;
    WaylandBuffer(wl_shm *shm, uint32_t width, uint32_t height) {
        mBufferListener.release = hookBufferRelease;
        mBuffer = createShmBuffer(shm, width, height);
        wl_buffer_add_listener(mBuffer, &mBufferListener, this);
        mReadyToDraw = true;
    }
    virtual ~WaylandBuffer() {}

    uint8_t *data() {
        return mData;
    }

    wl_buffer *buffer() {
        return mBuffer;
    }

    bool readyToDraw() {
        return mReadyToDraw;
    }

    void setReadyToDraw(bool ready) {
        mReadyToDraw = ready;
    }

private:
    wl_buffer *mBuffer;
    wl_buffer_listener mBufferListener;
    bool mReadyToDraw;
    uint8_t *mData;

    wl_buffer *createShmBuffer(wl_shm *shm, uint32_t width, uint32_t height) {
        int32_t stride = width * 4;
        int32_t size = stride * height;
        int fd = createAnonymousFile(size);
        mData = reinterpret_cast<uint8_t *>(
            mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
        if (mData == MAP_FAILED) {
            close(fd);
            throw WLException("Cannot create shm buffer");
        }
        wl_shm_pool *pool = wl_shm_create_pool(shm, fd, size);
        wl_buffer *buff =
            wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);
        wl_shm_pool_destroy(pool);
        close(fd);
        return buff;
    }

    void release(wl_buffer * /*buffer*/) {
        LOGVP("");
        mReadyToDraw = true;
    }

    static int createAnonymousFile(off_t size) {
        char tmpname[] = "XXXXXX";
        int fd = mkostemp(tmpname, O_CLOEXEC);
        if (fd >= 0) {
            unlink(tmpname);
        } else {
            throw WLException("Cannot create anonymous file");
        }
        if (posix_fallocate(fd, 0, size) != 0) {
            close(fd);
            throw WLException("Cannot allocate anonymous file");
        }
        return fd;
    }

    static void hookBufferRelease(void *data, struct wl_buffer *buffer) {
        reinterpret_cast<WaylandBuffer *>(data)->release(buffer);
    }
};

class WaylandSurface
{
public:
    WaylandSurface(wl_compositor *compositor, wl_shm *sharedMemory)
        : mSharedMemory(sharedMemory), mWidth(480), mHeight(360) {
        mFrameListener.done = hookFrameDone;
        mSurface = wl_compositor_create_surface(compositor);
        mBuffer = make_unique<WaylandBuffer>(mSharedMemory, mWidth, mHeight);
        redraw(nullptr, 0);
    }

    virtual ~WaylandSurface() {}

    void redraw(wl_callback *callback, uint32_t /*time*/) {
        if (not mBuffer->readyToDraw()) {
            return;
        }
        // just paint something white
        static uint8_t color = 0x00;
        color++;
        uint8_t *data = mBuffer->data();
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
    wl_shm *mSharedMemory;
    wl_surface *mSurface;
    wl_callback *mFrameCallback;
    wl_callback_listener mFrameListener;
    unique_ptr<WaylandBuffer> mBuffer;
    int mWidth;
    int mHeight;

    static void hookFrameDone(void *data, wl_callback *callback, uint32_t time) {
        reinterpret_cast<WaylandSurface *>(data)->redraw(callback, time);
    }
};

class WaylandClient
{
public:
    using WLException = Exception<WaylandClient>;
    WaylandClient()
        : mDisplay(nullptr), mCompositor(nullptr), mSharedMemory(nullptr), mSurface(nullptr) {
        mRegistryListener.global = WaylandClient::registryHandler;
        mRegistryListener.global_remove = WaylandClient::registryRemover;
        mDisplay = wl_display_connect(nullptr);
        if (not mDisplay) {
            throw WLException("Error opening display");
        }

        wl_registry *registry = wl_display_get_registry(mDisplay);
        wl_registry_add_listener(registry, &mRegistryListener, this);

        wl_display_dispatch(mDisplay);
        wl_display_roundtrip(mDisplay);  // wait synchronously for event

        if (not(mCompositor && mSharedMemory)) {
            throw WLException("cannot find wl_compositor or wl_shm");
        }
    }

    void createSurface() {
        mSurface = make_unique<WaylandSurface>(mCompositor, mSharedMemory);
    }

    virtual ~WaylandClient() {
        wl_display_disconnect(mDisplay);
    }

    void run() {
        createSurface();
        while (wl_display_dispatch(mDisplay)) {
        };
    }

private:
    wl_display *mDisplay;
    wl_compositor *mCompositor;
    wl_shm *mSharedMemory;
    wl_shell *mShell;
    wl_registry_listener mRegistryListener;
    unique_ptr<WaylandSurface> mSurface;

    void registryHandler(wl_registry *registry, uint32_t id, const char *interface,
                         uint32_t /*version*/) {
        LOGVP("Got a registry event for %s id %d\n", interface, id);
        if (strcmp(interface, wl_compositor_interface.name) == 0) {
            mCompositor = reinterpret_cast<wl_compositor *>(
                wl_registry_bind(registry, id, &wl_compositor_interface, 1));
        } else if (strcmp(interface, wl_shm_interface.name) == 0) {
            mSharedMemory =
                reinterpret_cast<wl_shm *>(wl_registry_bind(registry, id, &wl_shm_interface, 1));
        } else if (strcmp(interface, wl_shell_interface.name) == 0) {
            mShell = reinterpret_cast<wl_shell *>(
                wl_registry_bind(registry, id, &wl_shell_interface, 1));
        }
    }

    static void registryHandler(void *data, wl_registry *registry, uint32_t id,
                                const char *interface, uint32_t version) {
        reinterpret_cast<WaylandClient *>(data)->registryHandler(registry, id, interface, version);
    }

    static void registryRemover(void * /*data*/, struct wl_registry * /*registry*/, uint32_t id) {
        LOGVP("Got a registry losing event for %d\n", id);
    }
};

int main() {
    WaylandClient().run();
    return 0;
}
