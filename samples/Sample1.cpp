#include "SampleBase.hpp"

class Sample1: public SampleBase
{
public:
    bool initializeSample()
    {
        return true;
    }

    void render()
    {
        auto context = agpuGetImmediateContext(device);
        if(agpuMakeCurrent(context) != AGPU_OK)
        {
            fprintf(stderr, "Failed to use agpu immediate context");
            return;
        }

        agpuSetClearColor(context, 0, 0, 1, 0);
        agpuClear(context, AGPU_COLOR_BUFFER_BIT);

        swapBuffers();
    }
};

SAMPLE_MAIN(Sample1)
