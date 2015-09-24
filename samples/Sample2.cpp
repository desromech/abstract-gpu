#include "SampleBase.hpp"
#include "glm/gtc/matrix_transform.hpp"

static SampleVertex vertices[] = {
    SampleVertex::onlyColor(-1.0, -1.0, 0.0, 1.0, 0.0, 0.0, 1.0),
    SampleVertex::onlyColor(0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0),
    SampleVertex::onlyColor(1.0, -1.0, 0.0, 0.0, 0.0, 1.0, 1.0),
};

static uint32_t indices[] = {
    0, 1, 2
};

struct TransformationState
{
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 modelMatrix;
};

class Sample2: public SampleBase
{
public:
    bool initializeSample()
    {
        // Create the programs.
        auto vertexShader = compileShaderFromFile("data/shaders/simpleVertex", AGPU_VERTEX_SHADER);
        auto fragmentShader = compileShaderFromFile("data/shaders/simpleFragment", AGPU_FRAGMENT_SHADER);
        if (!vertexShader || !fragmentShader)
            return false;

        // Create the vertex layout.
        vertexLayout = agpuCreateVertexLayout(device);
        agpuAddVertexAttributeBindings(vertexLayout, 1, SampleVertex::DescriptionSize, SampleVertex::Description);

        // Create the pipeline builder
        auto pipelineBuilder = agpuCreatePipelineBuilder(device);
        agpuAttachShader(pipelineBuilder, vertexShader);
        agpuAttachShader(pipelineBuilder, fragmentShader);
        agpuSetVertexLayout(pipelineBuilder, vertexLayout);
        agpuSetPrimitiveType(pipelineBuilder, AGPU_PRIMITIVE_TYPE_TRIANGLE);

        // Build the pipeline
        pipeline = buildPipeline(pipelineBuilder);
        if (!pipeline)
            return false;

        // Create the vertex and the index buffer
        vertexBuffer = createImmutableVertexBuffer(3, sizeof(vertices[0]), vertices);
        indexBuffer = createImmutableIndexBuffer(3, sizeof(indices[0]), indices);

        // Create the transformation buffer.
        transformationBuffer = createUploadableUniformBuffer(sizeof(TransformationState), nullptr);

        // Create the shader bindings.
        shaderBindings = agpuCreateShaderResourceBinding(device, 0);
        agpuBindUniformBuffer(shaderBindings, 0, transformationBuffer);

        // Create the vertex buffer binding.
        vertexBinding = agpuCreateVertexBinding(device, vertexLayout);
        agpuBindVertexBuffers(vertexBinding, 1, &vertexBuffer);

        // Create the command list
        commandAllocator = agpuCreateCommandAllocator(device);
        commandList = agpuCreateCommandList(device, commandAllocator, nullptr);
        agpuCloseCommandList(commandList);
        return true;
    }

    void render()
    {
        return;
        
        // Compute the projection matrix
        float aspect = float(screenWidth) / float(screenHeight);
        float h = 2.0;
        float w = h*aspect;
        transformationState.projectionMatrix = glm::ortho(-w, w, -h, h, -10.0f, 10.0f);

        // Upload the transformation state.
        agpuUploadBufferData(transformationBuffer, 0, sizeof(transformationState), &transformationState);

        // Build the command list
        agpuResetCommandAllocator(commandAllocator);
        agpuResetCommandList(commandList, commandAllocator, pipeline);
        //agpuBeginFrame(commandList, agpuGetCurrentBackBuffer(device));

        // Set the viewport
        agpuSetViewport(commandList, 0, 0, screenWidth, screenHeight);
        agpuSetScissor(commandList, 0, 0, screenWidth, screenHeight);
        agpuSetClearColor(commandList, 0, 0, 0, 0);
        agpuClear(commandList, AGPU_COLOR_BUFFER_BIT);

        // Use the vertices and the indices.
        agpuUseVertexBinding(commandList, vertexBinding);
        agpuUseIndexBuffer(commandList, indexBuffer);
        agpuSetPrimitiveTopology(commandList, AGPU_TRIANGLES);
        agpuUseShaderResources(commandList, shaderBindings);

        // Draw the objects
        agpuDrawElements(commandList, 3, 1, 0, 0, 0);

        // Finish the command list
        agpuEndFrame(commandList);
        agpuCloseCommandList(commandList);

        // Queue the command list
        auto queue = agpuGetDefaultCommandQueue(device);
        agpuAddCommandList(queue, commandList);

        swapBuffers();
    }

    void shutdownSample()
    {
        agpuReleaseBuffer(vertexBuffer);
        agpuReleaseBuffer(indexBuffer);
        agpuReleaseBuffer(transformationBuffer);
        agpuReleaseShaderResourceBinding(shaderBindings);
        agpuReleaseVertexLayout(vertexLayout);
        agpuReleaseVertexBinding(vertexBinding);
        agpuReleasePipelineState(pipeline);
        agpuReleaseCommandList(commandList);
        agpuReleaseCommandAllocator(commandAllocator);
    }

    agpu_buffer *transformationBuffer;
    agpu_shader_resource_binding *shaderBindings;

    agpu_buffer *vertexBuffer;
    agpu_buffer *indexBuffer;
    agpu_vertex_layout *vertexLayout;
    agpu_vertex_binding *vertexBinding;
    agpu_pipeline_state *pipeline;
    agpu_command_allocator *commandAllocator;
    agpu_command_list *commandList;

    TransformationState transformationState;
};

SAMPLE_MAIN(Sample2)
