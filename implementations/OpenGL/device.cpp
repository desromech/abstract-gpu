#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "device.hpp"
#include "buffer.hpp"
#include "shader.hpp"
#include "shader_resource_binding.hpp"
#include "pipeline_builder.hpp"
#include "command_allocator.hpp"
#include "command_list.hpp"
#include "command_queue.hpp"
#include "vertex_binding.hpp"
#include "vertex_layout.hpp"
#include "framebuffer.hpp"
#include "swap_chain.hpp"
#include "texture.hpp"

#define LOAD_FUNCTION(functionName) loadExtensionFunction(functionName, #functionName)

void printError(const char *format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 1024, format, args);
#ifdef _WIN32
    OutputDebugStringA(buffer);
#else
    fputs(buffer, stderr);
#endif
    va_end(args);
}

OpenGLVersion GLContextVersionPriorities[] = {
    OpenGLVersion::Version43,
    OpenGLVersion::Version42,
    OpenGLVersion::Version41,
    OpenGLVersion::Version40,
    OpenGLVersion::Version33,
    OpenGLVersion::Version32,
    OpenGLVersion::Version31,
    OpenGLVersion::Version30,
    OpenGLVersion::Version21,
    OpenGLVersion::Version20,
    OpenGLVersion::Invalid
};

std::pair<GLuint, bool> OpenGLContext::getFrameBufferObject(agpu_framebuffer *framebuffer, int newDirtyCount)
{
    std::unique_lock<std::mutex> l(clientResourceMutex);
    assert(isCurrent());

    // Find an existing framebuffer object.
    auto it = framebufferObjects.find(framebuffer);
    if(it == framebufferObjects.end())
    {
        // Create the new framebuffer object.
        GLuint handle;
        device->glGenFramebuffers(1, &handle);
        framebufferObjects.insert(std::make_pair(framebuffer, std::make_pair(handle, newDirtyCount)));
        return std::make_pair(handle, true);
    }

    // Get the existing handle.
    auto &handleDirtyPair = it->second;
    bool changed = handleDirtyPair.second != newDirtyCount;
    handleDirtyPair.second = newDirtyCount;
    return std::make_pair(handleDirtyPair.first, changed);
}

std::pair<GLuint, bool> OpenGLContext::getVertexArrayObject(agpu_vertex_binding *vertexBinding, int newDirtyCount)
{
    std::unique_lock<std::mutex> l(clientResourceMutex);
    assert(isCurrent());

    // Find an existing vertex array object.
    auto it = vertexArrayObjects.find(vertexBinding);
    if(it == vertexArrayObjects.end())
    {
        // Create the new framebuffer object.
        GLuint handle;
        device->glGenVertexArrays(1, &handle);
        vertexArrayObjects.insert(std::make_pair(vertexBinding, std::make_pair(handle, newDirtyCount)));
        return std::make_pair(handle, true);
    }

    // Get the existing handle.
    auto handleDirtyPair = it->second;
    bool changed = handleDirtyPair.second != newDirtyCount;
    handleDirtyPair.second = newDirtyCount;
    return std::make_pair(handleDirtyPair.first, changed);
}

bool OpenGLContext::isCurrent() const
{
    return getCurrent() == this;
}

void OpenGLContext::framebufferDeleted(agpu_framebuffer *framebuffer)
{
    std::unique_lock<std::mutex> l(clientResourceMutex);

    auto it = framebufferObjects.find(framebuffer);
    if(it == framebufferObjects.end())
        return;

    if(isCurrent())
    {
        auto handle = it->second.first;
        device->glDeleteFramebuffers(1, &handle);
        framebufferObjects.erase(it);
    }
    else
    {
        framebufferCleanQueue.push_back(framebuffer);
        waitResourceCleanup(l);
    }
}

void OpenGLContext::vertexBindingDeleted(agpu_vertex_binding *vertexBinding)
{
    std::unique_lock<std::mutex> l(clientResourceMutex);

    auto it = vertexArrayObjects.find(vertexBinding);
    if(it == vertexArrayObjects.end())
        return;

    if(isCurrent())
    {

        auto handle = it->second.first;
        device->glDeleteVertexArrays(1, &handle);
        vertexArrayObjects.erase(it);
    }
    else
    {
        vaoCleanQueue.push_back(vertexBinding);
        waitResourceCleanup(l);
    }
}

void OpenGLContext::cleanClientResources()
{
    std::unique_lock<std::mutex> l(clientResourceMutex);

    // Clean FBO.
    for(auto &fb : framebufferCleanQueue)
    {
        auto it = framebufferObjects.find(fb);
        if(it == framebufferObjects.end())
            continue;

        auto handle = it->second.first;
        device->glDeleteFramebuffers(1, &handle);
        framebufferObjects.erase(it);
    }

    // Clean VAO.
    for(auto &vb : vaoCleanQueue)
    {
        auto it = vertexArrayObjects.find(vb);
        if(it == vertexArrayObjects.end())
            continue;

        auto handle = it->second.first;
        device->glDeleteVertexArrays(1, &handle);
        vertexArrayObjects.erase(it);
    }

    framebufferCleanQueue.clear();
    vaoCleanQueue.clear();
    ++resourceCleanCount;
}

void OpenGLContext::waitResourceCleanup(std::unique_lock<std::mutex> &lock)
{
    auto oldCleanCount = resourceCleanCount;

    if(ownerWaitCondition)
        ownerWaitCondition->notify_all();

    while(resourceCleanCount != oldCleanCount && (!framebufferCleanQueue.empty() || !vaoCleanQueue.empty()))
        clientResourceCleanMutex.wait(lock);
}

void OpenGLContext::finish()
{
    auto fence = device->glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glFlush();
    device->glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, -1);
    device->glDeleteSync(fence);
}

_agpu_device:: _agpu_device()
{
}

// Helper to check for extension string presence.  Adapted from:
//   http://www.opengl.org/resources/features/OGLextensions/
bool _agpu_device::isExtensionSupported(const char *extList, const char *extension)
{
    const char *start;
    const char *where, *terminator;

    /* Extension names should not have spaces. */
    where = strchr(extension, ' ');
    if (where || *extension == '\0')
        return false;

    /* It takes a bit of care to be fool-proof about parsing the
     OpenGL extensions string. Don't be fooled by sub-strings,
     etc. */
    for (start=extList;;) {
        where = strstr(start, extension);

        if (!where)
            break;

        terminator = where + strlen(extension);

        if ( where == start || *(where - 1) == ' ' )
        if ( *terminator == ' ' || *terminator == '\0' )
            return true;

        start = terminator;
    }

    return false;
}

void _agpu_device::lostReferences()
{
    defaultCommandQueue->release();

    if(mainContext)
    {
        onMainContextBlocking([&]() {
            mainContext->destroy();
            delete mainContext;
            mainContext = nullptr;
        });
    }

    mainContextJobQueue.shutdown();
}

void _agpu_device::readVersionInformation()
{
    int majorVersion, minorVersion;
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
    versionNumber = OpenGLVersion(majorVersion*10 + minorVersion);
    printf("OpenGL version %s\n", glGetString(GL_VERSION));
}

void _agpu_device::loadExtensions()
{
    // Vertex buffer object.
    LOAD_FUNCTION(glGenBuffers);
    LOAD_FUNCTION(glDeleteBuffers);
    LOAD_FUNCTION(glBindBuffer);
    LOAD_FUNCTION(glBufferData);
    LOAD_FUNCTION(glGetBufferSubData);
    LOAD_FUNCTION(glBufferSubData);
    LOAD_FUNCTION(glMapBuffer);
    LOAD_FUNCTION(glUnmapBuffer);
    LOAD_FUNCTION(glBufferStorage);

    // Buffer binding
    LOAD_FUNCTION(glBindBufferRange);
    LOAD_FUNCTION(glBindBufferBase);

    // Vertex array object.
    LOAD_FUNCTION(glGenVertexArrays);
    LOAD_FUNCTION(glDeleteVertexArrays);
    LOAD_FUNCTION(glBindVertexArray);

    // Instancing.
    LOAD_FUNCTION(glDrawArraysInstancedBaseInstance);
    LOAD_FUNCTION(glDrawElementsInstancedBaseVertexBaseInstance);

    // Indirect drawing
    LOAD_FUNCTION(glDrawElementsIndirect);
    LOAD_FUNCTION(glMultiDrawElementsIndirect);

    // Shader
    LOAD_FUNCTION(glCreateShader);
    LOAD_FUNCTION(glDeleteShader);
    LOAD_FUNCTION(glShaderSource);
    LOAD_FUNCTION(glCompileShader);
    LOAD_FUNCTION(glGetShaderSource);
    LOAD_FUNCTION(glGetShaderiv);
    LOAD_FUNCTION(glGetShaderInfoLog);
    LOAD_FUNCTION(glIsShader);

    // Program
    LOAD_FUNCTION(glCreateProgram);
    LOAD_FUNCTION(glDeleteProgram);
    LOAD_FUNCTION(glAttachShader);
    LOAD_FUNCTION(glDetachShader);
    LOAD_FUNCTION(glBindAttribLocation);
    LOAD_FUNCTION(glLinkProgram);

    LOAD_FUNCTION(glUseProgram);
    LOAD_FUNCTION(glIsProgram);
    LOAD_FUNCTION(glValidateProgram);

    LOAD_FUNCTION(glGetProgramiv);
    LOAD_FUNCTION(glGetProgramInfoLog);

    LOAD_FUNCTION(glGetActiveAttrib);
    LOAD_FUNCTION(glGetActiveUniform);

    LOAD_FUNCTION(glVertexAttribPointer);
    LOAD_FUNCTION(glDisableVertexAttribArray);
    LOAD_FUNCTION(glEnableVertexAttribArray);
    LOAD_FUNCTION(glGetAttribLocation);
    LOAD_FUNCTION(glGetUniformLocation);

    // Framebuffer object
    LOAD_FUNCTION(glBindFramebuffer);
    LOAD_FUNCTION(glDeleteFramebuffers);
    LOAD_FUNCTION(glGenFramebuffers);
    LOAD_FUNCTION(glCheckFramebufferStatus);
    LOAD_FUNCTION(glFramebufferTexture2D);
    LOAD_FUNCTION(glBlitFramebuffer);

    // Texture storage
    LOAD_FUNCTION(glTexStorage1D);
    LOAD_FUNCTION(glTexStorage2D);
    LOAD_FUNCTION(glTexStorage3D);

    // Texture functions
    LOAD_FUNCTION(glTexSubImage3D);

    // Depth range
    LOAD_FUNCTION(glDepthRangedNV);

    // Synchronization objects
    LOAD_FUNCTION(glDeleteSync);
    LOAD_FUNCTION(glFenceSync);
    LOAD_FUNCTION(glClientWaitSync);
    LOAD_FUNCTION(glWaitSync);
}

void _agpu_device::initializeObjects()
{
    readVersionInformation();
    loadExtensions();
    createDefaultCommandQueue();
}

void _agpu_device::createDefaultCommandQueue()
{
    defaultCommandQueue = agpu_command_queue::create(this);
}

AGPU_EXPORT agpu_error agpuAddDeviceReference ( agpu_device *device )
{
    CHECK_POINTER(device);
    return device->retain();
}

AGPU_EXPORT agpu_error agpuReleaseDevice ( agpu_device *device )
{
    CHECK_POINTER(device);
    return device->release();
}

AGPU_EXPORT agpu_buffer* agpuCreateBuffer ( agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data )
{
    if (!device)
        return nullptr;
    return agpu_buffer::createBuffer(device, *description, initial_data);
}

AGPU_EXPORT agpu_vertex_layout* agpuCreateVertexLayout(agpu_device* device)
{
    if (!device)
        return nullptr;
    return agpu_vertex_layout::createVertexLayout(device);
}

AGPU_EXPORT agpu_vertex_binding* agpuCreateVertexBinding(agpu_device* device, agpu_vertex_layout* layout)
{
    if (!device)
        return nullptr;
    return agpu_vertex_binding::createVertexBinding(device, layout);
}

AGPU_EXPORT agpu_shader* agpuCreateShader ( agpu_device* device, agpu_shader_type type )
{
    if (!device)
        return nullptr;
    return agpu_shader::createShader(device, type);
}

AGPU_EXPORT agpu_pipeline_builder* agpuCreatePipelineBuilder ( agpu_device* device )
{
    if (!device)
        return nullptr;
    return agpu_pipeline_builder::createBuilder(device);
}

AGPU_EXPORT agpu_command_allocator* agpuCreateCommandAllocator ( agpu_device* device )
{
    if(!device)
        return nullptr;
    return agpu_command_allocator::create(device);
}

AGPU_EXPORT agpu_command_list* agpuCreateCommandList ( agpu_device* device, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
{
    if (!device)
        return nullptr;
    return agpu_command_list::create(device, allocator, initial_pipeline_state, false);
}

AGPU_EXPORT agpu_command_list* agpuCreateCommandListBundle ( agpu_device* device, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
{
    if (!device)
        return nullptr;
    return agpu_command_list::create(device, allocator, initial_pipeline_state, true);
}

AGPU_EXPORT agpu_command_queue* agpuGetDefaultCommandQueue(agpu_device* device)
{
    if (!device)
        return nullptr;
    return device->defaultCommandQueue;
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredShaderLanguage(agpu_device* device)
{
    return AGPU_SHADER_LANGUAGE_GLSL;
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredHighLevelShaderLanguage(agpu_device* device)
{
    return AGPU_SHADER_LANGUAGE_GLSL;
}

AGPU_EXPORT agpu_shader_resource_binding* agpuCreateShaderResourceBinding ( agpu_device* device, agpu_int bindingBank )
{
    if(!device)
        return nullptr;
    return agpu_shader_resource_binding::create(device, bindingBank);
}

AGPU_EXPORT agpu_swap_chain* agpuCreateSwapChain ( agpu_device* device, agpu_swap_chain_create_info* swapChainInfo )
{
    if(!device)
        return nullptr;
    return agpu_swap_chain::create(device, swapChainInfo);
}

AGPU_EXPORT agpu_framebuffer* agpuCreateFrameBuffer ( agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint renderTargetCount, agpu_bool hasDepth, agpu_bool hasStencil )
{
    if(!device)
        return nullptr;

    return agpu_framebuffer::create(device, width, height, renderTargetCount, hasDepth, hasStencil);
}

AGPU_EXPORT agpu_texture* agpuCreateTexture ( agpu_device* device, agpu_texture_description* description, agpu_pointer initialData )
{
    if(!device)
        return nullptr;

    return agpu_texture::create(device, description, initialData);
}
