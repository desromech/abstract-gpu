#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "device.hpp"

#if defined(__linux__)

//------------------------------------------------------------------------------
// GLX context creation code adapted from: https://www.opengl.org/wiki/Tutorial:_OpenGL_3.0_Context_Creation_%28GLX%29 (April 2015)

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092

static std::mutex contextErrorMutex;
static bool ctxErrorOccurred = false;
static int ctxErrorHandler( Display *dpy, XErrorEvent *ev )
{
    ctxErrorOccurred = true;
    return 0;
}

static thread_local OpenGLContext *currentGLContext = nullptr;

OpenGLContext::OpenGLContext()
    : ownsWindow(false), display(nullptr), window(0), context(0)
{
}

OpenGLContext::~OpenGLContext()
{
}

OpenGLContext *OpenGLContext::getCurrent()
{
    return currentGLContext;
}

bool OpenGLContext::makeCurrentWithWindow(agpu_pointer window)
{
    auto res = glXMakeCurrent(display, (Window)window, context) == True;
    if(res)
        currentGLContext = this;
    return res;
}

bool OpenGLContext::makeCurrent()
{
    auto res = glXMakeCurrent(display, window, context) == True;
    if(res)
        currentGLContext = this;
    return res;
}

void OpenGLContext::swapBuffers()
{
    glFlush();
    glXSwapBuffers(display, window);
}

void OpenGLContext::swapBuffersOfWindow(agpu_pointer window)
{
    glFlush();
    glXSwapBuffers(display, (Window)window);
}

void OpenGLContext::destroy()
{

}

OpenGLContext agpu_device::createSecondaryContext(bool useMainWindow)
{
    std::unique_lock<std::mutex> l(contextErrorMutex);
    OpenGLContext result;
    result.framebufferConfig = mainContext.framebufferConfig;
    result.display = mainContext.display;
    result.window = mainContext.window;

    auto version = (int)mainContext.version;
    int contextAttributes[] =
    {
        GLX_CONTEXT_MAJOR_VERSION_ARB, version / 10,
        GLX_CONTEXT_MINOR_VERSION_ARB, version % 10,
        //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        None
    };

    // Install an X error handler so the application won't exit if GL 3.0
    // context allocation fails.
    ctxErrorOccurred = false;
    auto oldHandler = XSetErrorHandler(&ctxErrorHandler);

    // Create the new context
    result.context = mainContext.glXCreateContextAttribsARB( result.display, result.framebufferConfig, mainContext.context, True, contextAttributes );

    // Sync to ensure any errors generated are processed.
    XSync( result.display, False );

    // Restore the original error handler
    XSetErrorHandler( oldHandler );

    if ( ctxErrorOccurred || !result.context )
    {
        fprintf( stderr, "Failed to create a secondary OpenGL context\n" );
        abort();
    }

    // Set the current context.
    if(useMainWindow && !result.makeCurrent())
    {
        fprintf( stderr, "Failed to make current a secondary OpenGL context\n" );
        abort();
    }

    return result;
}

/*static void deviceOpenInfoToVisualAttributes(agpu_device_open_info* openInfo, std::vector<int> &visualInfo)
{
    visualInfo.clear();
    visualInfo.push_back(GLX_X_RENDERABLE); visualInfo.push_back(True);
    visualInfo.push_back(GLX_RENDER_TYPE); visualInfo.push_back(GLX_RGBA_BIT);
    visualInfo.push_back(GLX_X_VISUAL_TYPE); visualInfo.push_back(GLX_TRUE_COLOR);

    if(openInfo->red_size)
    {
        visualInfo.push_back(GLX_RED_SIZE);
        visualInfo.push_back(openInfo->red_size);
    }

    if(openInfo->green_size)
    {
        visualInfo.push_back(GLX_GREEN_SIZE);
        visualInfo.push_back(openInfo->green_size);
    }

    if(openInfo->blue_size)
    {
        visualInfo.push_back(GLX_BLUE_SIZE);
        visualInfo.push_back(openInfo->blue_size);
    }

    if(openInfo->alpha_size)
    {
        visualInfo.push_back(GLX_ALPHA_SIZE);
        visualInfo.push_back(openInfo->alpha_size);
    }

    if(openInfo->depth_size)
    {
        visualInfo.push_back(GLX_DEPTH_SIZE);
        visualInfo.push_back(openInfo->depth_size);
    }

    if(openInfo->stencil_size)
    {
        visualInfo.push_back(GLX_STENCIL_SIZE);
        visualInfo.push_back(openInfo->stencil_size);
    }

    if(openInfo->doublebuffer)
    {
        visualInfo.push_back(GLX_DOUBLEBUFFER);
        visualInfo.push_back(openInfo->doublebuffer);
    }

    if(openInfo->sample_buffers)
    {
        visualInfo.push_back(GLX_SAMPLE_BUFFERS);
        visualInfo.push_back(openInfo->sample_buffers);
        visualInfo.push_back(GLX_SAMPLES);
        visualInfo.push_back(openInfo->samples);

    }

    visualInfo.push_back(None);
}*/

static bool findFramebufferConfig(Display *display, int *visualAttributes, GLXFBConfig &config)
{
    int glx_major, glx_minor;
    // FBConfigs were added in GLX version 1.3.
    if ( !glXQueryVersion( display, &glx_major, &glx_minor ) ||
       ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
    {
        fprintf(stderr, "Invalid GLX version");
        return false;
    }

    int fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), visualAttributes, &fbcount);
    if (!fbc)
    {
        fprintf( stderr, "Failed to retrieve a framebuffer config\n" );
        return false;
    }

    // Pick the FB config/visual with the most samples per pixel
    int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

    int i;
    for (i=0; i<fbcount; ++i)
    {
        XVisualInfo *vi = glXGetVisualFromFBConfig( display, fbc[i] );
        if ( vi )
        {
          int samp_buf, samples;
          glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
          glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLES       , &samples  );

          if ( best_fbc < 0 || (samp_buf && samples > best_num_samp) )
            best_fbc = i, best_num_samp = samples;
          if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
            worst_fbc = i, worst_num_samp = samples;
        }
        XFree( vi );
    }

    config = fbc[ best_fbc ];
    XFree( fbc );
    return true;
}

agpu_device *_agpu_device::open(agpu_device_open_info* openInfo)
{
    // Ensure X11 threads are initialized
    XInitThreads();

    // Create the device.
    std::unique_ptr<agpu_device> device(new agpu_device);
    bool failure = false;

    // Perform the main context creation in
    device->mainContextJobQueue.start();
    AsyncJob contextCreationJob([&] {
        auto &contextWrapper = device->mainContext;
        contextWrapper.display = (Display*)openInfo->display;

        // Create a simple context.
        int visualAttributes[] {
            GLX_X_RENDERABLE, True,
            GLX_RENDER_TYPE, GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,

            GLX_RED_SIZE, 8,
            GLX_GREEN_SIZE, 8,
            GLX_BLUE_SIZE, 8,
            GLX_ALPHA_SIZE, 8,

            GLX_DOUBLEBUFFER, True,

            None, None
        };

        // Find the framebuffer config.
        GLXFBConfig framebufferConfig;
        if(!findFramebufferConfig(contextWrapper.display, &visualAttributes[0], framebufferConfig))
        {
            failure = true;
            return;
        }
        contextWrapper.framebufferConfig = framebufferConfig;

        // Get a visual
        XVisualInfo *vi = glXGetVisualFromFBConfig( contextWrapper.display, framebufferConfig );
        XSetWindowAttributes swa;
        Colormap cmap;
        swa.colormap = cmap = XCreateColormap( contextWrapper.display,
                                             RootWindow( contextWrapper.display, vi->screen ),
                                             vi->visual, AllocNone );
        swa.background_pixmap = None ;
        swa.border_pixel      = 0;
        swa.event_mask        = StructureNotifyMask;

        Window win = XCreateWindow( contextWrapper.display, RootWindow( contextWrapper.display, vi->screen ),
                                  0, 0, 4, 4, 0, vi->depth, InputOutput,
                                  vi->visual,
                                  CWBorderPixel|CWColormap|CWEventMask, &swa );
        if ( !win )
        {
            printf( "Failed to create window.\n" );
            failure = true;
        }

        // Done with the visual info data
        XFree( vi );

        XStoreName( contextWrapper.display, win, "AGPU dummy window" );

        //Store the window in the context wrapper.
        contextWrapper.ownsWindow = true;
        contextWrapper.window = win;

        // Get the default screen's GLX extension list
        const char *glxExts = glXQueryExtensionsString( contextWrapper.display,
                                                      DefaultScreen( contextWrapper.display ) );

        // NOTE: It is not necessary to create or make current to a context before
        // calling glXGetProcAddressARB
        auto glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
               glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );
        contextWrapper.glXCreateContextAttribsARB = glXCreateContextAttribsARB;

        GLXContext context = 0;

        // Install an X error handler so the application won't exit if GL 3.0
        // context allocation fails.
        std::unique_lock<std::mutex> l(contextErrorMutex);
        ctxErrorOccurred = false;
        auto oldHandler = XSetErrorHandler(&ctxErrorHandler);

        // Check for the GLX_ARB_create_context extension string and the function.
        // If either is not present, use GLX 1.3 context creation method.
        if ( !agpu_device::isExtensionSupported( glxExts, "GLX_ARB_create_context" ) ||
           !glXCreateContextAttribsARB )
        {
            fprintf(stderr, "glXCreateContextAttribsARB() not found... using old-style GLX context\n" );
            context = glXCreateNewContext( contextWrapper.display, framebufferConfig, GLX_RGBA_TYPE, 0, True );
        }
        else
        {
            int contextAttributes[] =
            {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 0,
                GLX_CONTEXT_MINOR_VERSION_ARB, 0,
                //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                None
            };

            for(int versionIndex = 0; GLContextVersionPriorities[versionIndex] !=  OpenGLVersion::Invalid; ++versionIndex)
            {
                auto version = (int)GLContextVersionPriorities[versionIndex];

                // GLX_CONTEXT_MAJOR_VERSION_ARB
                contextAttributes[1] = version / 10;
                // GLX_CONTEXT_MINOR_VERSION_ARB
                contextAttributes[3] = version % 10;

                context = glXCreateContextAttribsARB( contextWrapper.display, framebufferConfig, 0, True, contextAttributes );

                // Sync to ensure any errors generated are processed.
                XSync( contextWrapper.display, False );

                // Check for success.
                if(!ctxErrorOccurred && context)
                {
                    contextWrapper.version = OpenGLVersion(version);
                    break;
                }
            }

            // Check failure.
            if ( ctxErrorOccurred || !context )
            {
                // Couldn't create GL 3.0 context.  Fall back to old-style 2.x context.
                // When a context version below 3.0 is requested, implementations will
                // return the newest context version compatible with OpenGL versions less
                // than version 3.0.
                // GLX_CONTEXT_MAJOR_VERSION_ARB = 1
                contextAttributes[1] = 1;
                // GLX_CONTEXT_MINOR_VERSION_ARB = 0
                contextAttributes[3] = 0;
                contextWrapper.version = OpenGLVersion::Version10;

                ctxErrorOccurred = false;
                context = glXCreateContextAttribsARB( contextWrapper.display, framebufferConfig, 0, True, contextAttributes );
            }
        }

        // Sync to ensure any errors generated are processed.
        XSync( contextWrapper.display, False );

        // Restore the original error handler
        XSetErrorHandler( oldHandler );

        if ( ctxErrorOccurred || !context )
        {
            fprintf( stderr, "Failed to create an OpenGL context\n" );
            failure = true;
            return;
        }

        // Store the context in the wrapper.
        contextWrapper.context = context;
        if(!contextWrapper.makeCurrent())
        {
            fprintf(stderr, "Failed to make current the main OpenGL context.\n");
            failure = true;
            return;
        }

        // Initialize the device objects.
        device->initializeObjects();
    });

    device->mainContextJobQueue.addJob(&contextCreationJob);
    contextCreationJob.wait();
    if(failure)
        return nullptr;
    return device.release();
}

void *agpu_device::getProcAddress(const char *symbolName)
{
    return (void*)glXGetProcAddress((const GLubyte*)symbolName);
}

#endif
