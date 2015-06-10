#include "SampleBase.hpp"


static SampleVertex vertices[] = {
    SampleVertex::onlyColor(-1.0, -1.0, 0.0, 1.0, 0.0, 0.0, 1.0),
    SampleVertex::onlyColor(-1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0),
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
        
        // Use the vertex buffer
        
        // Use the index buffer
        
        // Draw the objects
        
        swapBuffers();
    }
    
    agpu_buffer *vertexBuffer;
    agpu_buffer *indexBuffer;
    agpu_program *program;
};

SAMPLE_MAIN(Sample2)
