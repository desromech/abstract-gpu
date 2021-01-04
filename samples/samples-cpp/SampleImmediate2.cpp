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

static const glm::vec4 CubeAlbedos[] = {
    glm::vec4(0.8f, 0.1f, 0.1f, 1.0f),
    glm::vec4(0.1f, 0.8f, 0.1f, 1.0f),
    glm::vec4(0.1f, 0.1f, 0.8f, 1.0f),

    glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),

    glm::vec4(0.8f, 0.8f, 0.1f, 1.0f),
    glm::vec4(0.1f, 0.8f, 0.8f, 1.0f),
    glm::vec4(0.8f, 0.1f, 0.8f, 1.0f),
};

constexpr uint32_t CubeAlbedoCount = sizeof(CubeAlbedos) / sizeof(CubeAlbedos[0]);


class Sample5: public SampleBase
{
public:
    Sample5()
    {
        draggingLeft = false;
        draggingRight = false;
        lightingModel = AGPU_IMMEDIATE_RENDERER_LIGHTING_MODEL_PER_VERTEX;
        cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
    }

    virtual bool initializeSample() override
    {
        mainRenderPass = createMainPass();
        if(!mainRenderPass)
            return false;

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


        stateTrackerCache = device->createStateTrackerCache(commandQueue);
        stateTracker = stateTrackerCache->createStateTracker(AGPU_COMMAND_LIST_TYPE_DIRECT, commandQueue);
        immediateRenderer = stateTrackerCache->createImmediateRenderer();

        return true;
    }

    virtual void onKeyDown(const SDL_KeyboardEvent &event) override
    {
        switch(event.keysym.sym)
        {
        case SDLK_TAB:
            lightingModel = agpu_immediate_renderer_lighting_model((lightingModel + 1) % AGPU_IMMEDIATE_RENDERER_LIGHTING_MODEL_COUNT);
            printf("Lighting model selected: %d\n", lightingModel);
            break;
        default:
            break;
        }

        SampleBase::onKeyDown(event);
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
        // Begin rendering the frame
        auto backBuffer = swapChain->getCurrentBackBuffer();
        stateTracker->beginRecordingCommands();
        stateTracker->beginRenderPass(mainRenderPass, backBuffer, false);
        immediateRenderer->beginRendering(stateTracker);

        immediateRenderer->setViewport(0, 0, screenWidth, screenHeight);
        immediateRenderer->setScissor(0, 0, screenWidth, screenHeight);

        // Enable face culling and depth testing.
        immediateRenderer->setCullMode(AGPU_CULL_MODE_BACK);
        immediateRenderer->setDepthState(true, true, AGPU_LESS_EQUAL);

        // Setup the projection matrix.
        immediateRenderer->projectionMatrixMode();
        immediateRenderer->loadIdentity();
        float aspect = float(screenWidth) / float(screenHeight);
        immediateRenderer->perspective(60.0, aspect, 0.01f, 100.0f);

        // Setup the model view matrix.
        immediateRenderer->modelViewMatrixMode();
        immediateRenderer->loadIdentity();

        // Setup the lighting model.
        immediateRenderer->setLightingModel(lightingModel);

        immediateRenderer->clearLights();

        // Set the camera matrix.
        auto viewMatrix = glm::inverse(cameraModelMatrix());
        immediateRenderer->multiplyMatrix(&viewMatrix[0][0]);

        // Set the directional light.
        {
            auto lightDirection = glm::normalize(glm::vec3(0.5f, 0.9f, -1.0f));
            auto intensity = glm::vec3(0.8f);
            agpu_immediate_renderer_light lightState = {};
            if(hasMetallicRoughnessLighting())
            {
                lightState.pbr.intensity = {intensity.r, intensity.g, intensity.b};
                lightState.pbr.position = {lightDirection.x, lightDirection.y, lightDirection.z, 0.0f};
                lightState.pbr.spot_direction = {0.0f, 0.0f, -1.0f};
                lightState.pbr.spot_cutoff = lightState.pbr.spot_inner_cutoff = 180.0f;
                lightState.pbr.radius = 1.0f;
            }
            else
            {
                lightState.classic.diffuse = {intensity.r, intensity.g, intensity.b, 1.0f};
                lightState.classic.specular = {intensity.r, intensity.g, intensity.b, 1.0f};
                lightState.classic.position = {lightDirection.x, lightDirection.y, lightDirection.z, 0.0f};
                lightState.classic.constant_attenuation = 1.0f;
                lightState.classic.spot_direction = {0.0f, 0.0f, -1.0f};
                lightState.classic.spot_cutoff = 180.0f;
            }
            immediateRenderer->setLight(0, true, &lightState);
        }

        // Set the center light.
        {
            auto center = glm::vec3(0.0f, 0.0f, -6.0f);
            agpu_immediate_renderer_light lightState = {};
            auto intensity = glm::vec3(2.0f);
            if(hasMetallicRoughnessLighting())
            {
                lightState.pbr.intensity = {intensity.r, intensity.g, intensity.b};
                lightState.pbr.position = {center.x, center.y, center.z, 1.0f};
                lightState.pbr.spot_direction = {0.0f, 0.0f, -1.0f};
                lightState.pbr.spot_cutoff = lightState.pbr.spot_inner_cutoff = 180.0f;
                lightState.pbr.radius = 100.0f;
            }
            else
            {
                lightState.classic.diffuse = {intensity.r, intensity.g, intensity.b, 1.0f};
                lightState.classic.specular = {intensity.r, intensity.g, intensity.b, 1.0f};
                lightState.classic.position = {center.x, center.y, center.z, 0.0f};
                lightState.classic.constant_attenuation = 1.0f;
                lightState.classic.quadratic_attenuation = 0.01f;
                lightState.classic.spot_direction = {0.0f, 0.0f, -1.0f};
                lightState.classic.spot_cutoff = 180.0f;
            }
            immediateRenderer->setLight(1, true, &lightState);
        }

        // Draw the cubes with implicit rendering.
        {
            immediateRenderer->pushMatrix();
            drawCubes(false);
            immediateRenderer->popMatrix();
        }

        // Draw the cube with explicit buffers.
        {
            immediateRenderer->pushMatrix();
            immediateRenderer->translate(0.0, 4.0, 0.0);
            drawCubes(true);
            immediateRenderer->popMatrix();
        }

        // End rendering the frame.
        immediateRenderer->endRendering();
        stateTracker->endRenderPass();
        stateTracker->endRecordingAndFlushCommands();

        swapBuffers();
        commandQueue->finishExecution();
    }

    virtual void shutdownSample() override
    {
    }

    void drawCubeMeshMaterialRow(bool explicitBuffers)
    {
        auto gap = 2.0f;
        auto currentX = -(CubeAlbedoCount * gap / 2.0);
        agpu_vector4f specular = {0.04f, 0.04f, 0.04f, 1.0f};

        for(size_t i = 0; i < CubeAlbedoCount; ++i, currentX += gap)
        {
            auto cubeAlbedo = CubeAlbedos[i];

            agpu_vector4f albedo = {cubeAlbedo.x, cubeAlbedo.y, cubeAlbedo.z, cubeAlbedo.w};
            agpu_immediate_renderer_material material = {};
            if(hasMetallicRoughnessLighting())
            {
                material.metallic_roughness.base_color = albedo;
                material.metallic_roughness.roughness_factor = 0.4f;
                material.metallic_roughness.metallic_factor = 0.0f;
                material.metallic_roughness.occlusion_factor = 1.0f;
            }
            else
            {
                auto diffuseFactor = 1.0f - 0.04f;
                material.classic.ambient = albedo;
                material.classic.diffuse = {albedo.x *diffuseFactor, albedo.y *diffuseFactor, albedo.z *diffuseFactor, albedo.w *diffuseFactor};
                material.classic.specular = specular;
                material.classic.shininess = 50.0f;
            }

            immediateRenderer->pushMatrix();
            immediateRenderer->translate(currentX, 0.0f, 0.0f);
            immediateRenderer->setMaterial(&material);
            cubeMesh->drawWithImmediateRenderer(immediateRenderer, explicitBuffers);

            immediateRenderer->popMatrix();
        }
    }

    void drawCubeMeshRow(bool explicitBuffers)
    {
        // Bottom - unlighted
        immediateRenderer->pushMatrix();
        immediateRenderer->translate(0.0f, -2.0f, 0.0f);
        immediateRenderer->setLightingEnabled(false);
        drawCubeMeshMaterialRow(explicitBuffers);
        immediateRenderer->popMatrix();

        // Center - lighted
        immediateRenderer->pushMatrix();
        immediateRenderer->setLightingEnabled(true);
        drawCubeMeshMaterialRow(explicitBuffers);
        immediateRenderer->popMatrix();
    }

    void drawCubes(bool explicitBuffers)
    {
        // Begin drawing the mesh.
        cubeMesh->beginDrawingWithImmediateRenderer(immediateRenderer, explicitBuffers);

        // First row: without texture.
        immediateRenderer->setTexturingEnabled(false);
        {
            immediateRenderer->pushMatrix();
            immediateRenderer->setLightingEnabled(false);
            drawCubeMeshRow(explicitBuffers);
            immediateRenderer->popMatrix();
        }

        // Second row: with textures.
        immediateRenderer->setTexturingEnabled(true);
        immediateRenderer->bindTexture(diffuseTexture);
        {
            immediateRenderer->pushMatrix();
            immediateRenderer->translate(0.0f, 0.0f, -2.0f);
            immediateRenderer->setLightingEnabled(false);
            drawCubeMeshRow(explicitBuffers);
            immediateRenderer->popMatrix();
        }

        // Third row: without textures and fog.
        immediateRenderer->setFogMode(AGPU_IMMEDIATE_RENDERER_FOG_MODE_LINEAR);
        immediateRenderer->setFogColor(0.0f, 0.0f, 0.0f, 0.0);
        immediateRenderer->setFogDistances(0.0f, 10.0f);
        immediateRenderer->setTexturingEnabled(false);
        {
            immediateRenderer->pushMatrix();
            immediateRenderer->translate(0.0f, 0.0f, -4.0f);
            drawCubeMeshRow(explicitBuffers);
            immediateRenderer->popMatrix();
        }

        // Fourth row: with textures and fog
        immediateRenderer->setTexturingEnabled(true);
        immediateRenderer->bindTexture(diffuseTexture);
        {
            immediateRenderer->pushMatrix();
            immediateRenderer->translate(0.0f, 0.0f, -6.0f);
            drawCubeMeshRow(explicitBuffers);
            immediateRenderer->popMatrix();
        }

        // Fifth row: without textures and fog.
        immediateRenderer->setFogMode(AGPU_IMMEDIATE_RENDERER_FOG_MODE_EXPONENTIAL);
        immediateRenderer->setFogDensity(0.1f);
        immediateRenderer->setTexturingEnabled(false);
        {
            immediateRenderer->pushMatrix();
            immediateRenderer->translate(0.0f, 0.0f, -8.0f);
            drawCubeMeshRow(explicitBuffers);
            immediateRenderer->popMatrix();
        }

        // Sixth row: with textures and fog
        immediateRenderer->setTexturingEnabled(true);
        immediateRenderer->bindTexture(diffuseTexture);
        {
            immediateRenderer->pushMatrix();
            immediateRenderer->translate(0.0f, 0.0f, -10.0f);
            drawCubeMeshRow(explicitBuffers);
            immediateRenderer->popMatrix();
        }

        // Seventh row: without textures and fog.
        immediateRenderer->setFogMode(AGPU_IMMEDIATE_RENDERER_FOG_MODE_EXPONENTIAL_SQUARED);
        immediateRenderer->setFogDensity(0.1f);
        immediateRenderer->setTexturingEnabled(false);
        {
            immediateRenderer->pushMatrix();
            immediateRenderer->translate(0.0f, 0.0f, -12.0f);
            drawCubeMeshRow(explicitBuffers);
            immediateRenderer->popMatrix();
        }

        // Eight row: with textures and fog
        immediateRenderer->setTexturingEnabled(true);
        immediateRenderer->bindTexture(diffuseTexture);
        {
            immediateRenderer->pushMatrix();
            immediateRenderer->translate(0.0f, 0.0f, -14.0f);
            drawCubeMeshRow(explicitBuffers);
            immediateRenderer->popMatrix();
        }

        // End drawing the mesh.
        cubeMesh->endDrawingWithImmediateRenderer(immediateRenderer);
    }

    bool hasMetallicRoughnessLighting()
    {
        return lightingModel == AGPU_IMMEDIATE_RENDERER_LIGHTING_MODEL_METALLIC_ROUGHNESS;
    }

    agpu_state_tracker_cache_ref stateTrackerCache;
    agpu_state_tracker_ref stateTracker;
    agpu_immediate_renderer_ref immediateRenderer;

    agpu_texture_ref diffuseTexture;
    agpu_renderpass_ref mainRenderPass;
    agpu_immediate_renderer_lighting_model lightingModel;

    SampleMeshPtr cubeMesh;

    bool draggingLeft;
    bool draggingRight;
    glm::vec3 cameraAngle;
    glm::vec3 cameraPosition;
};

SAMPLE_MAIN(Sample5)
