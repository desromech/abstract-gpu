#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "device.hpp"
#include "buffer.hpp"
#include "shader.hpp"
#include "shader_signature_builder.hpp"
#include "compute_pipeline_builder.hpp"
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

namespace AgpuGL
{

std::string getStringFromEnvironment(const char *varname)
{
#ifdef _WIN32
	char *buffer;
	size_t size;
	auto error = _dupenv_s(&buffer, &size, varname);;
	if (error) return std::string();
	if (!buffer) return std::string();
	std::string res = buffer;
	free(buffer);
	return res;
#else
	auto value = getenv(varname);
	if (!value)
		return std::string();
	return value;
#endif
}

bool getBooleanEnvironment(const char *varname, bool defaultValue = false)
{
	auto result = getStringFromEnvironment(varname);
	if (result.empty())
		return defaultValue;
	return result != "0" && result != "n";
}


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
	auto device = weakDevice.lock();
    auto fence = deviceForGL->glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glFlush();
	GLenum waitReturn = GL_UNSIGNALED;
	while (waitReturn != GL_ALREADY_SIGNALED && waitReturn != GL_CONDITION_SATISFIED)
		waitReturn = deviceForGL->glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000);
    deviceForGL->glDeleteSync(fence);
}

GLDevice::GLDevice()
{
}

GLDevice::~GLDevice()
{
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

// Helper to check for extension string presence.  Adapted from:
//   http://www.opengl.org/resources/features/OGLextensions/
bool GLDevice::isExtensionSupported(const char *extList, const char *extension)
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

bool GLDevice::hasOpenGLExtension(const char *extension)
{
    return isExtensionSupported(extensions.c_str(), extension);
}

void GLDevice::readVersionInformation()
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
#ifdef _WIN32
		sscanf_s((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION), "%d.%d", &glslMajor, &glslMinor);
#else
        sscanf((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION), "%d.%d", &glslMajor, &glslMinor);
#endif
        glslVersionNumber = glslMajor*100 + glslMinor;
    }
}

agpu_int GLDevice::getMultiSampleQualityLevels(agpu_uint sample_count)
{
    return 1;
}


void GLDevice::checkEnvironmentVariables()
{
	dumpShaders = getBooleanEnvironment("DUMP_SHADERS", false);
	dumpShadersOnError = getBooleanEnvironment("DUMP_SHADERS_ON_ERROR", false);
}

void GLDevice::loadExtensions()
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
	LOAD_FUNCTION(glDrawArraysIndirect);
    LOAD_FUNCTION(glMultiDrawArraysIndirect);
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

	// Compute shader
	LOAD_FUNCTION(glDispatchCompute);
	LOAD_FUNCTION(glDispatchComputeIndirect);

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
	LOAD_FUNCTION(glVertexAttribIPointer);
	LOAD_FUNCTION(glVertexAttribLPointer);
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
	LOAD_FUNCTION(glTexStorage2DMultisample);
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

	// Memory barrier
	LOAD_FUNCTION(glMemoryBarrier);
	LOAD_FUNCTION(glFlushMappedBufferRange);

    isPersistentMemoryMappingSupported_ = isCoherentMemoryMappingSupported_ = glBufferStorage != nullptr && hasOpenGLExtension("GL_ARB_buffer_storage");
    hasExtension_GL_NV_depth_buffer_float = glDepthRangedNV != nullptr && hasOpenGLExtension("GL_NV_depth_buffer_float");
    hasExtension_GL_ARB_clip_control = glClipControl != nullptr && hasOpenGLExtension("GL_ARB_clip_control");

}

agpu::command_queue_ptr GLDevice::getDefaultCommandQueue()
{
	return defaultCommandQueue.disownedNewRef();
}

agpu::swap_chain_ptr GLDevice::createSwapChain(const agpu::command_queue_ref & commandQueue, agpu_swap_chain_create_info* swapChainInfo)
{
	return GLSwapChain::create(refFromThis<agpu::device> (), commandQueue, swapChainInfo).disown();
}

agpu::buffer_ptr GLDevice::createBuffer(agpu_buffer_description* description, agpu_pointer initial_data)
{
	if(!description)
		return agpu::buffer_ptr();
	return GLBuffer::createBuffer(refFromThis<agpu::device> (), *description, initial_data).disown();
}

agpu::vertex_layout_ptr GLDevice::createVertexLayout()
{
	return GLVertexLayout::createVertexLayout(refFromThis<agpu::device> ()).disown();
}

agpu::vertex_binding_ptr GLDevice::createVertexBinding(const agpu::vertex_layout_ref & layout)
{
	return GLVertexBinding::createVertexBinding(refFromThis<agpu::device> (), layout).disown();
}

agpu::shader_ptr GLDevice::createShader(agpu_shader_type type)
{
	return GLShader::createShader(refFromThis<agpu::device> (), type).disown();
}

agpu::shader_signature_builder_ptr GLDevice::createShaderSignatureBuilder()
{
	return GLShaderSignatureBuilder::create(refFromThis<agpu::device> ()).disown();
}

agpu::pipeline_builder_ptr GLDevice::createPipelineBuilder()
{
	return GLGraphicsPipelineBuilder::createBuilder(refFromThis<agpu::device> ()).disown();
}

agpu::compute_pipeline_builder_ptr GLDevice::createComputePipelineBuilder()
{
	return GLComputePipelineBuilder::create(refFromThis<agpu::device> ()).disown();
}

agpu::command_allocator_ptr GLDevice::createCommandAllocator(agpu_command_list_type type, const agpu::command_queue_ref & queue)
{
	return GLCommandAllocator::create(refFromThis<agpu::device> (), type, queue).disown();
}

agpu::command_list_ptr GLDevice::createCommandList(agpu_command_list_type type, const agpu::command_allocator_ref & allocator, const agpu::pipeline_state_ref & initial_pipeline_state)
{
	return GLCommandList::create(refFromThis<agpu::device> (), type, allocator, initial_pipeline_state).disown();
}

agpu_shader_language GLDevice::getPreferredShaderLanguage()
{
    return AGPU_SHADER_LANGUAGE_GLSL;
}

agpu_shader_language GLDevice::getPreferredIntermediateShaderLanguage()
{
    return AGPU_SHADER_LANGUAGE_SPIR_V;
}

agpu_shader_language GLDevice::getPreferredHighLevelShaderLanguage()
{
    return AGPU_SHADER_LANGUAGE_NONE;
}

agpu::framebuffer_ptr GLDevice::createFrameBuffer(agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView)
{
	return GLFramebuffer::create(refFromThis<agpu::device> (), width, height, colorCount, colorViews, depthStencilView).disown();
}

agpu::renderpass_ptr GLDevice::createRenderPass(agpu_renderpass_description* description)
{
	return GLRenderPass::create(refFromThis<agpu::device> (), description).disown();
}

agpu::texture_ptr GLDevice::createTexture(agpu_texture_description* description)
{
	return GLTexture::create(refFromThis<agpu::device> (), description).disown();
}

agpu::fence_ptr GLDevice::createFence()
{
	return GLFence::create(refFromThis<agpu::device> ()).disown();
}

agpu_bool GLDevice::hasBottomLeftTextureCoordinates()
{
    return true;
}

agpu_bool GLDevice::hasTopLeftNdcOrigin()
{
    return false;//hasExtension_GL_ARB_clip_control;
}

void GLDevice::initializeObjects()
{
    readVersionInformation();
	checkEnvironmentVariables();
    loadExtensions();
    createDefaultCommandQueue();
}

void GLDevice::createDefaultCommandQueue()
{
    defaultCommandQueue = GLCommandQueue::create(refFromThis<agpu::device> ());
}

agpu_bool GLDevice::isFeatureSupported (agpu_feature feature)
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

agpu::vr_system_ptr GLDevice::getVRSystem()
{
	return nullptr;
}

} // End of namespace AgpuGL
