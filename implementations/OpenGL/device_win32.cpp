#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "device.hpp"

#if defined(_WIN32)
#include <GL/wglext.h>

namespace AgpuGL
{

static thread_local OpenGLContext *currentGLContext = nullptr;

OpenGLContext::OpenGLContext()
    : device(nullptr), ownsWindow(false), window(0), hDC(0), context(0)
{
}

OpenGLContext::~OpenGLContext()
{
}

OpenGLContext *OpenGLContext::getCurrent()
{
    return currentGLContext;
}

static bool printLastError()
{
    LPSTR *tempBuffer = nullptr;
    auto error = GetLastError();
    auto messageSize = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_ARGUMENT_ARRAY, nullptr, error, LANG_NEUTRAL, (LPSTR)&tempBuffer, 0, nullptr);
    if (messageSize && tempBuffer) {
        tempBuffer[strlen((const char*)tempBuffer) - 2] = 0;
        printError((const char*)tempBuffer);
        LocalFree((HLOCAL)tempBuffer);
        return true;
    }
    return false;
}

bool OpenGLContext::makeCurrentWithWindow(agpu_pointer window)
{
    auto windowDC = GetDC((HWND)window);
    auto res = wglMakeCurrent(windowDC, context) == TRUE;
    if (res)
    {
        currentGLContext = this;
    }
    else
    {
        if (!printLastError())
            printError("Failed to set context with window.\n");
    }
    return res;
}

bool OpenGLContext::makeCurrent()
{
    auto res = wglMakeCurrent(hDC, context) == TRUE;
    if (res)
    {
        currentGLContext = this;
    }
    else
    {
        printError("Failed to use context.\n");
    }
    return res;
}

void OpenGLContext::swapBuffers()
{
    glFlush();
    SwapBuffers(hDC);
}

void OpenGLContext::swapBuffersOfWindow(agpu_pointer window)
{
    glFlush();
    auto windowDC = GetDC((HWND)window);
    SwapBuffers(windowDC);
	ReleaseDC((HWND)window, windowDC);
}

void OpenGLContext::destroy()
{
    if (!context)
        return;

    device->glBindVertexArray(0);
    device->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    device->glBindBuffer(GL_ARRAY_BUFFER, 0);
    device->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    device->glBindBuffer(GL_UNIFORM_BUFFER, 0);

    wglMakeCurrent(wglGetCurrentDC(), 0);
    wglDeleteContext(context);
    context = 0;
}

/*static void deviceOpenInfoToVisualAttributes(agpu_device_open_info* openInfo, std::vector<int> &visualInfo)
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
*/

static LRESULT CALLBACK dummyWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hWnd, message, wParam, lParam);;
}

void GLDevice::setWindowPixelFormat(agpu_pointer window)
{
    auto hwnd = (HWND)window;
    auto dc = GetDC(hwnd);

    UINT numFormats;
    int pixelAttributes[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_RED_BITS_ARB, 8,
        WGL_GREEN_BITS_ARB, 8,
        WGL_BLUE_BITS_ARB, 8,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 0,
        0
    };

    PIXELFORMATDESCRIPTOR pixelFormatDescriptor =
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
        0,                        //Number of bits for the depthbuffer
        0,                        //Number of bits for the stencilbuffer
        0,                        //Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    int pixelFormat= 0;
    mainContext->wglChoosePixelFormatARB(dc, &pixelAttributes[0], nullptr, 1, &pixelFormat, &numFormats);

    auto res = SetPixelFormat(dc, pixelFormat, &pixelFormatDescriptor);
    if (res == FALSE)
    {
        if (!printLastError())
            printError("Failed to set window pixel format.\n");

    }
}

agpu_device *GLDevice::open(agpu_device_open_info* openInfo)
{
    // Create the device.
    std::unique_ptr<agpu_device> device(new agpu_device);
    bool failure = false;

    // Perform the main context creation in
    device->mainContextJobQueue.start();
    AsyncJob contextCreationJob([&] {
        std::unique_ptr<OpenGLContext> contextWrapper(new OpenGLContext());

        // Window for context creation.
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        WNDCLASSW wc;
        memset(&wc, 0, sizeof(wc));
        wc.lpfnWndProc = dummyWindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = L"agpuDummyOGLWindow";
        wc.style = CS_OWNDC;
        if (!RegisterClassW(&wc))
        {
            failure = true;
            return;
        }

        HWND dummyWindow = CreateWindowW(wc.lpszClassName, L"agpuDummyOGLWindow", WS_OVERLAPPEDWINDOW, 0, 0, 16, 16, 0, 0, hInstance, 0);

        // Need to create a dummy context first.
        PIXELFORMATDESCRIPTOR pixelFormatDescriptor =
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
        int dummyPixelFormat = ChoosePixelFormat(dummyDC, &pixelFormatDescriptor);
        auto pfRes = SetPixelFormat(dummyDC, dummyPixelFormat, &pixelFormatDescriptor);;
        if (pfRes == FALSE)
        {
            if (!printLastError())
                printError("Failed to set window pixel format.\n");

        }

        // Create the context.
        auto dummyContext = wglCreateContext(dummyDC);
        if (!dummyContext)
        {
            DestroyWindow(dummyWindow);
            failure = true;
            return;
        }

        // Use the context.
        if (!wglMakeCurrent(dummyDC, dummyContext))
        {
            wglDeleteContext(dummyContext);
            DestroyWindow(dummyWindow);
            failure = true;
            return;
        }

        // Create the context window.
        HWND contextWindow = CreateWindowW(wc.lpszClassName, L"agpuOGLWindow", WS_OVERLAPPEDWINDOW, 0, 0, 16, 16, 0, 0, hInstance, 0);
        auto contextDC = GetDC(contextWindow);

        // Choose a proper pixel format
        contextWrapper->wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
        if (contextWrapper->wglChoosePixelFormatARB)
        {
            int pixelFormat;
            UINT numFormats;
            int pixelAttributes[] = {
                WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                WGL_RED_BITS_ARB, 8,
                WGL_GREEN_BITS_ARB, 8,
                WGL_BLUE_BITS_ARB, 8,
                WGL_ALPHA_BITS_ARB, 8,
                WGL_DEPTH_BITS_ARB, 0,
                0
            };

            contextWrapper->wglChoosePixelFormatARB(contextDC, &pixelAttributes[0], nullptr, 1, &pixelFormat, &numFormats);
            auto res = SetPixelFormat(contextDC, pixelFormat, &pixelFormatDescriptor);
            if (res == FALSE)
            {
                printError("Failed to set the pixel format.\n");
                failure = true;
                return;
            }
        }
        else
        {
            printError("Missing a required extension.\n");
            failure = true;
            return;
        }

        // Now create the actual context.
        contextWrapper->wglCreateContextAttribsARB = (wglCreateContextAttribsARBProc)wglGetProcAddress("wglCreateContextAttribsARB");
        if (contextWrapper->wglCreateContextAttribsARB)
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

                contextWrapper->context = contextWrapper->wglCreateContextAttribsARB(contextDC, NULL, contextAttributes);//glXCreateContextAttribsARB(display, framebufferConfig, 0, True, contextAttributes);

                // Check for success.
                if (contextWrapper->context)
                {
                    contextWrapper->version = OpenGLVersion(version);
                    break;
                }
            }
        }
        else
        {
            contextWrapper->context = wglCreateContext(contextDC);
        }

        // Destroy the dummy context
        wglMakeCurrent(dummyDC, NULL);
        wglDeleteContext(dummyContext);
        DestroyWindow(dummyWindow);

        // Set the current context.
        if (!contextWrapper->context || !wglMakeCurrent(contextDC, contextWrapper->context))
        {
            failure = true;
            return;
        }

        // Create the device and load the extensions.
        contextWrapper->window = contextWindow;
        contextWrapper->hDC = contextDC;
        contextWrapper->makeCurrent();
        device->mainContext = contextWrapper.release();
        device->mainContext->device = device.get();
        device->initializeObjects();
    });

    device->mainContextJobQueue.addJob(&contextCreationJob);
    contextCreationJob.wait();
    if (failure)
        return nullptr;

    return device.release();
}

void *agpu_device::getProcAddress(const char *symbolName)
{
    return (void*) wglGetProcAddress(symbolName);
}

/*agpu_error agpu_device::swapBuffers()
{
    return SwapBuffers(hDC) ? AGPU_OK : AGPU_ERROR;
}

bool agpu_device::makeCurrent()
{
    return wglMakeCurrent(hDC, context) == TRUE;
}
*/

} // End of namespace AgpuGL

#endif
