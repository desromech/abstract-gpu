#ifndef _SAMPLE_BASE_HPP_
#define _SAMPLE_BASE_HPP_

#include <AGPU/agpu.h>
#include <SDL.h>
#include <SDL_main.h>

class SampleBase
{
public:
    int main(int argc, const char **argv);

    virtual bool initializeSample();
    virtual void shutdownSample();
    virtual void render();
    virtual void processEvents();

    virtual void onKeyDown(const SDL_Event &event);
    virtual void onKeyUp(const SDL_Event &event);

    void swapBuffers();

protected:

    SDL_Window *window;

    agpu_device *device;
    agpu_context *immediateContext;
    bool quit;
};

#define SAMPLE_MAIN(SampleClass) \
int main(int argc, const char **argv) \
{ \
    SampleClass sample; \
    return sample.main(argc, argv); \
}

#endif //_SAMPLE_BASE_HPP_
