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

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
 
static bool ctxErrorOccurred = false;
static int ctxErrorHandler( Display *dpy, XErrorEvent *ev )
{
    ctxErrorOccurred = true;
    return 0;
}

static void deviceOpenInfoToVisualAttributes(agpu_device_open_info* openInfo, std::vector<int> &visualInfo)
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
}

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
    // Cast the display and the window.
    Display *display = (Display*)openInfo->display;
    Window window = (Window)openInfo->window;

    // Find the framebuffer config
    std::vector<int> visualAttributes;
    deviceOpenInfoToVisualAttributes(openInfo, visualAttributes);
    GLXFBConfig framebufferConfig;
    if(!findFramebufferConfig(display, &visualAttributes[0], framebufferConfig))
        return false;
    
    // Get the default screen's GLX extension list
    const char *glxExts = glXQueryExtensionsString( display,
                                                  DefaultScreen( display ) );

    // NOTE: It is not necessary to create or make current to a context before
    // calling glXGetProcAddressARB
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
           glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );

    GLXContext context = 0;

    // Install an X error handler so the application won't exit if GL 3.0
    // context allocation fails.
    //
    // Note this error handler is global.  All display connections in all threads
    // of a process use the same error handler, so be sure to guard against other
    // threads issuing X commands while this code is running.
    ctxErrorOccurred = false;
    int (*oldHandler)(Display*, XErrorEvent*) =
      XSetErrorHandler(&ctxErrorHandler);

    // Check for the GLX_ARB_create_context extension string and the function.
    // If either is not present, use GLX 1.3 context creation method.
    if ( !agpu_device::isExtensionSupported( glxExts, "GLX_ARB_create_context" ) ||
       !glXCreateContextAttribsARB )
    {
        fprintf(stderr, "glXCreateContextAttribsARB() not found... using old-style GLX context\n" );
        context = glXCreateNewContext( display, framebufferConfig, GLX_RGBA_TYPE, 0, True );
    }
    else
    {
        // Try to get a GL 3.0 context!
        int contextAttributes[] =
          {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 0,
            //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            None
          };

        context = glXCreateContextAttribsARB( display, framebufferConfig, 0, True, contextAttributes );
        // Sync to ensure any errors generated are processed.
        XSync( display, False );
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

            ctxErrorOccurred = false;
            context = glXCreateContextAttribsARB( display, framebufferConfig, 0, True, contextAttributes );
        }
    }


    // Sync to ensure any errors generated are processed.
    XSync( display, False );

    // Restore the original error handler
    XSetErrorHandler( oldHandler );

    if ( ctxErrorOccurred || !context )
    {
        fprintf( stderr, "Failed to create an OpenGL context\n" );
        return nullptr;
    }

    // Created the context.
    agpu_device *device = new agpu_device();
    device->display = display;
    device->window = window;
    device->context = context;
    if(!device->makeCurrent())
    {
        fprintf( stderr, "Failed to make current recently created OpenGL context\n" );
        return nullptr;
    }

    device->readVersionInformation();
    device->loadExtensions();
    return device;
}

void *agpu_device::getProcAddress(const char *symbolName)
{
    return (void*)glXGetProcAddress((const GLubyte*)symbolName);
}

agpu_error agpu_device::swapBuffers()
{
    if(!makeCurrent()) return AGPU_NOT_CURRENT_CONTEXT;
    glFlush();
    glXSwapBuffers(display, window);
    return AGPU_OK;
}

bool agpu_device::makeCurrent()
{
    return glXMakeCurrent(display, window, context) == True;
}

#endif

