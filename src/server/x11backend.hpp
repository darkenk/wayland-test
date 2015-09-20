#ifndef X11BACKEND_HPP
#define X11BACKEND_HPP

#include <X11/Xlib.h>
#include "../utils/exceptions.hpp"
#include "../utils/logger.hpp"

class X11Backend
{
public:
    X11Backend(uint32_t width, uint32_t height) {
        LOGVP("W: %d H: %d", width, height);
        mWidth = width;
        mHeight = height;
        mDisplay = XOpenDisplay(NULL);
        if (not mDisplay) {
            Exception(__FUNCTION__);
        }
        mRoot = DefaultRootWindow(mDisplay);
        XSetWindowAttributes swa;
        swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
        mWindow = XCreateWindow(mDisplay, mRoot,
                                0, 0, width, height,
                                0, CopyFromParent, InputOutput,
                                CopyFromParent, CWEventMask, &swa);
        XMapWindow(mDisplay, mWindow);
        XFlush(mDisplay);
        mPixmap = XCreatePixmap(mDisplay, mWindow, mWidth, mHeight, sDepth);
        XGCValues gcvalues;
        mGraphicContext = XCreateGC(mDisplay, mPixmap, 0, &gcvalues);
    }

    Display* getDisplay() { return mDisplay; }
    Window getWindow() { return mWindow; }

    void drawBuffer(uint32_t* buffer) {
        XImage *img = XCreateImage(mDisplay, nullptr, sDepth, ZPixmap, 0, (char*)buffer,
                                   mWidth, mHeight, 32, 0);
        if (not img) {
            LOGVP("Creation failed");
            return;
        }
        XPutImage(mDisplay, mPixmap, mGraphicContext, img, 0, 0, 0, 0, mWidth, mHeight);
        XSetWindowBackgroundPixmap(mDisplay, mWindow, mPixmap);
        XClearWindow(mDisplay, mWindow);
        XFlush(mDisplay);
    }

private:
    static constexpr int sDepth = 32;
    Pixmap mPixmap;
    Display* mDisplay;
    Window mRoot;
    Window mWindow;
    GC mGraphicContext;
    uint32_t mWidth;
    uint32_t mHeight;

};

#endif // X11BACKEND_HPP
