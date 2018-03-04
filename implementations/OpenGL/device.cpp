#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "device.hpp"
#include "buffer.hpp"
#include "shader.hpp"
#include "shader_signature_builder.hpp"
#include "pipeline_builder.hpp"
#include "command_allocator.hpp"
#include "command_list.hpp"
#include "command_queue.hpp"
#include "vertex_binding.hpp"
#include "vertex_layout.hpp"
#include "framebuffer.hpp"
#include "renderpass.hpp"
#include "swap_chain.hpp"
#include "texture.hpp"
#include "fence.hpp"

#define LOAD_FUNCTION(functionName) loadExtensionFunction(functionName, #functionName)

void printMessage(const char *format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 1024, format, args);
#ifdef _WIN32
    OutputDebugStringA(buffer);
#else
    fputs(buffer, stdout);
#endif
    va_end(args);
}

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

bool OpenGLContext::isCurrent() const
{
    return getCurrent() == this;
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

bool _agpu_device::hasOpenGLExtension(const char *extension)
{
    return isExtensionSupported(extensions.c_str(), extension);
}

void _agpu_device::readVersionInformation()
{
    int majorVersion, minorVersion;
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
    versionNumber = OpenGLVersion(majorVersion*10 + minorVersion);

    if(majorVersion >= 3)
    {
        int numberOfExtensions;
        LOAD_FUNCTION(glGetStringi);

        glGetIntegerv(GL_NUM_EXTENSIONS, &numberOfExtensions);
        for(int i = 0; i < numberOfExtensions; ++i)
        {
            if(i > 0)
                extensions += " ";
            extensions += (const char*)glGetStringi(GL_EXTENSIONS, i);
        }
    }
    else
    {
        extensions = (const char*)glGetString(GL_EXTENSIONS);
    }

    printMessage("OpenGL version %s\n", glGetString(GL_VERSION));
    printMessage("OpenGL vendor %s\n", glGetString(GL_VENDOR));
    printMessage("GLSL version %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    {
        int glslMajor;
        int glslMinor;
        sscanf((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION), "%d.%d", &glslMajor, &glslMinor);
        glslVersionNumber = glslMajor*100 + glslMinor;
    }
}

agpu_int _agpu_device::getMultiSampleQualityLevels(agpu_uint sample_count)
{
    return 1;
}

void _agpu_device::loadExtensions()
{
    isPersistentMemoryMappingSupported_ = false;
    isCoherentMemoryMappingSupported_ = false;

    // Vertex buffer object.
    LOAD_FUNCTION(glGenBuffers);
    LOAD_FUNCTION(glDeleteBuffers);
    LOAD_FUNCTION(glBindBuffer);
    LOAD_FUNCTION(glBufferData);
    LOAD_FUNCTION(glGetBufferSubData);
    LOAD_FUNCTION(glBufferSubData);
    LOAD_FUNCTION(glMapBuffer);
    LOAD_FUNCTION(glMapBufferRange);
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
    LOAD_FUNCTION(glGetUniformBlockIndex);
    LOAD_FUNCTION(glUniformBlockBinding);
    LOAD_FUNCTION(glUniform1i);

    // Framebuffer object
    LOAD_FUNCTION(glBindFramebuffer);
    LOAD_FUNCTION(glDeleteFramebuffers);
    LOAD_FUNCTION(glGenFramebuffers);
    LOAD_FUNCTION(glCheckFramebufferStatus);
    LOAD_FUNCTION(glFramebufferTexture2D);
    LOAD_FUNCTION(glFramebufferTextureLayer);
    LOAD_FUNCTION(glBlitFramebuffer);

    // Texture storage
    LOAD_FUNCTION(glTexStorage1D);
    LOAD_FUNCTION(glTexStorage2D);
    LOAD_FUNCTION(glTexStorage3D);

    // Texture functions
    LOAD_FUNCTION(glActiveTexture);
    LOAD_FUNCTION(glTexSubImage3D);
    LOAD_FUNCTION(glCompressedTexSubImage1D);
    LOAD_FUNCTION(glCompressedTexSubImage2D);
    LOAD_FUNCTION(glCompressedTexSubImage3D);

    // Samplers
    LOAD_FUNCTION(glGenSamplers);
    LOAD_FUNCTION(glDeleteSamplers);
    LOAD_FUNCTION(glBindSampler);
    LOAD_FUNCTION(glSamplerParameteri);
    LOAD_FUNCTION(glSamplerParameterf);

    // Depth range
    LOAD_FUNCTION(glDepthRangedNV);

    // Synchronization objects
    LOAD_FUNCTION(glDeleteSync);
    LOAD_FUNCTION(glFenceSync);
    LOAD_FUNCTION(glClientWaitSync);
    LOAD_FUNCTION(glWaitSync);

    // Stencil buffer
    LOAD_FUNCTION(glStencilFuncSeparate);
    LOAD_FUNCTION(glStencilOpSeparate);

    // Stencil buffer
    LOAD_FUNCTION(glBlendFuncSeparate);
    LOAD_FUNCTION(glBlendEquationSeparate);

    // Clip control
    LOAD_FUNCTION(glClipControl);

    isPersistentMemoryMappingSupported_ = isCoherentMemoryMappingSupported_ = glBufferStorage != nullptr && hasOpenGLExtension("GL_ARB_buffer_storage");
    hasExtension_GL_NV_depth_buffer_float = glDepthRangedNV != nullptr && hasOpenGLExtension("GL_NV_depth_buffer_float");
    hasExtension_GL_ARB_clip_control = glClipControl != nullptr && hasOpenGLExtension("GL_ARB_clip_control");

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

agpu_bool _agpu_device::isFeatureSupported (agpu_feature feature)
{
    switch(feature)
    {
    case AGPU_FEATURE_PERSISTENT_MEMORY_MAPPING: return isPersistentMemoryMappingSupported_;
    case AGPU_FEATURE_COHERENT_MEMORY_MAPPING: return isCoherentMemoryMappingSupported_;
    case AGPU_FEATURE_PERSISTENT_COHERENT_MEMORY_MAPPING: return isPersistentMemoryMappingSupported_ && isCoherentMemoryMappingSupported_;
    case AGPU_FEATURE_COMMAND_LIST_REUSE: return true;
    case AGPU_FEATURE_NON_EMULATED_COMMAND_LIST_REUSE: return false;
    default: return false;
    }
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

AGPU_EXPORT agpu_shader_signature_builder* agpuCreateShaderSignatureBuilder(agpu_device* device)
{
    if (!device)
        return nullptr;
    return agpu_shader_signature_builder::create(device);
}

AGPU_EXPORT agpu_pipeline_builder* agpuCreatePipelineBuilder ( agpu_device* device )
{
    if (!device)
        return nullptr;
    return agpu_pipeline_builder::createBuilder(device);
}

AGPU_EXPORT agpu_command_allocator* agpuCreateCommandAllocator ( agpu_device* device, agpu_command_list_type type, agpu_command_queue *queue)
{
    if(!device)
        return nullptr;
    return agpu_command_allocator::create(device, type, queue);
}

AGPU_EXPORT agpu_command_list* agpuCreateCommandList ( agpu_device* device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
{
    if (!device)
        return nullptr;
    return agpu_command_list::create(device, type, allocator, initial_pipeline_state);
}

AGPU_EXPORT agpu_command_queue* agpuGetDefaultCommandQueue(agpu_device* device)
{
    if (!device)
        return nullptr;

    device->defaultCommandQueue->retain();
    return device->defaultCommandQueue;
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredShaderLanguage(agpu_device* device)
{
    return AGPU_SHADER_LANGUAGE_GLSL;
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredIntermediateShaderLanguage(agpu_device* device)
{
    return AGPU_SHADER_LANGUAGE_SPIR_V;
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredHighLevelShaderLanguage(agpu_device* device)
{
    return AGPU_SHADER_LANGUAGE_NONE;
}

AGPU_EXPORT agpu_swap_chain* agpuCreateSwapChain ( agpu_device* device, agpu_command_queue* commandQueue, agpu_swap_chain_create_info* swapChainInfo )
{
    if(!device)
        return nullptr;
    return agpu_swap_chain::create(device, commandQueue, swapChainInfo);
}

AGPU_EXPORT agpu_framebuffer* agpuCreateFrameBuffer(agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorView, agpu_texture_view_description* depthStencilViews)
{
    if(!device)
        return nullptr;

    return agpu_framebuffer::create(device, width, height, colorCount, colorView, depthStencilViews);
}

AGPU_EXPORT agpu_renderpass* agpuCreateRenderPass(agpu_device* device, agpu_renderpass_description* description)
{
    if (!device)
        return nullptr;

    return agpu_renderpass::create(device, description);
}

AGPU_EXPORT agpu_texture* agpuCreateTexture ( agpu_device* device, agpu_texture_description* description )
{
    if(!device)
        return nullptr;

    return agpu_texture::create(device, description);
}

AGPU_EXPORT agpu_fence* agpuCreateFence ( agpu_device* device )
{
    if(!device)
        return nullptr;

    return agpu_fence::create(device);
}

AGPU_EXPORT agpu_int agpuGetMultiSampleQualityLevels(agpu_device* device, agpu_uint sample_count)
{
    if (!device)
        return 0;
    return device->getMultiSampleQualityLevels(sample_count);
}

AGPU_EXPORT agpu_bool agpuHasTopLeftNdcOrigin(agpu_device *device)
{
    return false;
}

AGPU_EXPORT agpu_bool agpuIsFeatureSupportedOnDevice ( agpu_device* device, agpu_feature feature )
{
    if(!device)
        return false;
    return device->isFeatureSupported(feature);
}
