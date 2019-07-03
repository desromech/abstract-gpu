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
        // Create the render pass.
        mainRenderPass = createMainPass();
        if(!mainRenderPass)
            return false;

        // Create the programs.
        auto vertexShader = compileShaderFromFile("data/shaders/simpleVertex.glsl", AGPU_VERTEX_SHADER);
        auto fragmentShader = compileShaderFromFile("data/shaders/simpleFragment.glsl", AGPU_FRAGMENT_SHADER);
        if (!vertexShader || !fragmentShader)
            return false;

        // Create the shader signature.
        auto shaderSignatureBuilder = agpuCreateShaderSignatureBuilder(device);
        agpuBeginShaderSignatureBindingBank(shaderSignatureBuilder, 1);
        agpuAddShaderSignatureBindingBankElement(shaderSignatureBuilder, AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER, 1);

        shaderSignature = agpuBuildShaderSignature(shaderSignatureBuilder);
        agpuReleaseShaderSignatureBuilder(shaderSignatureBuilder);
        if (!shaderSignature)
            return false;

        // Create the vertex layout.
        vertexLayout = agpuCreateVertexLayout(device);
        agpu_size vertexStride = sizeof(SampleVertex);
        agpuAddVertexAttributeBindings(vertexLayout, 1, &vertexStride, SampleVertex::DescriptionSize, SampleVertex::Description);

        // Create the pipeline builder
        auto pipelineBuilder = agpuCreatePipelineBuilder(device);
        agpuSetPipelineShaderSignature(pipelineBuilder, shaderSignature);
        agpuAttachShader(pipelineBuilder, vertexShader);
        agpuAttachShader(pipelineBuilder, fragmentShader);
        agpuSetVertexLayout(pipelineBuilder, vertexLayout);
        agpuSetPrimitiveType(pipelineBuilder, AGPU_TRIANGLES);

        // Build the pipeline
        pipeline = buildPipeline(pipelineBuilder);
        agpuReleasePipelineBuilder(pipelineBuilder);
        if (!pipeline)
            return false;

        // Create the vertex and the index buffer
        vertexBuffer = createImmutableVertexBuffer(3, sizeof(vertices[0]), vertices);
        indexBuffer = createImmutableIndexBuffer(3, sizeof(indices[0]), indices);

        // Create the transformation buffer.
        transformationBuffer = createUploadableUniformBuffer(sizeof(TransformationState), nullptr);

        // Create the shader bindings.
        shaderBindings = agpuCreateShaderResourceBinding(shaderSignature, 0);
        agpuBindUniformBuffer(shaderBindings, 0, transformationBuffer);

        // Create the vertex buffer binding.
        vertexBinding = agpuCreateVertexBinding(device, vertexLayout);
        agpuBindVertexBuffers(vertexBinding, 1, &vertexBuffer);

        // Create the command list
        commandAllocator = agpuCreateCommandAllocator(device, AGPU_COMMAND_LIST_TYPE_DIRECT, commandQueue);
        commandList = agpuCreateCommandList(device, AGPU_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr);
        agpuCloseCommandList(commandList);
        return true;
    }

    void render()
    {
        // Compute the projection matrix
        float aspect = float(screenWidth) / float(screenHeight);
        float h = 2.0;
        float w = h*aspect;
        transformationState.projectionMatrix = ortho(-w, w, -h, h, -10.0f, 10.0f);

        // Upload the transformation state.
        agpuUploadBufferData(transformationBuffer, 0, sizeof(transformationState), &transformationState);

        // Build the command list
        agpuResetCommandAllocator(commandAllocator);
        agpuResetCommandList(commandList, commandAllocator, nullptr);
        auto backBuffer = agpuGetCurrentBackBuffer(swapChain);
        agpuSetShaderSignature(commandList, shaderSignature);
        agpuBeginRenderPass(commandList, mainRenderPass, backBuffer, false);
        agpuUsePipelineState(commandList, pipeline);

        // Set the viewport
        agpuSetViewport(commandList, 0, 0, screenWidth, screenHeight);
        agpuSetScissor(commandList, 0, 0, screenWidth, screenHeight);

        // Use the vertices and the indices.
        agpuUseVertexBinding(commandList, vertexBinding);
        agpuUseIndexBuffer(commandList, indexBuffer);
        agpuUseShaderResources(commandList, shaderBindings);

        // Draw the objects
        agpuDrawElements(commandList, 3, 1, 0, 0, 0);

        // Finish the command list
        agpuEndRenderPass(commandList);
        agpuCloseCommandList(commandList);

        // Queue the command list
        agpuAddCommandList(commandQueue, commandList);
        swapBuffers();

        agpuFinishQueueExecution(commandQueue);
        agpuReleaseFramebuffer(backBuffer);
    }

    void shutdownSample()
    {
        agpuReleaseBuffer(vertexBuffer);
        agpuReleaseBuffer(indexBuffer);
        agpuReleaseBuffer(transformationBuffer);
        agpuReleaseVertexBinding(vertexBinding);

        agpuReleaseShaderResourceBinding(shaderBindings);
        agpuReleaseShaderSignature(shaderSignature);

        agpuReleaseVertexLayout(vertexLayout);

        agpuReleasePipelineState(pipeline);
        agpuReleaseCommandList(commandList);
        agpuReleaseCommandAllocator(commandAllocator);
    }

    agpu_buffer *transformationBuffer;
    agpu_shader_signature *shaderSignature;
    agpu_shader_resource_binding *shaderBindings;

    agpu_buffer *vertexBuffer;
    agpu_buffer *indexBuffer;
    agpu_vertex_layout *vertexLayout;
    agpu_vertex_binding *vertexBinding;
    agpu_pipeline_state *pipeline;
    agpu_command_allocator *commandAllocator;
    agpu_command_list *commandList;
    agpu_renderpass *mainRenderPass;

    TransformationState transformationState;
};

SAMPLE_MAIN(Sample2)
