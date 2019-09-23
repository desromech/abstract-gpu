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
        auto viewMatrix = glm::inverse(cameraModelMatrix());
        immediateRenderer->multiplyMatrix(&viewMatrix[0][0]);

        // Begin drawing the mesh.
        cubeMesh->beginDrawingWithImmediateRenderer(immediateRenderer);

        // First row: without texture.
        immediateRenderer->setTexturingEnabled(false);
        {
            immediateRenderer->pushMatrix();
            immediateRenderer->setLightingEnabled(false);
            drawCubeMeshRow();
            immediateRenderer->popMatrix();
        }

        // Second row: with textures.
        immediateRenderer->setTexturingEnabled(true);
        immediateRenderer->bindTexture(diffuseTexture);
        {
            immediateRenderer->pushMatrix();
            immediateRenderer->translate(0.0f, 0.0f, -2.0f);
            immediateRenderer->setLightingEnabled(false);
            drawCubeMeshRow();
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
            drawCubeMeshRow();
            immediateRenderer->popMatrix();
        }

        // Fourth row: with textures and fog
        immediateRenderer->setTexturingEnabled(true);
        immediateRenderer->bindTexture(diffuseTexture);
        {
            immediateRenderer->pushMatrix();
            immediateRenderer->translate(0.0f, 0.0f, -6.0f);
            drawCubeMeshRow();
            immediateRenderer->popMatrix();
        }

        // Fifth row: without textures and fog.
        immediateRenderer->setFogMode(AGPU_IMMEDIATE_RENDERER_FOG_MODE_EXPONENTIAL);
        immediateRenderer->setFogDensity(0.1f);
        immediateRenderer->setTexturingEnabled(false);
        {
            immediateRenderer->pushMatrix();
            immediateRenderer->translate(0.0f, 0.0f, -8.0f);
            drawCubeMeshRow();
            immediateRenderer->popMatrix();
        }

        // Sixth row: with textures and fog
        immediateRenderer->setTexturingEnabled(true);
        immediateRenderer->bindTexture(diffuseTexture);
        {
            immediateRenderer->pushMatrix();
            immediateRenderer->translate(0.0f, 0.0f, -10.0f);
            drawCubeMeshRow();
            immediateRenderer->popMatrix();
        }

        // Seventh row: without textures and fog.
        immediateRenderer->setFogMode(AGPU_IMMEDIATE_RENDERER_FOG_MODE_EXPONENTIAL_SQUARED);
        immediateRenderer->setFogDensity(0.1f);
        immediateRenderer->setTexturingEnabled(false);
        {
            immediateRenderer->pushMatrix();
            immediateRenderer->translate(0.0f, 0.0f, -12.0f);
            drawCubeMeshRow();
            immediateRenderer->popMatrix();
        }

        // Eight row: with textures and fog
        immediateRenderer->setTexturingEnabled(true);
        immediateRenderer->bindTexture(diffuseTexture);
        {
            immediateRenderer->pushMatrix();
            immediateRenderer->translate(0.0f, 0.0f, -14.0f);
            drawCubeMeshRow();
            immediateRenderer->popMatrix();
        }

        // End drawing the mesh.
        cubeMesh->endDrawingWithImmediateRenderer(immediateRenderer);

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

    void drawCubeMeshMaterialRow()
    {
        auto gap = 2.0f;
        auto currentX = -(CubeAlbedoCount * gap / 2.0);
        agpu_vector4f specular = {0.04f, 0.04f, 0.04f, 0.04f};

        for(size_t i = 0; i < CubeAlbedoCount; ++i, currentX += gap)
        {
            auto cubeAlbedo = CubeAlbedos[i];

            agpu_vector4f albedo = {cubeAlbedo.x, cubeAlbedo.y, cubeAlbedo.z, cubeAlbedo.w};
            agpu_immediate_renderer_material material = {};
            material.ambient = material.diffuse = albedo;
            material.specular = specular;
            material.shininess = 20.0f;

            immediateRenderer->pushMatrix();
            immediateRenderer->translate(currentX, 0.0f, 0.0f);
            immediateRenderer->setMaterial(&material);
            cubeMesh->drawWithImmediateRenderer(immediateRenderer);

            immediateRenderer->popMatrix();
        }
    }

    void drawCubeMeshRow()
    {
        // Bottom - unlighted
        immediateRenderer->pushMatrix();
        immediateRenderer->translate(0.0f, -2.0f, 0.0f);
        immediateRenderer->setLightingEnabled(false);
        drawCubeMeshMaterialRow();
        immediateRenderer->popMatrix();

        // Center - lighted
        immediateRenderer->pushMatrix();
        immediateRenderer->setLightingEnabled(true);
        drawCubeMeshMaterialRow();
        immediateRenderer->popMatrix();
    }

    agpu_state_tracker_cache_ref stateTrackerCache;
    agpu_state_tracker_ref stateTracker;
    agpu_immediate_renderer_ref immediateRenderer;

    agpu_texture_ref diffuseTexture;
    agpu_renderpass_ref mainRenderPass;

    SampleMeshPtr cubeMesh;

    bool draggingLeft;
    bool draggingRight;
    glm::vec3 cameraAngle;
    glm::vec3 cameraPosition;
};

SAMPLE_MAIN(Sample5)
