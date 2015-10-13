#include "SampleBase.hpp"

class Sample1: public SampleBase
{
public:
    bool initializeSample()
    {
        commandAllocator = agpuCreateCommandAllocator(device);
        commandList = agpuCreateCommandList(device, commandAllocator, nullptr);
        agpuCloseCommandList(commandList);

        return true;
    }

    void render()
    {
        // Build the command list
        agpuResetCommandAllocator(commandAllocator);
        agpuResetCommandList(commandList, commandAllocator, nullptr);
        auto backBuffer = agpuGetCurrentBackBuffer(swapChain);
        agpuBeginFrame(commandList, backBuffer);

        agpuSetViewport(commandList, 0, 0, screenWidth, screenHeight);
        agpuSetScissor(commandList, 0, 0, screenWidth, screenHeight);
        agpuSetClearColor(commandList, 0, 0, 1, 0);
        agpuClear(commandList, AGPU_COLOR_BUFFER_BIT);

        // Finish the command list
        agpuEndFrame(commandList);
        agpuCloseCommandList(commandList);

        // Queue the command list
        agpuAddCommandList(commandQueue, commandList);

        swapBuffers();
        agpuFinishQueueExecution(commandQueue);
        agpuReleaseFramebuffer(backBuffer);
    }

    void shutdownSample()
    {
        agpuReleaseCommandList(commandList);
        agpuReleaseCommandAllocator(commandAllocator);
    }

    agpu_command_allocator *commandAllocator;
    agpu_command_list *commandList;
};

SAMPLE_MAIN(Sample1)
