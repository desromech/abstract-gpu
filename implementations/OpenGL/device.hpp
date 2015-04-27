#ifndef _AGPU_DEVICE_HPP_
#define _AGPU_DEVICE_HPP_

#if defined(_WIN32)
#include <windows.h>
#include <GL/GL.h>
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#else
#error unsupported platform
#endif

#include "object.hpp"

/**
 * Agpu OpenGL device
 */
class _agpu_device: public Object<_agpu_device>
{
public:
    void lostReferences();

    static bool isExtensionSupported(const char *extList, const char *extension);
    static agpu_device *open(agpu_device_open_info* openInfo);


#ifdef _WIN32
#elif defined(__linux__)
    Display *display;
    Window window;
    GLXContext context;
#endif
};

#endif //_AGPU_DEVICE_HPP_

