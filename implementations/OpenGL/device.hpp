#ifndef _AGPU_DEVICE_HPP_
#define _AGPU_DEVICE_HPP_

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
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

#include "object.hpp"
#include "job_queue.hpp"

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

    agpu_device *device;

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

    std::pair<GLuint, bool> getFrameBufferObject(agpu_framebuffer *framebuffer, int newDirtyCount);
    std::pair<GLuint, bool> getVertexArrayObject(agpu_vertex_binding *vertexBinding, int newDirtyCount);

    bool isCurrent() const;

    void framebufferDeleted(agpu_framebuffer *framebuffer);
    void vertexBindingDeleted(agpu_vertex_binding *vertexBinding);
    void cleanClientResources();

    template<typename LT>
    void waitCondition(std::condition_variable &waitCondition, LT &lock)
    {
        // Store the wait condition.
        {
            std::unique_lock<std::mutex> l2(clientResourceMutex);
            ownerWaitCondition = &waitCondition;
        }

        // Wait the condition.
        waitCondition.wait(lock);
        cleanClientResources();

        // Unstore the wait condition.
        {
            std::unique_lock<std::mutex> l2(clientResourceMutex);
            ownerWaitCondition = nullptr;
        }
    }

private:
    void waitResourceCleanup(std::unique_lock<std::mutex> &lock);

    std::map<agpu_framebuffer*, std::pair<GLuint, int> > framebufferObjects;
    std::map<agpu_vertex_binding*, std::pair<GLuint, int> > vertexArrayObjects;
    std::vector<agpu_framebuffer*> framebufferCleanQueue;
    std::vector<agpu_vertex_binding*> vaoCleanQueue;

    std::mutex clientResourceMutex;
    std::condition_variable clientResourceCleanMutex;
    int resourceCleanCount;
    std::condition_variable *ownerWaitCondition;
};

/**
 * Agpu OpenGL device
 */
struct _agpu_device: public Object<_agpu_device>
{
public:
    _agpu_device();

    void lostReferences();

    static bool isExtensionSupported(const char *extList, const char *extension);
    static agpu_device *open(agpu_device_open_info* openInfo);

    OpenGLContext* createSecondaryContext(bool useMainWindow);

    void setWindowPixelFormat(agpu_pointer window);

    void readVersionInformation();
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

    template<typename FT>
    void allContextDo(const FT &f)
    {
        std::unique_lock<std::mutex> l(allContextMutex);
        for(auto context : allContexts)
            f(context);
    }

    OpenGLVersion versionNumber;
    std::string rendererString, shaderString;

    agpu_command_queue *defaultCommandQueue;

    // OpenGL API
    OpenGLContext *mainContext;
    JobQueue mainContextJobQueue;

    std::mutex allContextMutex;
    std::list<OpenGLContext*> allContexts;

    // Vertex buffer object
    PFNGLGENBUFFERSPROC glGenBuffers;
    PFNGLDELETEBUFFERSPROC glDeleteBuffers;
    PFNGLBINDBUFFERPROC glBindBuffer;
    PFNGLBUFFERDATAPROC glBufferData;
    PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
    PFNGLBUFFERSUBDATAPROC glBufferSubData;
    PFNGLMAPBUFFERPROC glMapBuffer;
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
    PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;

    // Texture storage.
    PFNGLTEXSTORAGE1DPROC glTexStorage1D;
    PFNGLTEXSTORAGE2DPROC glTexStorage2D;
    PFNGLTEXSTORAGE3DPROC glTexStorage3D;

    // Texture
    PFNGLACTIVETEXTUREPROC glActiveTexture;
    PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;

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
};

#endif //_AGPU_DEVICE_HPP_
