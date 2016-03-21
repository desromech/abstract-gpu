#include "SampleBase.hpp"
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

class Sample3 : public SampleBase
{
public:
    bool initializeSample()
    {
        // Create the programs.
        auto vertexShader = compileShaderFromFile("data/shaders/texturedVertex", AGPU_VERTEX_SHADER);
        auto fragmentShader = compileShaderFromFile("data/shaders/texturedFragment", AGPU_FRAGMENT_SHADER);
        if (!vertexShader || !fragmentShader)
            return false;

        // Load the texture.
        diffuseTexture = loadTexture("data/textures/checkboard.bmp");
        if(!diffuseTexture)
            return false;

        // Create the shader signature.
        auto shaderSignatureBuilder = agpuCreateShaderSignatureBuilder(device);
        agpuAddShaderSignatureBindingBank(shaderSignatureBuilder, AGPU_SHADER_BINDING_TYPE_CBV, 1, 1);
        agpuAddShaderSignatureBindingBank(shaderSignatureBuilder, AGPU_SHADER_BINDING_TYPE_SRV, 1, 1);
        agpuAddShaderSignatureBindingBank(shaderSignatureBuilder, AGPU_SHADER_BINDING_TYPE_SAMPLER, 1, 1);

        shaderSignature = agpuBuildShaderSignature(shaderSignatureBuilder);
        agpuReleaseShaderSignatureBuilder(shaderSignatureBuilder);
        if (!shaderSignature)
            return false;

        // Create the vertex layout.
        vertexLayout = agpuCreateVertexLayout(device);
        agpuAddVertexAttributeBindings(vertexLayout, 1, SampleVertex::DescriptionSize, SampleVertex::Description);

        // Create the pipeline builder
        auto pipelineBuilder = agpuCreatePipelineBuilder(device);
        agpuSetPipelineShaderSignature(pipelineBuilder, shaderSignature);
        agpuAttachShader(pipelineBuilder, vertexShader);
        agpuAttachShader(pipelineBuilder, fragmentShader);
        agpuSetVertexLayout(pipelineBuilder, vertexLayout);
        agpuSetPrimitiveType(pipelineBuilder, AGPU_PRIMITIVE_TYPE_TRIANGLE);

        // Build the pipeline
        pipeline = buildPipeline(pipelineBuilder);
        agpuReleasePipelineBuilder(pipelineBuilder);
        if (!pipeline)
            return false;

        // Create the vertex and the index buffer
        vertexBuffer = createImmutableVertexBuffer(sizeof(vertices)/sizeof(vertices[0]), sizeof(vertices[0]), vertices);
        indexBuffer = createImmutableIndexBuffer(sizeof(indices)/sizeof(indices[0]), sizeof(indices[0]), indices);

        // Create the transformation buffer.
        transformationBuffer = createUploadableUniformBuffer(sizeof(TransformationState), nullptr);

        // Create the shader bindings.
        shaderBindings = agpuCreateShaderResourceBinding(shaderSignature, 0);
        textureBindings = agpuCreateShaderResourceBinding(shaderSignature, 1);
        samplerBindings = agpuCreateShaderResourceBinding(shaderSignature, 2);
        agpuBindUniformBuffer(shaderBindings, 0, transformationBuffer);
        agpuBindTexture(textureBindings, 0, diffuseTexture, 0, -1, 0.0f);

        agpu_sampler_description samplerDesc;
        memset(&samplerDesc, 0, sizeof(samplerDesc));
        samplerDesc.filter = AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST;
        samplerDesc.address_u = AGPU_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc.address_v = AGPU_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc.address_w = AGPU_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc.max_lod = MaxLod;
        agpuCreateSampler(samplerBindings, 0, &samplerDesc);

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
        transformationState.projectionMatrix = glm::ortho(-w, w, -h, h, -10.0f, 10.0f);

        // Upload the transformation state.
        agpuUploadBufferData(transformationBuffer, 0, sizeof(transformationState), &transformationState);

        // Build the command list
        agpuResetCommandAllocator(commandAllocator);
        agpuResetCommandList(commandList, commandAllocator, pipeline);
        auto backBuffer = agpuGetCurrentBackBuffer(swapChain);
        agpuSetShaderSignature(commandList, shaderSignature);
        agpuBeginFrame(commandList, backBuffer, false);

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
        agpuUseShaderResources(commandList, textureBindings);
        agpuUseShaderResources(commandList, samplerBindings);

        // Draw the objects
        agpuDrawElements(commandList, sizeof(indices) / sizeof(indices[0]), 1, 0, 0, 0);

        // Finish the command list
        agpuEndFrame(commandList);
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
        agpuReleaseShaderResourceBinding(textureBindings);
        agpuReleaseShaderResourceBinding(samplerBindings);
        agpuReleaseShaderSignature(shaderSignature);

        agpuReleaseVertexLayout(vertexLayout);
        agpuReleaseTexture(diffuseTexture);

        agpuReleasePipelineState(pipeline);
        agpuReleaseCommandList(commandList);
        agpuReleaseCommandAllocator(commandAllocator);
    }

    agpu_buffer *transformationBuffer;
    agpu_shader_signature *shaderSignature;
    agpu_shader_resource_binding *shaderBindings;
    agpu_shader_resource_binding *textureBindings;
    agpu_shader_resource_binding *samplerBindings;

    agpu_buffer *vertexBuffer;
    agpu_buffer *indexBuffer;
    agpu_vertex_layout *vertexLayout;
    agpu_vertex_binding *vertexBinding;
    agpu_pipeline_state *pipeline;
    agpu_command_allocator *commandAllocator;
    agpu_command_list *commandList;

    agpu_texture *diffuseTexture;

    TransformationState transformationState;
};

SAMPLE_MAIN(Sample3)
