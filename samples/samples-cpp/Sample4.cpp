#include "SampleBase.hpp"
#include "SampleMesh.hpp"
#include "glm/gtc/matrix_transform.hpp"

static const float MaxLod = 10000.0;

struct TransformationState
{
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 modelMatrix;
};


class Sample4: public SampleBase
{
public:
    Sample4()
    {
        draggingLeft = false;
        draggingRight = false;
        cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
    }

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

            // Create the pipeline builder
            auto pipelineBuilder = device->createPipelineBuilder();
            pipelineBuilder->setRenderTargetFormat(0, ColorBufferFormat);
            pipelineBuilder->setDepthStencilFormat(DepthStencilBufferFormat);
            pipelineBuilder->setShaderSignature(shaderSignature);
            pipelineBuilder->attachShader(vertexShader);
            pipelineBuilder->attachShader(fragmentShader);
            pipelineBuilder->setVertexLayout(getSampleVertexLayout());
            pipelineBuilder->setPrimitiveType(AGPU_TRIANGLES);
            pipelineBuilder->setCullMode(AGPU_CULL_MODE_BACK);

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

        // Create the cube mesh
        {
            SampleMeshBuilder builder(this);
            builder.addCubeWithExtent(glm::vec3(1.0f, 1.0f, 1.0f));

            cubeMesh = builder.mesh();
        }


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
            samplerDesc.filter = AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST;
            samplerDesc.address_u = AGPU_TEXTURE_ADDRESS_MODE_WRAP;
            samplerDesc.address_v = AGPU_TEXTURE_ADDRESS_MODE_WRAP;
            samplerDesc.address_w = AGPU_TEXTURE_ADDRESS_MODE_WRAP;
            samplerDesc.max_lod = MaxLod;
            sampler = device->createSampler(&samplerDesc);
            samplerBindings->bindSampler(0, sampler);
        }

        commandAllocator = device->createCommandAllocator(AGPU_COMMAND_LIST_TYPE_DIRECT, commandQueue);
        commandList = device->createCommandList(AGPU_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr);
        commandList->close();

        return true;
    }

    virtual void onMouseButtonDown(const SDL_MouseButtonEvent &event) override
    {
        if(event.button == SDL_BUTTON_LEFT)
            draggingLeft = true;
        else if(event.button == SDL_BUTTON_RIGHT)
            draggingRight = true;

    }

    virtual void onMouseButtonUp(const SDL_MouseButtonEvent &event) override
    {
        if(event.button == SDL_BUTTON_LEFT)
            draggingLeft = false;
        else if(event.button == SDL_BUTTON_RIGHT)
            draggingRight = false;
    }

    virtual void onMouseMotion(const SDL_MouseMotionEvent &event) override
    {
        if(draggingLeft)
        {
            cameraAngle += glm::vec3(-event.yrel, -event.xrel, 0.0f)*0.01f;
        }
        else if(draggingRight)
        {
            cameraPosition += glm::vec3(cameraOrientation() * glm::vec4(event.xrel, -event.yrel, 0.0f, 0.0f)*0.01f);
        }
    }

    virtual void onMouseWheel(const SDL_MouseWheelEvent &event) override
    {
        cameraPosition += glm::vec3(cameraOrientation() * glm::vec4(0.0f, 0.0f, event.y*-0.1f, 0.0f));
    }

    glm::mat4 cameraTranslation()
    {
        return glm::translate(glm::mat4(1.0f), cameraPosition);
    }

    glm::mat4 cameraOrientation()
    {
        glm::mat4 matrix(1.0f);
        matrix = glm::rotate(matrix, cameraAngle.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, cameraAngle.x, glm::vec3(1.0f, 0.0f, 0.0f));
        return matrix;
    }

    glm::mat4 cameraModelMatrix()
    {
        return cameraTranslation()*cameraOrientation();
    }

    virtual void render() override
    {
        // Compute the projection matrix
        float aspect = float(screenWidth) / float(screenHeight);
        transformationState.projectionMatrix = perspective(60.0f, aspect, 0.01f, 100.0f);

        transformationState.viewMatrix = glm::inverse(cameraModelMatrix());

        // Upload the transformation state.
        transformationBuffer->uploadBufferData(0, sizeof(transformationState), &transformationState);

        // Build the command list
        commandAllocator->reset();
        commandList->reset(commandAllocator, nullptr);

        auto backBuffer = swapChain->getCurrentBackBuffer();

        commandList->setShaderSignature(shaderSignature);
        commandList->beginRenderPass(mainRenderPass, backBuffer, false);

        commandList->setViewport(0, 0, screenWidth, screenHeight);
        commandList->setScissor(0, 0, screenWidth, screenHeight);

        // Set the shader resource bindings
        commandList->usePipelineState(pipeline);
        commandList->useShaderResources(shaderBindings);
        commandList->useShaderResources(textureBindings);
        commandList->useShaderResources(samplerBindings);

        // Draw the mesh
        cubeMesh->drawWithCommandList(commandList);

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

    agpu_pipeline_state_ref pipeline;
    agpu_command_allocator_ref commandAllocator;
    agpu_command_list_ref commandList;

    agpu_texture_ref diffuseTexture;
    agpu_sampler_ref sampler;
    agpu_renderpass_ref mainRenderPass;

    SampleMeshPtr cubeMesh;
    TransformationState transformationState;

    bool draggingLeft;
    bool draggingRight;
    glm::vec3 cameraAngle;
    glm::vec3 cameraPosition;
};

SAMPLE_MAIN(Sample4)
