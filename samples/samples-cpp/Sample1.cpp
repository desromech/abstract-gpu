#include "SampleBase.hpp"

class Sample1: public SampleBase
{
public:
    virtual bool initializeSample() override
    {
        mainRenderPass = createMainPass(glm::vec4(0, 0, 1, 0));
        if(!mainRenderPass)
            return false;

        {
            auto shaderSignatureBuilder = device->createShaderSignatureBuilder();
            shaderSignature = shaderSignatureBuilder->build();
            if (!shaderSignature)
                return false;
        }

        commandAllocator = device->createCommandAllocator(AGPU_COMMAND_LIST_TYPE_DIRECT, commandQueue);
        commandList = device->createCommandList(AGPU_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr);
        commandList->close();

        return true;
    }

    virtual void render() override
    {
        // Build the command list
        commandAllocator->reset();
        commandList->reset(commandAllocator, nullptr);

        auto backBuffer = swapChain->getCurrentBackBuffer();

        commandList->beginRenderPass(mainRenderPass, backBuffer, false);

        commandList->setViewport(0, 0, screenWidth, screenHeight);
        commandList->setScissor(0, 0, screenWidth, screenHeight);

        // Finish the command list
        commandList->endRenderPass();
        commandList->close();

        // Queue the command list
        commandQueue->addCommandList(commandList);

        swapBuffers();
        commandQueue->finishExecution();
    }

    virtual void shutdownSample() override
    {
    }

    agpu_renderpass_ref mainRenderPass;
    agpu_shader_signature_ref shaderSignature;
    agpu_command_allocator_ref commandAllocator;
    agpu_command_list_ref commandList;
};

SAMPLE_MAIN(Sample1)
