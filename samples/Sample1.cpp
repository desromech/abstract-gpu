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
        agpuSetClearColor(context, 0, 0, 1, 0);
        agpuClear(context, AGPU_COLOR_BUFFER_BIT);

        swapBuffers();
    }
};

SAMPLE_MAIN(Sample1)
