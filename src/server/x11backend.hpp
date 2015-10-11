#ifndef X11BACKEND_HPP
#define X11BACKEND_HPP

#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <X11/Xutil.h>
#include "../utils/exceptions.hpp"
#include "../utils/logger.hpp"
#include "wayland-server.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"

class X11Backend
{
public:
    X11Backend(uint32_t width, uint32_t height, wl_display* display) :
        mWidth(width), mHeight(height), mWlDisplay(display) {
        mDisplay = XOpenDisplay(nullptr);
        if (not mDisplay) {
            Exception(__FUNCTION__);
        }
        mRoot = DefaultRootWindow(mDisplay);
        mWindow = XCreateSimpleWindow(mDisplay, mRoot, 0, 0, mWidth, mHeight, 0, 0, 0);
        mWmDeleteWindow = XInternAtom(mDisplay, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(mDisplay, mWindow, &mWmDeleteWindow, 1);
        XSizeHints* sizeHints = XAllocSizeHints();
        sizeHints->flags = PMinSize | PMaxSize;
        sizeHints->min_width = sizeHints->max_width = static_cast<int>(mWidth);
        sizeHints->min_height = sizeHints->max_height = static_cast<int>(mHeight);
        XSetWMNormalHints(mDisplay, mWindow, sizeHints);
        XFree(sizeHints);
        XMapWindow(mDisplay, mWindow);
        XFlush(mDisplay);
        mXcbConnection = XGetXCBConnection(mDisplay);
        mPixmap = XCreatePixmap(mDisplay, mWindow, mWidth, mHeight, sDepth);
        XGCValues gcvalues;
        mGraphicContext = XCreateGC(mDisplay, mPixmap, 0, &gcvalues);
        mImage = XCreateImage(mDisplay, nullptr, sDepth, ZPixmap, 0, nullptr, mWidth, mHeight,
                              32, 0);
        if (not mImage) {
            throw Exception("Can't create XImage");
        }
        mXcbSource = wl_event_loop_add_fd(wl_display_get_event_loop(display),
                                          xcb_get_file_descriptor(mXcbConnection), WL_EVENT_READABLE,
                                          X11Backend::hookHandleX11Events, this);
        wl_event_source_check(mXcbSource);
    }

    ~X11Backend() {
        XDestroyImage(mImage);
        mImage = nullptr;
        XFreeGC(mDisplay, mGraphicContext);
        XCloseDisplay(mDisplay);
        mDisplay = nullptr;
    }

    uint32_t getWidth() {
        return mWidth;
    }

    uint32_t getHeight() {
        return mHeight;
    }

    void setSize(uint32_t width, uint32_t height) {
        if (mWidth == width && mHeight == height) {
            return;
        }
        LOGVP("This doesn't work, need true implementation here!");
        mWidth = width;
        mHeight = height;
    }

    void drawBuffer(uint8_t* buffer) {
        mImage->data = reinterpret_cast<char*>(buffer);
        XPutImage(mDisplay, mPixmap, mGraphicContext, mImage, 0, 0, 0, 0, mWidth, mHeight);
        XSetWindowBackgroundPixmap(mDisplay, mWindow, mPixmap);
        XClearWindow(mDisplay, mWindow);
        XFlush(mDisplay);
        mImage->data = nullptr;
    }

private:
    static constexpr int sDepth = 24;
    Pixmap mPixmap;
    Display* mDisplay;
    Window mRoot;
    Window mWindow;
    GC mGraphicContext;
    uint32_t mWidth;
    uint32_t mHeight;
    XImage* mImage;
    wl_event_source* mXcbSource;
    xcb_connection_t* mXcbConnection;
    Atom mWmDeleteWindow;
    wl_display* mWlDisplay;

    static int hookHandleX11Events(int /*fd*/, uint32_t /*mask*/, void* data) {
        return reinterpret_cast<X11Backend*>(data)->handleX11Events();
    }

    int handleX11Events() {
        xcb_generic_event_t* event = xcb_poll_for_event(mXcbConnection);
        if (not event) {
            return 0;
        }
        switch(event->response_type & ~0x80) {
        case XCB_CLIENT_MESSAGE:
        {
            xcb_client_message_event_t* t = reinterpret_cast<xcb_client_message_event_t*>(event);
            if (t->data.data32[0] == mWmDeleteWindow) {
                LOGVP("Destroy me");
                wl_event_source_remove(mXcbSource);
                XDestroyWindow(mDisplay, mWindow);
                XFlush(mDisplay);
                wl_display_terminate(mWlDisplay);
            }
            break;
        }
        default:
            LOGVP("Not handled %d", event->response_type & ~0x80);
            break;
        }
        free(event);
        return 1;
    }
};

#pragma GCC diagnostic pop

#endif // X11BACKEND_HPP
