#include "SampleBase.hpp"

class Sample1: public SampleBase
{
public:
    bool initializeSample()
    {
        mainRenderPass = createMainPass(glm::vec4(0, 0, 1, 0));
        if(!mainRenderPass)
            return false;

        {
            agpu_shader_signature_builder_ref shaderSignatureBuilder = device->createShaderSignatureBuilder();
            shaderSignature = shaderSignatureBuilder->build();
            if (!shaderSignature)
                return false;
        }

        commandAllocator = device->createCommandAllocator(AGPU_COMMAND_LIST_TYPE_DIRECT, commandQueue.get());
        commandList = device->createCommandList(AGPU_COMMAND_LIST_TYPE_DIRECT, commandAllocator.get(), nullptr);
        commandList->close();

        return true;
    }

    void render()
    {
        // Build the command list
        commandAllocator->reset();
        commandList->reset(commandAllocator.get(), nullptr);

        agpu_framebuffer_ref backBuffer = swapChain->getCurrentBackBuffer();

        commandList->beginRenderPass(mainRenderPass.get(), backBuffer.get(), false);

        commandList->setViewport(0, 0, screenWidth, screenHeight);
        commandList->setScissor(0, 0, screenWidth, screenHeight);

        // Finish the command list
        commandList->endRenderPass();
        commandList->close();

        // Queue the command list
        commandQueue->addCommandList(commandList.get());

        swapBuffers();
        commandQueue->finishExecution();
    }

    void shutdownSample()
    {
    }

    agpu_renderpass_ref mainRenderPass;
    agpu_shader_signature_ref shaderSignature;
    agpu_command_allocator_ref commandAllocator;
    agpu_command_list_ref commandList;
};

SAMPLE_MAIN(Sample1)
