#include "SampleBase.hpp"


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
        
        // Create the program
        program = createProgramFromFiles("shaders/simple.glslv", "shaders/simple.glslf");
        if(!program)
            return false;
        return true;
    }

    void render()
    {
        auto context = agpuGetImmediateContext(device);
        if(agpuMakeCurrent(context) != AGPU_OK)
        {
            fprintf(stderr, "Failed to use agpu immediate context");
            return;
        }

        // Clear the background
        agpuSetClearColor(context, 0, 0, 0, 0);
        agpuClear(context, AGPU_COLOR_BUFFER_BIT | AGPU_DEPTH_BUFFER_BIT);

        // Use the program
        agpuUseProgram(context, program);
        
        // Set some matrices.
        agpuSetUniformMatrix4f(context, agpuGetUniformLocation(program, "projectionMatrix"), 1, AGPU_FALSE, (agpu_float*)&projectionMatrix);
        agpuSetUniformMatrix4f(context, agpuGetUniformLocation(program, "viewMatrix"), 1, AGPU_FALSE, (agpu_float*)&viewMatrix);
        agpuSetUniformMatrix4f(context, agpuGetUniformLocation(program, "modelMatrix"), 1, AGPU_FALSE, (agpu_float*)&modelMatrix);
        
        // Use the vertices and the indices.
        agpuUseVertexBinding(context, vertexBinding);
        agpuUseIndexBuffer(context, indexBuffer);
        agpuUseDrawBuffer(context, drawBuffer);
        
        // Draw the objects
        agpuDrawElementsIndirect(context, AGPU_TRIANGLES, 0);
        
        // Swap the front and back buffer
        swapBuffers();
    }
    
    agpu_buffer *vertexBuffer;
    agpu_buffer *indexBuffer;
    agpu_buffer *drawBuffer;
    agpu_vertex_binding *vertexBinding;
    agpu_program *program;
    
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 modelMatrix;
};

SAMPLE_MAIN(Sample2)
