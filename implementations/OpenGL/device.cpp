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

#define LOAD_FUNCTION(functionName) loadExtensionFunction(functionName, #functionName)

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
    
    // Depth range
    LOAD_FUNCTION(glDepthRangedNV);
}

void _agpu_device::initializeObjects()
{
    readVersionInformation();
    loadExtensions();
    createDefaultCommandQueue();
    createMainFrameBuffer();
}

void _agpu_device::createDefaultCommandQueue()
{
    defaultCommandQueue = agpu_command_queue::create(this);
}

void _agpu_device::createMainFrameBuffer()
{
    // TODO: Get the actual size
    mainFrameBuffer = agpu_framebuffer::createMain(this, 640, 480, 1, true, true);
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

AGPU_EXPORT agpu_error agpuSwapBuffers ( agpu_device* device )
{
    CHECK_POINTER(device);
    return device->swapBuffers();
}

AGPU_EXPORT agpu_framebuffer* agpuGetCurrentBackBuffer ( agpu_device* device )
{
    if(!device)
        return nullptr;
    return device->mainFrameBuffer;
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
    return agpu_command_list::create(device, allocator, initial_pipeline_state);
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
