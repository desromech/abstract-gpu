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

        // Draw the mesh.
        immediateRenderer->setTexturingEnabled(true);
        immediateRenderer->bindTexture(diffuseTexture);
        cubeMesh->beginDrawingWithImmediateRenderer(immediateRenderer);
        cubeMesh->drawWithImmediateRenderer(immediateRenderer);
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
