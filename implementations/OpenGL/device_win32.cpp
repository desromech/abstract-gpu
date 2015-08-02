#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "device.hpp"

#if defined(_WIN32)
#include <GL/wglext.h>

static void deviceOpenInfoToVisualAttributes(agpu_device_open_info* openInfo, std::vector<int> &visualInfo)
{
    visualInfo.clear();
    visualInfo.push_back(WGL_DRAW_TO_WINDOW_ARB); visualInfo.push_back(GL_TRUE);
    visualInfo.push_back(WGL_SUPPORT_OPENGL_ARB); visualInfo.push_back(GL_TRUE);
    visualInfo.push_back(WGL_PIXEL_TYPE_ARB); visualInfo.push_back(WGL_TYPE_RGBA_ARB);

    if (openInfo->red_size)
    {
        visualInfo.push_back(WGL_RED_BITS_ARB);
        visualInfo.push_back(openInfo->red_size);
    }

    if (openInfo->green_size)
    {
        visualInfo.push_back(WGL_GREEN_BITS_ARB);
        visualInfo.push_back(openInfo->green_size);
    }

    if (openInfo->blue_size)
    {
        visualInfo.push_back(WGL_BLUE_BITS_ARB);
        visualInfo.push_back(openInfo->blue_size);
    }

    if (openInfo->alpha_size)
    {
        visualInfo.push_back(WGL_ALPHA_BITS_ARB);
        visualInfo.push_back(openInfo->alpha_size);
    }

    if (openInfo->depth_size)
    {
        visualInfo.push_back(WGL_DEPTH_BITS_ARB);
        visualInfo.push_back(openInfo->depth_size);
    }

    if (openInfo->stencil_size)
    {
        visualInfo.push_back(WGL_STENCIL_BITS_ARB);
        visualInfo.push_back(openInfo->stencil_size);
    }

    if (openInfo->doublebuffer)
    {
        visualInfo.push_back(WGL_DOUBLE_BUFFER_ARB);
        visualInfo.push_back(openInfo->doublebuffer);
    }

    if (openInfo->sample_buffers)
    {
        visualInfo.push_back(WGL_SAMPLE_BUFFERS_ARB);
        visualInfo.push_back(openInfo->sample_buffers);
        visualInfo.push_back(WGL_SAMPLES_ARB);
        visualInfo.push_back(openInfo->samples);

    }

    visualInfo.push_back(0);
}

static LRESULT CALLBACK dummyWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hWnd, message, wParam, lParam);;
}

agpu_device *_agpu_device::open(agpu_device_open_info* openInfo)
{
    // Cast the open info
    HWND window = (HWND)openInfo->window;
    HDC windowDC = (HDC)openInfo->surface;

    // Window for context creation.
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    WNDCLASSW wc;
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = dummyWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"agpuDummyOGLWindow";
    wc.style = CS_OWNDC;
    if (!RegisterClassW(&wc))
        return nullptr;

    HWND dummyWindow = CreateWindowW(wc.lpszClassName, L"agpuDummyOGLWindow", WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, 0, 0, hInstance, 0);

    // Need to create a dummy context first.
    PIXELFORMATDESCRIPTOR dummyPfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
        PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
        32,                        //Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                        //Number of bits for the depthbuffer
        8,                        //Number of bits for the stencilbuffer
        0,                        //Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    // Set the pixel format.
    auto dummyDC = GetDC(dummyWindow);
    int dummyPixelFormat = ChoosePixelFormat(dummyDC, &dummyPfd);
    SetPixelFormat(dummyDC, dummyPixelFormat, &dummyPfd);

    // Create the context.
    auto dummyContext = wglCreateContext(dummyDC);
    if (!dummyContext)
    {
        DestroyWindow(dummyWindow);
        return nullptr;
    }

    // Use the context.
    if (!wglMakeCurrent(dummyDC, dummyContext))
    {
        wglDeleteContext(dummyContext);
        DestroyWindow(dummyWindow);
        return nullptr;

    }

    // Choose a proper pixel format
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
    if (wglChoosePixelFormatARB)
    {
        int pixelFormat;
        UINT numFormats;
        std::vector<int> attributes;
        deviceOpenInfoToVisualAttributes(openInfo, attributes);

        wglChoosePixelFormatARB(windowDC, &attributes[0], nullptr, 1, &pixelFormat, &numFormats);
        SetPixelFormat(windowDC, dummyPixelFormat, &dummyPfd);
    }
    else
    {
        // Use the old pfd
        dummyPfd.cRedBits = openInfo->red_size;
        dummyPfd.cGreenBits = openInfo->green_size;
        dummyPfd.cBlueBits = openInfo->blue_size;
        dummyPfd.cAlphaBits = openInfo->alpha_size;
        dummyPfd.cStencilBits = openInfo->stencil_size;
        dummyPfd.cDepthBits = openInfo->depth_size;
        
        int pixelFormat = ChoosePixelFormat(windowDC, &dummyPfd);
        SetPixelFormat(windowDC, pixelFormat, &dummyPfd);
    }

    // Now create the actual context.
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    HGLRC context;
    if (wglCreateContextAttribsARB)
    {
        int contextAttributes[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 0,
            WGL_CONTEXT_MINOR_VERSION_ARB, 0,
            //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            0
        };

        for (int versionIndex = 0; GLContextVersionPriorities[versionIndex] != OpenGLVersion::Invalid; ++versionIndex)
        {
            auto version = (int)GLContextVersionPriorities[versionIndex];

            // GLX_CONTEXT_MAJOR_VERSION_ARB
            contextAttributes[1] = version / 10;
            // GLX_CONTEXT_MINOR_VERSION_ARB
            contextAttributes[3] = version % 10;

            context = wglCreateContextAttribsARB(windowDC, NULL, contextAttributes);//glXCreateContextAttribsARB(display, framebufferConfig, 0, True, contextAttributes);

            // Check for success.
            if (context)
                break;
        }
    }
    else
    {
        context = wglCreateContext(windowDC);
    }

    // Destroy the dummy context
    wglMakeCurrent(windowDC, NULL);
    wglDeleteContext(dummyContext);
    DestroyWindow(dummyWindow);

    // Set the current context.
    if (!context || !wglMakeCurrent(windowDC, context))
        return nullptr;

    // Create the device and load the extensions.
    auto device = new agpu_device();
    device->window = window;
    device->hDC = windowDC;
    device->context = context;
    device->readVersionInformation();
    device->loadExtensions();

    return device;
}

void *agpu_device::getProcAddress(const char *symbolName)
{
    return (void*) wglGetProcAddress(symbolName);
}

agpu_error agpu_device::swapBuffers()
{
    return SwapBuffers(hDC) ? AGPU_OK : AGPU_ERROR;
}

bool agpu_device::makeCurrent()
{
    return wglMakeCurrent(hDC, context) == TRUE;
}

#endif

