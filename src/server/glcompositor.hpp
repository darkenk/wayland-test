#ifndef GLCOMPOSITOR_HPP
#define GLCOMPOSITOR_HPP

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <cstdio>
#include "../utils/exceptions.hpp"

class GLCompositor
{
public:
    GLCompositor(EGLNativeDisplayType dpy, EGLNativeWindowType window) {
        EGLint major, minor, count, n;
        EGLConfig *configs;
        EGLint config_attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
        };

        static const EGLint context_attribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };

        mDisplay = eglGetDisplay(dpy);
        if (mDisplay == EGL_NO_DISPLAY) {
            throw Exception("Can't create egl display");
        } else {
            fprintf(stderr, "Created egl display\n");
        }

        if (eglInitialize(mDisplay, &major, &minor) != EGL_TRUE) {
            throw Exception("Can't initialise egl display");
        }
        printf("EGL major: %d, minor %d\n", major, minor);

        eglGetConfigs(mDisplay, nullptr, 0, &count);
        printf("EGL has %d configs\n", count);

        configs = reinterpret_cast<EGLConfig*>(calloc(static_cast<size_t>(count), sizeof *configs));
        eglChooseConfig(mDisplay, config_attribs, configs, count, &n);

        mConfig = configs[0];
        free(configs);
        mContext = eglCreateContext(mDisplay, mConfig, EGL_NO_CONTEXT, context_attribs);

        mSurface = eglCreateWindowSurface(mDisplay, mConfig, window, nullptr);
        eglMakeCurrent(mDisplay, mSurface, mSurface, mContext);
    }

private:
    EGLDisplay mDisplay;
    EGLConfig mConfig;
    EGLSurface mSurface;
    EGLContext mContext;
};

#endif // GLCOMPOSITOR_HPP
