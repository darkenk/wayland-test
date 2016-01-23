#ifndef IDISLAYBACKEND_HPP
#define IDISLAYBACKEND_HPP

#include <wayland-server.h>

class IDisplayBackend
{
public:
    IDisplayBackend(unsigned int width, unsigned int height);
    virtual ~IDisplayBackend() = default;

    virtual void init(wl_display* display) = 0;

    unsigned int getHeight() const;
    unsigned int getWidth() const;

    virtual void drawBuffer(uint8_t* buffer) = 0;

private:
    unsigned int mWidth;
    unsigned int mHeight;
};

#endif // IDISLAYBACKEND_HPP
