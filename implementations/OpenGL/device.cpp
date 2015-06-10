#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "device.hpp"
#include "context.hpp"
#include "buffer.hpp"
#include "shader.hpp"
#include "program.hpp"

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
    immediateContext = new agpu_context(new AgpuGLImmediateContext(this));
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

agpu_context* _agpu_device::getImmediateContext()
{
    immediateContext->retain();
    return immediateContext;
}

agpu_context* _agpu_device::createDeferredContext()
{
    return nullptr;
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
    
    LOAD_FUNCTION(glDisableVertexAttribArray);
    LOAD_FUNCTION(glEnableVertexAttribArray);
    LOAD_FUNCTION(glGetAttribLocation);
    LOAD_FUNCTION(glGetUniformLocation);
}

AGPU_EXPORT agpu_error agpuAddDeviceReference ( agpu_device *device )
{
    return device->retain();
}

AGPU_EXPORT agpu_error agpuReleaseDevice ( agpu_device *device )
{
    return device->release();
}

AGPU_EXPORT agpu_context* agpuGetImmediateContext ( agpu_device* device )
{
    return device->getImmediateContext();
}

AGPU_EXPORT agpu_context* agpuCreateDeferredContext ( agpu_device* device )
{
    return device->createDeferredContext();
}

AGPU_EXPORT agpu_error agpuSwapBuffers ( agpu_device* device )
{
    return device->swapBuffers();
}

AGPU_EXPORT agpu_buffer* agpuCreateBuffer ( agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data )
{
    return agpu_buffer::createBuffer(device, *description, initial_data);
}

AGPU_EXPORT agpu_shader* agpuCreateShader ( agpu_device* device, agpu_shader_type type )
{
    return agpu_shader::createShader(device, type);
}

AGPU_EXPORT agpu_program* agpuCreateProgram ( agpu_device* device )
{
    return agpu_program::createProgram(device);
}