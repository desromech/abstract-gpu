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

class Sample2: public SampleBase
{
public:
    bool initializeSample()
    {
        // Create the programs.
        auto vertexShader = compileShaderFromFile("data/shaders/simple.glslv", AGPU_VERTEX_SHADER);
        auto fragmentShader = compileShaderFromFile("data/shaders/simple.glslf", AGPU_FRAGMENT_SHADER);
        if (!vertexShader || !fragmentShader)
            return false;

        // Create the pipeline builder
        auto pipelineBuilder = agpuCreatePipelineBuilder(device);
        agpuAttachShader(pipelineBuilder, vertexShader);
        agpuAttachShader(pipelineBuilder, fragmentShader);
        agpuSetPrimitiveTopology(pipelineBuilder, AGPU_TRIANGLES);

        // Build the pipeline
        pipeline = buildPipeline(pipelineBuilder);
        if (!pipeline)
            return false;

        // Create the vertex and the index buffer
        vertexBuffer = createImmutableVertexBuffer(3, sizeof(vertices[0]), vertices);
        indexBuffer = createImmutableIndexBuffer(3, sizeof(indices[0]), indices);
        
        // Create the command buffer
        agpu_draw_elements_command command;
        memset(&command, 0, sizeof(command));
        command.count = 3;
        command.instance_count = 1;
        drawBuffer = createImmutableDrawBuffer(1, &command);
        
        // Create the vertex buffer binding.
        vertexBinding = agpuCreateVertexBinding(device);
        agpuAddVertexBufferBindings(vertexBinding, vertexBuffer, SampleVertex::DescriptionSize, SampleVertex::Description);

        // Create the command list
        commandList = agpuCreateCommandList(device, pipeline);
        agpuCloseCommandList(commandList);
        return true;
    }

    void render()
    {
        // Build the command list
        agpuResetCommandList(commandList, nullptr);
        agpuBeginFrame(commandList);

        // Set the viewport
        agpuSetViewport(commandList, 0, 0, screenWidth, screenHeight);
        agpuSetClearColor(commandList, 0, 0, 0, 0);
        agpuClear(commandList, AGPU_COLOR_BUFFER_BIT);

        // Compute the projection matrix
        float aspect = float(screenWidth) / float(screenHeight);
        float h = 2.0;
        float w = h*aspect;
        projectionMatrix = glm::ortho(-w, w, -h, h, -10.0f, 10.0f);

        // Set some matrices.
        agpuSetUniformMatrix4f(commandList, agpuGetUniformLocation(pipeline, "projectionMatrix"), 1, false, (agpu_float*)&projectionMatrix);
        agpuSetUniformMatrix4f(commandList, agpuGetUniformLocation(pipeline, "viewMatrix"), 1, false, (agpu_float*)&viewMatrix);
        agpuSetUniformMatrix4f(commandList, agpuGetUniformLocation(pipeline, "modelMatrix"), 1, false, (agpu_float*)&modelMatrix);

        // Use the vertices and the indices.
        agpuUseVertexBinding(commandList, vertexBinding);
        agpuUseIndexBuffer(commandList, indexBuffer);
        agpuUseDrawIndirectBuffer(commandList, drawBuffer);

        // Draw the objects
        agpuDrawElementsIndirect(commandList, 0);

        // Finish the command list
        agpuEndFrame(commandList);
        agpuCloseCommandList(commandList);

        // Queue the command list
        auto queue = agpuGetDefaultCommandQueue(device);
        agpuAddCommandList(queue, commandList);


        swapBuffers();
    }
    
    agpu_buffer *vertexBuffer;
    agpu_buffer *indexBuffer;
    agpu_buffer *drawBuffer;
    agpu_vertex_binding *vertexBinding;
    agpu_pipeline_state *pipeline;
    agpu_command_list *commandList;
    
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 modelMatrix;
};

SAMPLE_MAIN(Sample2)
