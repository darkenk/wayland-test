#ifndef WAYLANDBUFFER_HPP
#define WAYLANDBUFFER_HPP

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client.h>
#include "dk_utils/exceptions.hpp"
#include "dk_utils/logger.hpp"
#include "dk_utils/make_unique.hpp"

namespace client {

class WaylandBuffer
{
public:
    using WLException = Exception<WaylandBuffer>;
    WaylandBuffer(wl_shm* shm, uint32_t width, uint32_t height) {
        mBufferListener.release = hookBufferRelease;
        mBuffer = createShmBuffer(shm, width, height);
        wl_buffer_add_listener(mBuffer, &mBufferListener, this);
        mReadyToDraw = true;
    }
    virtual ~WaylandBuffer() {}

    uint8_t* data() {
        return mData;
    }

    wl_buffer* buffer() {
        return mBuffer;
    }

    bool readyToDraw() {
        return mReadyToDraw;
    }

    void setReadyToDraw(bool ready) {
        mReadyToDraw = ready;
    }

private:
    wl_buffer* mBuffer;
    wl_buffer_listener mBufferListener;
    bool mReadyToDraw;
    uint8_t* mData;

    wl_buffer* createShmBuffer(wl_shm* shm, uint32_t width, uint32_t height) {
        int32_t stride = width * 4;
        int32_t size = stride * height;
        int fd = createAnonymousFile(size);
        mData = reinterpret_cast<uint8_t*>(
            mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
        if (mData == MAP_FAILED) {
            close(fd);
            throw WLException("Cannot create shm buffer");
        }
        wl_shm_pool* pool = wl_shm_create_pool(shm, fd, size);
        wl_buffer* buff =
            wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);
        wl_shm_pool_destroy(pool);
        close(fd);
        return buff;
    }

    void release(wl_buffer* /*buffer*/) {
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

    static void hookBufferRelease(void* data, struct wl_buffer* buffer) {
        reinterpret_cast<WaylandBuffer*>(data)->release(buffer);
    }
};
}

#endif  // WAYLANDBUFFER_HPP
