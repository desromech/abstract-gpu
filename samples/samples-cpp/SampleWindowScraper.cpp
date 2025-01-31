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


class Sample5: public SampleBase
{
public:
    Sample5()
    {
        draggingLeft = false;
        draggingRight = false;
        cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
    }

    virtual bool initializeSample() override
    {
        windowScraper = device->createWindowScraper();
        if(windowScraper)
        {
            auto windowCount = windowScraper->enumerateWindows();
            for(agpu_uint i = 0; i < windowCount; ++i)
            {
                std::string title = windowScraper->getWindowTitle(i);
                if(!title.empty())
                    printf("Window %u: %s\n", i, title.c_str());
            }

            agpu_uint selectedWindowIndex = 0;
            printf("Select window: ");
            fscanf(stdin, "%u", &selectedWindowIndex);
            if (selectedWindowIndex >= windowCount)
                selectedWindowIndex = 0;

            windowScraperHandle = windowScraper->createWindowHandle(selectedWindowIndex);
        }

        if(windowScraperHandle)
        {
            printf("Scraped window handle width: %u height %u\n", windowScraperHandle->getWidth(), windowScraperHandle->getHeight());
        }

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

        vertexShader = compileShaderFromFile("data/shaders/texturedVertex.glsl", AGPU_VERTEX_SHADER);
        fragmentShader = compileShaderFromFile("data/shaders/texturedFragment.glsl", AGPU_FRAGMENT_SHADER);

        // Load the texture
        {
            checkboardTexture = loadTexture("data/textures/checkboard.bmp");
            if(!checkboardTexture)
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
            textureBindings->bindSampledTextureView(0, checkboardTexture->getOrCreateFullView());

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

        stateTrackerCache = device->createStateTrackerCache(commandQueue);
        stateTracker = stateTrackerCache->createStateTracker(AGPU_COMMAND_LIST_TYPE_DIRECT, commandQueue);

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
        if(windowScraperHandle)
            scrapedTexture = windowScraperHandle->captureInTexture();

        if(scrapedTexture)
            textureBindings->bindSampledTextureView(0, scrapedTexture->getOrCreateFullView());
        else
            textureBindings->bindSampledTextureView(0, checkboardTexture->getOrCreateFullView());

        // Compute the projection matrix
        float aspect = float(screenWidth) / float(screenHeight);
        transformationState.projectionMatrix = perspective(60.0f, aspect, 0.01f, 100.0f);

        transformationState.viewMatrix = glm::inverse(cameraModelMatrix());

        // Upload the transformation state.
        transformationBuffer->uploadBufferData(0, sizeof(transformationState), &transformationState);

        // Begin rendering
        stateTracker->beginRecordingCommands();

        // Build the command list
        auto backBuffer = swapChain->getCurrentBackBuffer();

        stateTracker->beginRenderPass(mainRenderPass, backBuffer, false);
        stateTracker->setViewport(0, 0, screenWidth, screenHeight);
        stateTracker->setScissor(0, 0, screenWidth, screenHeight);

        stateTracker->setShaderSignature(shaderSignature);
        stateTracker->setVertexStage(vertexShader, "main");
        stateTracker->setFragmentStage(fragmentShader, "main");

        // Set the shader resource bindings
        stateTracker->useShaderResources(shaderBindings);
        stateTracker->useShaderResources(textureBindings);
        stateTracker->useShaderResources(samplerBindings);

        // Draw the mesh
        stateTracker->setPrimitiveType(AGPU_TRIANGLES);
        stateTracker->setVertexLayout(getSampleVertexLayout());
        stateTracker->setCullMode(AGPU_CULL_MODE_BACK);
        cubeMesh->drawWithStateTracker(stateTracker);

        // End the render pass.
        stateTracker->endRenderPass();

        // End recording and flush the commands.
        stateTracker->endRecordingAndFlushCommands();

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
    agpu_shader_ref vertexShader;
    agpu_shader_ref fragmentShader;

    agpu_state_tracker_cache_ref stateTrackerCache;
    agpu_state_tracker_ref stateTracker;

    agpu_texture_ref checkboardTexture;
    agpu_sampler_ref sampler;
    agpu_renderpass_ref mainRenderPass;

    SampleMeshPtr cubeMesh;
    TransformationState transformationState;

    agpu_window_scraper_ref windowScraper;
    agpu_window_scraper_handle_ref windowScraperHandle;
    agpu_texture_ref scrapedTexture;

    bool draggingLeft;
    bool draggingRight;
    glm::vec3 cameraAngle;
    glm::vec3 cameraPosition;
};

SAMPLE_MAIN(Sample5)
