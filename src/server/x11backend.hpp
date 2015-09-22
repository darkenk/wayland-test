#ifndef X11BACKEND_HPP
#define X11BACKEND_HPP

#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <X11/Xutil.h>
#include "../utils/exceptions.hpp"
#include "../utils/logger.hpp"
#include "wayland-server.h"

class X11Backend
{
public:
    X11Backend(uint32_t width, uint32_t height) {
        mWidth = 480;
        mHeight = 360;
        mDisplay = XOpenDisplay(NULL);
        if (not mDisplay) {
            Exception(__FUNCTION__);
        }
        mRoot = DefaultRootWindow(mDisplay);
        mWindow = XCreateSimpleWindow(mDisplay, mRoot, 0, 0, mWidth, mHeight, 0, 0, 0);
        XMapWindow(mDisplay, mWindow);
        XFlush(mDisplay);
        mPixmap = XCreatePixmap(mDisplay, mWindow, mWidth, mHeight, sDepth);
        XGCValues gcvalues;
        mGraphicContext = XCreateGC(mDisplay, mPixmap, 0, &gcvalues);
        mImage = XCreateImage(mDisplay, nullptr, sDepth, ZPixmap, 0, nullptr, mWidth, mHeight,
                              32, 0);
        if (not mImage) {
            throw Exception("Can't create XImage");
        }
    }

    Display* getDisplay() { return mDisplay; }
    Window getWindow() { return mWindow; }

    void setSize(uint32_t width, uint32_t height) {
        if (mWidth == width && mHeight == height) {
            return;
        }
        LOGVP("This doesn't work, need true implementation here!");
        mWidth = width;
        mHeight = height;
    }

    void drawBuffer(uint32_t* buffer) {
        mImage->data = reinterpret_cast<char*>(buffer);
        XPutImage(mDisplay, mPixmap, mGraphicContext, mImage, 0, 0, 0, 0, mWidth, mHeight);
        XSetWindowBackgroundPixmap(mDisplay, mWindow, mPixmap);
        XClearWindow(mDisplay, mWindow);
        XFlush(mDisplay);
        mImage->data = nullptr;
    }

    void addToLoop(wl_event_loop* loop) {
        mXcbConnection = XGetXCBConnection(mDisplay);
        XSetEventQueueOwner(mDisplay, XCBOwnsEventQueue);
        mXcbSource = wl_event_loop_add_fd(loop, xcb_get_file_descriptor(mXcbConnection), WL_EVENT_READABLE,
                             X11Backend::hookHandleX11Events, this);
        wl_event_source_check(mXcbSource);
    }

    int handleX11Events() {
        xcb_generic_event_t* event = xcb_poll_for_event(mXcbConnection);
        //LOGVP("handle %d", event->response_type);
        return 0;
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

    static int hookHandleX11Events(int fd, uint32_t mask, void* data) {
        LOGVP();
        return reinterpret_cast<X11Backend*>(data)->handleX11Events();
    }
};

#endif // X11BACKEND_HPP
