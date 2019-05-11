#ifndef _AGPU_DEVICE_HPP_
#define _AGPU_DEVICE_HPP_

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <GL/GL.h>
#include <GL/glext.h>

typedef HGLRC(WINAPI * wglCreateContextAttribsARBProc) (HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL(WINAPI * wglChoosePixelFormatARBProc) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

#else
#error unsupported platform
#endif

#include <string>
#include <map>
#include <list>

#include "common.hpp"
#include "job_queue.hpp"

namespace AgpuGL
{

class AgpuGLImmediateContext;

/**
 * OpenGL version number
 */
enum class OpenGLVersion
{
    Invalid = -1,
    Version10 = 10,
    Version20 = 20,
    Version21 = 21,
    Version30 = 30,
    Version31 = 31,
    Version32 = 32,
    Version33 = 33,
    Version40 = 40,
    Version41 = 41,
    Version42 = 42,
    Version43 = 43,
};

extern OpenGLVersion GLContextVersionPriorities[];

struct OpenGLContext
{
    OpenGLContext();
    ~OpenGLContext();

    bool makeCurrentWithWindow(agpu_pointer window);
    bool makeCurrent();
    void swapBuffers();
    void swapBuffersOfWindow(agpu_pointer window);
    void destroy();
    void finish();

    static OpenGLContext *getCurrent();

    agpu::device_weakref weakDevice;

    bool ownsWindow;
    bool ownsDisplay;
    OpenGLVersion version;

#ifdef _WIN32
    wglCreateContextAttribsARBProc wglCreateContextAttribsARB;
    wglChoosePixelFormatARBProc wglChoosePixelFormatARB;

    HWND window;
    HDC hDC;
    HGLRC context;

#elif defined(__linux__)
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB;
    GLXFBConfig framebufferConfig;

    Display *display;
    Window window;
    GLXContext context;
#endif

    bool isCurrent() const;
};

/**
 * Agpu OpenGL device
 */
struct GLDevice: public agpu::device
{
public:
    GLDevice();
    ~GLDevice();

    static agpu::device_ref open(agpu_device_open_info* openInfo);
    static bool isExtensionSupported(const char *extList, const char *extension);
    bool hasOpenGLExtension(const char *extension);

    void setWindowPixelFormat(agpu_pointer window);

    void readVersionInformation();
	void checkEnvironmentVariables();
    void loadExtensions();
    void *getProcAddress(const char *symbolName);
    void initializeObjects();
    void createDefaultCommandQueue();

    template<typename FT>
    void loadExtensionFunction(FT &functionPointer, const char *functionName)
    {
        functionPointer = reinterpret_cast<FT> (getProcAddress(functionName));
    }

    template<typename FT>
    void onMainContextBlocking(const FT &f)
    {
        AsyncJob job(f);
        mainContextJobQueue.addJob(&job);
        job.wait();
    }

public:
    virtual agpu::command_queue_ptr getDefaultCommandQueue() override;
	virtual agpu::swap_chain_ptr createSwapChain(const agpu::command_queue_ref & commandQueue, agpu_swap_chain_create_info* swapChainInfo) override;
	virtual agpu::buffer_ptr createBuffer(agpu_buffer_description* description, agpu_pointer initial_data) override;
	virtual agpu::vertex_layout_ptr createVertexLayout() override;
	virtual agpu::vertex_binding_ptr createVertexBinding(const agpu::vertex_layout_ref & layout) override;
	virtual agpu::shader_ptr createShader(agpu_shader_type type) override;
    virtual agpu::offline_shader_compiler_ptr createOfflineShaderCompiler() override;
	virtual agpu::shader_signature_builder_ptr createShaderSignatureBuilder() override;
	virtual agpu::pipeline_builder_ptr createPipelineBuilder() override;
	virtual agpu::compute_pipeline_builder_ptr createComputePipelineBuilder() override;
	virtual agpu::command_allocator_ptr createCommandAllocator(agpu_command_list_type type, const agpu::command_queue_ref & queue) override;
	virtual agpu::command_list_ptr createCommandList(agpu_command_list_type type, const agpu::command_allocator_ref & allocator, const agpu::pipeline_state_ref & initial_pipeline_state) override;
	virtual agpu_shader_language getPreferredShaderLanguage() override;
	virtual agpu_shader_language getPreferredIntermediateShaderLanguage() override;
	virtual agpu_shader_language getPreferredHighLevelShaderLanguage() override;
	virtual agpu::framebuffer_ptr createFrameBuffer(agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView) override;
	virtual agpu::renderpass_ptr createRenderPass(agpu_renderpass_description* description) override;
	virtual agpu::texture_ptr createTexture(agpu_texture_description* description) override;
	virtual agpu::fence_ptr createFence() override;

	virtual agpu_int getMultiSampleQualityLevels(agpu_uint sample_count) override;
	virtual agpu_bool hasTopLeftNdcOrigin() override;
	virtual agpu_bool hasBottomLeftTextureCoordinates() override;
	virtual agpu_bool isFeatureSupported(agpu_feature feature) override;

	virtual agpu::vr_system_ptr getVRSystem() override;

public:
    OpenGLVersion versionNumber;
    int glslVersionNumber;
    std::string rendererString, shaderString;
    std::string extensions;

    agpu::command_queue_ref defaultCommandQueue;

	// Debugging options.
	bool dumpShaders;
	bool dumpShadersOnError;

    // Important extensions
    bool isPersistentMemoryMappingSupported_;
    bool isCoherentMemoryMappingSupported_;
    bool hasExtension_GL_NV_depth_buffer_float;
    bool hasExtension_GL_ARB_clip_control;

    // OpenGL API
    OpenGLContext *mainContext;
    JobQueue mainContextJobQueue;

    // GetStringi
    PFNGLGETSTRINGIPROC glGetStringi;

    // Vertex buffer object
    PFNGLGENBUFFERSPROC glGenBuffers;
    PFNGLDELETEBUFFERSPROC glDeleteBuffers;
    PFNGLBINDBUFFERPROC glBindBuffer;
    PFNGLBUFFERDATAPROC glBufferData;
    PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
    PFNGLBUFFERSUBDATAPROC glBufferSubData;
    PFNGLMAPBUFFERPROC glMapBuffer;
    PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
    PFNGLUNMAPBUFFERPROC glUnmapBuffer;
    PFNGLBUFFERSTORAGEPROC glBufferStorage;

    // Buffer binding
    PFNGLBINDBUFFERRANGEPROC glBindBufferRange;
    PFNGLBINDBUFFERBASEPROC glBindBufferBase;

    // Vertex array object
    PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
    PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
    PFNGLBINDVERTEXARRAYPROC glBindVertexArray;

    // Instancing
    PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC glDrawArraysInstancedBaseInstance;
    PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glDrawElementsInstancedBaseVertexBaseInstance;

    // Indirect drawing.
    PFNGLDRAWARRAYSINDIRECTPROC glDrawArraysIndirect;
    PFNGLMULTIDRAWARRAYSINDIRECTPROC glMultiDrawArraysIndirect;
    PFNGLDRAWELEMENTSINDIRECTPROC glDrawElementsIndirect;
    PFNGLMULTIDRAWELEMENTSINDIRECTPROC glMultiDrawElementsIndirect;

    // Shader
    PFNGLCREATESHADERPROC glCreateShader;
    PFNGLDELETESHADERPROC glDeleteShader;
    PFNGLSHADERSOURCEPROC glShaderSource;
    PFNGLCOMPILESHADERPROC glCompileShader;
    PFNGLGETSHADERSOURCEPROC glGetShaderSource;
    PFNGLGETSHADERIVPROC glGetShaderiv;
    PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
    PFNGLISSHADERPROC glIsShader;

    // Compute shader
    PFNGLDISPATCHCOMPUTEPROC glDispatchCompute;
    PFNGLDISPATCHCOMPUTEINDIRECTPROC glDispatchComputeIndirect;

    // Program
    PFNGLCREATEPROGRAMPROC glCreateProgram;
    PFNGLDELETEPROGRAMPROC glDeleteProgram;
    PFNGLATTACHSHADERPROC glAttachShader;
    PFNGLDETACHSHADERPROC glDetachShader;
    PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
    PFNGLLINKPROGRAMPROC glLinkProgram;

    PFNGLUSEPROGRAMPROC glUseProgram;
    PFNGLISPROGRAMPROC glIsProgram;
    PFNGLVALIDATEPROGRAMPROC glValidateProgram;

    PFNGLGETPROGRAMIVPROC glGetProgramiv;
    PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;

    PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;
    PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;

    PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
    PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
    PFNGLVERTEXATTRIBLPOINTERPROC glVertexAttribLPointer;
    PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
    PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
    PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
    PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
    PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;
    PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;
    PFNGLUNIFORM1IPROC glUniform1i;

    // Framebuffer object
    PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
    PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
    PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
    PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
    PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
    PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer;
    PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;

    // Texture storage.
    PFNGLTEXSTORAGE1DPROC glTexStorage1D;
    PFNGLTEXSTORAGE2DPROC glTexStorage2D;
    PFNGLTEXSTORAGE2DMULTISAMPLEPROC glTexStorage2DMultisample;
    PFNGLTEXSTORAGE3DPROC glTexStorage3D;

    // Texture
    PFNGLACTIVETEXTUREPROC glActiveTexture;
    PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
    PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D;
    PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;
    PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;

    // Sampler
    PFNGLGENSAMPLERSPROC glGenSamplers;
    PFNGLDELETESAMPLERSPROC glDeleteSamplers;
    PFNGLBINDSAMPLERPROC glBindSampler;
    PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri;
    PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf;

    // Depth range
    PFNGLDEPTHRANGEDNVPROC glDepthRangedNV;

    // Synchronization
    PFNGLDELETESYNCPROC glDeleteSync;
    PFNGLFENCESYNCPROC glFenceSync;
    PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
    PFNGLWAITSYNCPROC glWaitSync;

    // Separate stencil
    PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;
    PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate;

    // Separate blending
    PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
    PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;

    // Clip control
    PFNGLCLIPCONTROLPROC glClipControl;

    // Memory barrier
    PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange;
    PFNGLMEMORYBARRIERPROC glMemoryBarrier;
};

} // End of namespace AgpuGL

#endif //_AGPU_DEVICE_HPP_
