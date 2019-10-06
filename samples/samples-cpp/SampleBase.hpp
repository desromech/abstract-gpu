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

class AbstractSampleBase
{
public:
    agpu_shader_ref compileShaderFromFile(const char *fileName, agpu_shader_type type);
    agpu_buffer_ref createImmutableVertexBuffer(size_t capacity, size_t vertexSize, void *initialData);
    agpu_buffer_ref createImmutableIndexBuffer(size_t capacity, size_t indexSize, void *initialData);
    agpu_buffer_ref createImmutableDrawBuffer(size_t capacity, void *initialData);
    agpu_buffer_ref createUploadableUniformBuffer(size_t capacity, void *initialData);
    agpu_buffer_ref createMappableUploadBuffer(size_t capacity, void *initialData);
    agpu_buffer_ref createMappableReadbackBuffer(size_t capacity, void *initialData);
    agpu_buffer_ref createStorageBuffer(size_t capacity, size_t stride, void *initialData);


    agpu_pipeline_state_ref buildPipeline(const agpu_pipeline_builder_ref &builder);
    agpu_pipeline_state_ref buildComputePipeline(const agpu_compute_pipeline_builder_ref &builder);
    agpu_texture_ref loadTexture(const char *fileName);

    const agpu_vertex_layout_ref &getSampleVertexLayout();

    agpu_device_ref device;
    agpu_command_queue_ref commandQueue;
    agpu_shader_language preferredShaderLanguage;

	bool hasPersistentCoherentMapping;

private:
    agpu_vertex_layout_ref sampleVertexLayout;
};

class SampleBase : public AbstractSampleBase
{
public:
    int main(int argc, const char **argv);

    virtual bool initializeSample();
    virtual void shutdownSample();
    virtual void render();
    virtual void processEvents();
    virtual void update(float deltaTime);

    virtual void onMouseButtonDown(const SDL_MouseButtonEvent &event);
    virtual void onMouseButtonUp(const SDL_MouseButtonEvent &event);
    virtual void onMouseMotion(const SDL_MouseMotionEvent &event);
    virtual void onMouseWheel(const SDL_MouseWheelEvent &event);

    virtual void onKeyDown(const SDL_KeyboardEvent &event);
    virtual void onKeyUp(const SDL_KeyboardEvent &event);

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

    glm::mat4 frustum(float left, float right, float bottom, float top, float nearDistance, float farDistance)
    {
        glm::mat4 m(0.0f);
        m[0][0] = 2.0*nearDistance / (right - left); m[2][0] = (right + left) / (right - left);
        m[1][1] = 2.0*nearDistance / (top - bottom); m[2][1] = (top + bottom) / (top - bottom);
        m[2][2] = -farDistance / (farDistance - nearDistance); m[3][2] = -nearDistance * farDistance / (farDistance - nearDistance);
        m[2][3] = -1.0f;

        // Flip the Y axis
        if (device->hasTopLeftNdcOrigin())
        {
            m[1][1] = -m[1][1];
            m[2][1] = -m[2][1];
        }

        return m;
    }

    glm::mat4 perspective(float fovy, float aspect, float nearDistance, float farDistance)
    {
        auto radians = fovy*(M_PI/180.0f*0.5f);
        auto top = nearDistance * tan(radians);
        auto right = top * aspect;
        return frustum(-right, right, -top, top, nearDistance, farDistance);
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
