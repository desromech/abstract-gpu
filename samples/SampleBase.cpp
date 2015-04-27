#include <stdio.h>
#include "SampleBase.hpp"
#include "SDL_syswm.h"

int SampleBase::main(int argc, const char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = SDL_CreateWindow("AGPU Sample", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    if(!window)
    {
        fprintf(stderr, "Failed to open window\n");
        return -1;
    }

    // Get the platform.
    agpu_platform *platform;
    agpuGetPlatforms(1, &platform, nullptr);
    if(!platform)
    {
        fprintf(stderr, "Failed to get AGPU platform\n");
        return -1;
    }

    // Get the window info.
    SDL_SysWMinfo windowInfo;
    SDL_VERSION(&windowInfo.version);
    SDL_GetWindowWMInfo(window, &windowInfo);

    // Open the device
    agpu_device_open_info openInfo;
    memset(&openInfo, 0, sizeof(openInfo));
    switch(windowInfo.subsystem)
    {
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
    case SDL_SYSWM_WINDOW:
        openInfo.window = (agpu_pointer)windowInfo.info.win.window;
        break;
#endif
#if defined(SDL_VIDEO_DRIVER_X11)
    case SDL_SYSWM_X11:
        openInfo.display = (agpu_pointer)windowInfo.info.x11.display;
        openInfo.window = (agpu_pointer)(uintptr_t)windowInfo.info.x11.window;
        break;
#endif
    default:
        fprintf(stderr, "Unsupported window system\n");
        return -1;
    }

    openInfo.red_size = 5;
    openInfo.blue_size = 5;
    openInfo.green_size = 5;
    openInfo.alpha_size = 5;
    openInfo.doublebuffer = 1;
    device = agpuOpenDevice(platform, &openInfo);
    if(!device)
    {
        fprintf(stderr, "failed to open the device");
        return false;
    }

    if(!initializeSample())
        return -1;

    quit = false;
    while(!quit)
    {
        processEvents();
        render();
    }

    shutdownSample();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void SampleBase::processEvents()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_KEYDOWN:
            onKeyDown(event);
            break;
        case SDL_KEYUP:
            onKeyUp(event);
            break;
        case SDL_QUIT:
            quit = true;
            break;
        default:
            //ignore
            break;
        }
    }
}

void SampleBase::onKeyDown(const SDL_Event &event)
{
    switch(event.key.keysym.sym)
    {
    case SDLK_ESCAPE:
        quit = true;
        break;
    default:
        // ignore
        break;
    }
}

void SampleBase::onKeyUp(const SDL_Event &event)
{
}

bool SampleBase::initializeSample()
{
    return true;
}

void SampleBase::shutdownSample()
{
}

void SampleBase::render()
{
}

void SampleBase::swapBuffers()
{
}
