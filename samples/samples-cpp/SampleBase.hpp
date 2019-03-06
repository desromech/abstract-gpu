#ifndef _SAMPLE_BASE_HPP_
#define _SAMPLE_BASE_HPP_

#include "glm/glm.hpp"
#include <AGPU/agpu.hpp>
#include <SDL.h>
#include <SDL_main.h>
#include <string>

// Utility functions
void printMessage(const char *format, ...);
void printError(const char *format, ...);

std::string readWholeFile(const std::string &fileName);

// Vertex used in the samples
struct SampleVertex
{
    SampleVertex(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec4 &color, const glm::vec2 &texcoord)
        : position(position), normal(normal), color(color), texcoord(texcoord) {}

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 color;
    glm::vec2 texcoord;

    static SampleVertex onlyColor(float x, float y, float z, float r, float g, float b, float a)
    {
        return SampleVertex(glm::vec3(x, y, z), glm::vec3(0,0,0), glm::vec4(r, g, b, a), glm::vec2(0, 1));
    }

    static SampleVertex onlyColorTc(float x, float y, float z, float r, float g, float b, float a, float u, float v)
    {
        return SampleVertex(glm::vec3(x, y, z), glm::vec3(0, 0, 0), glm::vec4(r, g, b, a), glm::vec2(u, v));
    }

    static agpu_vertex_attrib_description Description[];
    static const int DescriptionSize;
};

class AbstractSampleBase
{
public:
    agpu_shader_ref compileShaderFromFile(const char *fileName, agpu_shader_type type);
    agpu_buffer_ref createImmutableVertexBuffer(size_t capacity, size_t vertexSize, void *initialData);
    agpu_buffer_ref createImmutableIndexBuffer(size_t capacity, size_t indexSize, void *initialData);
    agpu_buffer_ref createImmutableDrawBuffer(size_t capacity, void *initialData);
    agpu_buffer_ref createUploadableUniformBuffer(size_t capacity, void *initialData);
	agpu_buffer_ref createMappableStorage(size_t capacity, void *initialData);

    agpu_pipeline_state_ref buildPipeline(const agpu_pipeline_builder_ref &builder);
    agpu_pipeline_state_ref buildComputePipeline(const agpu_compute_pipeline_builder_ref &builder);
    agpu_texture_ref loadTexture(const char *fileName);

    agpu_device_ref device;
    agpu_command_queue_ref commandQueue;
    agpu_shader_language preferredShaderLanguage;

	bool hasPersistentCoherentMapping;
};

class SampleBase : public AbstractSampleBase
{
public:
    int main(int argc, const char **argv);

    virtual bool initializeSample();
    virtual void shutdownSample();
    virtual void render();
    virtual void processEvents();

    virtual void onKeyDown(const SDL_Event &event);
    virtual void onKeyUp(const SDL_Event &event);

    void swapBuffers();

protected:
    agpu_renderpass_ref createMainPass(const glm::vec4 &clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

    int screenWidth, screenHeight;
    SDL_Window *window;

    agpu_swap_chain_ref swapChain;
    bool quit;

    glm::mat4 ortho(float left, float right, float bottom, float top, float near, float far)
    {
        glm::mat4 matrix(1.0f);
        matrix[0][0] = 2.0f / (right - left); matrix[3][0] = -(right + left) / (right - left);
        matrix[1][1] = 2.0f / (top - bottom); matrix[3][1] = -(top + bottom) / (top - bottom);

        matrix[2][2] = -1.0f / (far - near); matrix[3][2] = -near / (far - near);

        // Flip the Y axis
        if (device->hasTopLeftNdcOrigin())
        {
            matrix[1][1] = -matrix[1][1];
            matrix[3][1] = -matrix[3][1];
        }

        return matrix;
    }
};

class ComputeSampleBase : public AbstractSampleBase
{
public:
    int main(int argc, const char **argv);

    virtual int run(int argc, const char **argv);
};

#define SAMPLE_MAIN(SampleClass) \
int main(int argc, char *argv[]) \
{ \
    SampleClass sample; \
    return sample.main(argc, (const char **)argv); \
}

#endif //_SAMPLE_BASE_HPP_
