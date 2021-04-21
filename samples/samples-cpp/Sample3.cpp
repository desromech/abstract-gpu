#include "SampleBase.hpp"
#include "SampleVertex.hpp"
#include "glm/gtc/matrix_transform.hpp"

static const float MaxLod = 10000.0;

static SampleVertex vertices[] = {
    SampleVertex::onlyColorTc(
        -1.0, -1.0, 0.0,
        1.0, 1.0, 1.0, 1.0,
        0.0, 1.0),
    SampleVertex::onlyColorTc(1.0, -1.0, 0.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 1.0),
    SampleVertex::onlyColorTc(1.0, 1.0, 0.0,
        1.0, 1.0, 1.0, 1.0,
        1.0, 0.0),
    SampleVertex::onlyColorTc(-1.0, 1.0, 0.0, 1.0,
        1.0, 1.0, 1.0,
        0.0, 0.0),
};

static uint32_t indices[] = {
    0, 1, 2,
    2, 3, 0,
};

struct TransformationState
{
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 modelMatrix;
};


class Sample3: public SampleBase
{
public:
    virtual bool initializeSample() override
    {
        mainRenderPass = createMainPass();
        if(!mainRenderPass)
            return false;

        // Create the shader signature.
        {
            auto shaderSignatureBuilder = device->createShaderSignatureBuilder();
            shaderSignatureBuilder->beginBindingBank(1);
            shaderSignatureBuilder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER, 1);

            shaderSignatureBuilder->beginBindingBank(1);
            shaderSignatureBuilder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE, 1);

            shaderSignatureBuilder->beginBindingBank(1);
            shaderSignatureBuilder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_SAMPLER, 1);
            shaderSignature = shaderSignatureBuilder->build();
            if (!shaderSignature)
                return false;
        }

        {
            // Create the programs.
            auto vertexShader = compileShaderFromFile("data/shaders/texturedVertex.glsl", AGPU_VERTEX_SHADER);
            auto fragmentShader = compileShaderFromFile("data/shaders/texturedFragment.glsl", AGPU_FRAGMENT_SHADER);
            if (!vertexShader || !fragmentShader)
                return false;

            // Create the vertex layout.
            vertexLayout = device->createVertexLayout();
            agpu_size vertexStride = sizeof(SampleVertex);
            vertexLayout->addVertexAttributeBindings(1, &vertexStride, SampleVertex::DescriptionSize, SampleVertex::Description);

            // Create the pipeline builder
            auto pipelineBuilder = device->createPipelineBuilder();
            pipelineBuilder->setRenderTargetFormat(0, ColorBufferFormat);
            pipelineBuilder->setDepthStencilFormat(DepthStencilBufferFormat);
            pipelineBuilder->setShaderSignature(shaderSignature);
            pipelineBuilder->attachShader(vertexShader);
            pipelineBuilder->attachShader(fragmentShader);
            pipelineBuilder->setVertexLayout(vertexLayout);
            pipelineBuilder->setPrimitiveType(AGPU_TRIANGLES);

            // Build the pipeline
            pipeline = pipelineBuilder->build();
            if (!pipeline)
                return false;
        }

        // Load the texture
        {
            diffuseTexture = loadTexture("data/textures/checkboard.bmp");
            if(!diffuseTexture)
                return false;
        }

        // Create the vertex and the index buffer
        vertexBuffer = createImmutableVertexBuffer(sizeof(vertices)/sizeof(vertices[0]), sizeof(vertices[0]), vertices);
        indexBuffer = createImmutableIndexBuffer(sizeof(indices)/sizeof(indices[0]), sizeof(indices[0]), indices);

        // Create the transformation buffer.
        transformationBuffer = createUploadableUniformBuffer(sizeof(TransformationState), nullptr);

        // Create the shader bindings.
        {
            shaderBindings = shaderSignature->createShaderResourceBinding(0);
            shaderBindings->bindUniformBuffer(0, transformationBuffer);

            textureBindings = shaderSignature->createShaderResourceBinding(1);
            textureBindings->bindSampledTextureView(0, diffuseTexture->getOrCreateFullView());

            samplerBindings = shaderSignature->createShaderResourceBinding(2);

            agpu_sampler_description samplerDesc;
            memset(&samplerDesc, 0, sizeof(samplerDesc));
            samplerDesc.filter = AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR;
            samplerDesc.address_u = AGPU_TEXTURE_ADDRESS_MODE_WRAP;
            samplerDesc.address_v = AGPU_TEXTURE_ADDRESS_MODE_WRAP;
            samplerDesc.address_w = AGPU_TEXTURE_ADDRESS_MODE_WRAP;
            samplerDesc.max_lod = MaxLod;
            sampler = device->createSampler(&samplerDesc);
            samplerBindings->bindSampler(0, sampler);
        }

        // Create the vertex buffer binding.
        vertexBinding = device->createVertexBinding(vertexLayout);
        vertexBinding->bindVertexBuffers(1, &vertexBuffer);

        commandAllocator = device->createCommandAllocator(AGPU_COMMAND_LIST_TYPE_DIRECT, commandQueue);
        commandList = device->createCommandList(AGPU_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr);
        commandList->close();

        return true;
    }

    virtual void render() override
    {
        // Compute the projection matrix
        float aspect = float(screenWidth) / float(screenHeight);
        float h = 2.0;
        float w = h*aspect;
        transformationState.projectionMatrix = ortho(-w, w, -h, h, -10.0f, 10.0f);

        // Upload the transformation state.
        transformationBuffer->uploadBufferData(0, sizeof(transformationState), &transformationState);

        // Build the command list
        commandAllocator->reset();
        commandList->reset(commandAllocator, nullptr);

        agpu_framebuffer_ref backBuffer = swapChain->getCurrentBackBuffer();

        commandList->setShaderSignature(shaderSignature);
        commandList->beginRenderPass(mainRenderPass, backBuffer, false);

        commandList->setViewport(0, 0, screenWidth, screenHeight);
        commandList->setScissor(0, 0, screenWidth, screenHeight);

        // Use the vertices and the indices.
        commandList->usePipelineState(pipeline);
        commandList->useVertexBinding(vertexBinding);
        commandList->useIndexBuffer(indexBuffer);
        commandList->useShaderResources(shaderBindings);
        commandList->useShaderResources(textureBindings);
        commandList->useShaderResources(samplerBindings);

        // Draw the objects
        commandList->drawElements(sizeof(indices) / sizeof(indices[0]), 1, 0, 0, 0);

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

    agpu_buffer_ref transformationBuffer;
    agpu_shader_signature_ref shaderSignature;
    agpu_shader_resource_binding_ref shaderBindings;
    agpu_shader_resource_binding_ref textureBindings;
    agpu_shader_resource_binding_ref samplerBindings;

    agpu_buffer_ref vertexBuffer;
    agpu_buffer_ref indexBuffer;
    agpu_vertex_layout_ref vertexLayout;
    agpu_vertex_binding_ref vertexBinding;
    agpu_pipeline_state_ref pipeline;
    agpu_command_allocator_ref commandAllocator;
    agpu_command_list_ref commandList;

    agpu_texture_ref diffuseTexture;
    agpu_sampler_ref sampler;
    agpu_renderpass_ref mainRenderPass;

    TransformationState transformationState;
};

SAMPLE_MAIN(Sample3)
