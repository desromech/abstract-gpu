#include "SampleBase.hpp"

class Sample1: public SampleBase
{
public:
    bool initializeSample()
    {
        commandList = agpuCreateCommandList(device, nullptr);
        return true;
    }

    void render()
    {
        // Build the command list
        agpuResetCommandList(commandList);

        agpuSetClearColor(commandList, 0, 0, 1, 0);
        agpuClear(commandList, AGPU_COLOR_BUFFER_BIT);

        // Queue the command list
        auto queue = agpuGetDefaultCommandQueue(device);
        agpuAddCommandList(queue, commandList);

        swapBuffers();
    }

    agpu_command_list *commandList;
};

SAMPLE_MAIN(Sample1)
