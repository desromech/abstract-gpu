#include <stdio.h>
#include <vector>
#include <memory>
#include "SampleBase.hpp"
#include "SDL_syswm.h"

agpu_vertex_attrib_description SampleVertex::Description[] = {
    {0, 0, AGPU_FLOAT, 3, 1, false, offsetof(SampleVertex, position)},
    {0, 1, AGPU_FLOAT, 4, 1, false, offsetof(SampleVertex, color)},
    {0, 2, AGPU_FLOAT, 3, 1, false, offsetof(SampleVertex, normal)},
    {0, 3, AGPU_FLOAT, 2, 1, false, offsetof(SampleVertex, texcoord)},
};

const int SampleVertex::DescriptionSize = 4;

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

std::string readWholeFile(const std::string &fileName)
{
    FILE *file = fopen(fileName.c_str(), "rb");
    if(!file)
    {
        printError("Failed to open file %s\n", fileName.c_str());
        return std::string();
    }

    // Allocate the data.
    std::vector<char> data;
    fseek(file, 0, SEEK_END);
    data.resize(ftell(file));
    fseek(file, 0, SEEK_SET);

    // Read the file
    if(fread(&data[0], data.size(), 1, file) != 1)
    {
        printError("Failed to read file %s\n", fileName.c_str());
        fclose(file);
        return std::string();
    }

    fclose(file);
    return std::string(data.begin(), data.end());
}

int SampleBase::main(int argc, const char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);

    screenWidth = 640;
    screenHeight = 480;

    int flags = 0;
#ifndef _WIN32
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    flags |= SDL_WINDOW_OPENGL;
#endif

    SDL_Window * window = SDL_CreateWindow("AGPU Sample", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, flags);
    if(!window)
    {
        printError("Failed to open window\n");
        return -1;
    }

    // Get the platform.
    agpu_platform *platform;
    agpuGetPlatforms(1, &platform, nullptr);
    if(!platform)
    {
        printError("Failed to get AGPU platform\n");
        return -1;
    }

    printMessage("Choosen platform: %s\n", agpuGetPlatformName(platform));

    // Get the window info.
    SDL_SysWMinfo windowInfo;
    SDL_VERSION(&windowInfo.version);
    SDL_GetWindowWMInfo(window, &windowInfo);

    // Open the device
    agpu_swap_chain_create_info swapChainCreateInfo;
    agpu_device_open_info openInfo;
    memset(&openInfo, 0, sizeof(openInfo));
    memset(&swapChainCreateInfo, 0, sizeof(swapChainCreateInfo));
    switch(windowInfo.subsystem)
    {
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
    case SDL_SYSWM_WINDOWS:
        swapChainCreateInfo.window = (agpu_pointer)windowInfo.info.win.window;
        break;
#endif
#if defined(SDL_VIDEO_DRIVER_X11)
    case SDL_SYSWM_X11:
        openInfo.display = (agpu_pointer)windowInfo.info.x11.display;
        swapChainCreateInfo.window = (agpu_pointer)(uintptr_t)windowInfo.info.x11.window;
        break;
#endif
    default:
        printError("Unsupported window system\n");
        return -1;
    }

    swapChainCreateInfo.colorbuffer_format = AGPU_TEXTURE_FORMAT_R8G8B8A8_UNORM;
    swapChainCreateInfo.depth_stencil_format = AGPU_TEXTURE_FORMAT_D16_UNORM;
    swapChainCreateInfo.width = screenWidth;
    swapChainCreateInfo.height = screenHeight;
    swapChainCreateInfo.doublebuffer = 1;
#ifdef _DEBUG
    // Use the debug layer when debugging. This is useful for low level backends.
    openInfo.debug_layer= true;
#endif

    device = agpuOpenDevice(platform, &openInfo);
    if(!device)
    {
        printError("Failed to open the device\n");
        return false;
    }

    // Get the default command queue
    commandQueue = agpuGetDefaultCommandQueue(device);

    // Create the swap chain.
    swapChain = agpuCreateSwapChain(device, commandQueue, &swapChainCreateInfo);
    if(!swapChain)
    {
        printError("Failed to create the swap chain\n");
        return false;
    }

    // Get the preferred shader language.
    preferredShaderLanguage = agpuGetPreferredHighLevelShaderLanguage(device);

    if(!initializeSample())
        return -1;

    quit = false;
    while(!quit)
    {
        processEvents();
        render();

        SDL_Delay(3);
    }

    shutdownSample();
    agpuReleaseSwapChain(swapChain);
    agpuReleaseCommandQueue(commandQueue);
    agpuReleaseDevice(device);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void SampleBase::processEvents()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_MOUSEBUTTONDOWN:
            printMessage("Mouse down\n");
            break;
        case SDL_FINGERDOWN:
            printMessage("Finger down\n");
            break;
        case SDL_KEYDOWN:
            onKeyDown(event);
            break;
        case SDL_KEYUP:
            onKeyUp(event);
            break;
        case SDL_QUIT:
            quit = true;
            break;
        default:
            //ignore
            break;
        }
    }
}

void SampleBase::onKeyDown(const SDL_Event &event)
{
    switch(event.key.keysym.sym)
    {
    case SDLK_ESCAPE:
        quit = true;
        break;
    default:
        // ignore
        break;
    }
}

void SampleBase::onKeyUp(const SDL_Event &event)
{
}

bool SampleBase::initializeSample()
{
    return true;
}

void SampleBase::shutdownSample()
{
}

void SampleBase::render()
{
}

void SampleBase::swapBuffers()
{
    agpuSwapBuffers(swapChain);
}

agpu_shader *SampleBase::compileShaderFromFile(const char *fileName, agpu_shader_type type)
{
    // Read the source file
    std::string fullName = fileName;
    switch (preferredShaderLanguage)
    {
    case AGPU_SHADER_LANGUAGE_GLSL:
        fullName += ".glsl";
        break;
    case AGPU_SHADER_LANGUAGE_HLSL:
        fullName += ".hlsl";
        break;
    case AGPU_SHADER_LANGUAGE_BINARY:
        fullName += ".cso";
        break;
    case AGPU_SHADER_LANGUAGE_SPIR_V:
        fullName += ".spirv";
        break;
    default:
        break;
    }

    auto source = readWholeFile(fullName);
    if(source.empty())
        return nullptr;

    // Create the shader and compile it.
    auto shader = agpuCreateShader(device, type);
    agpuSetShaderSource(shader, preferredShaderLanguage, source.c_str(), (agpu_string_length)source.size());
    if(agpuCompileShader(shader, nullptr) != AGPU_OK)
    {
        auto logLength = agpuGetShaderCompilationLogLength(shader);
        std::unique_ptr<char[]> logBuffer(new char[logLength+1]);
        agpuGetShaderCompilationLog(shader, logLength+1, logBuffer.get());
        agpuReleaseShader(shader);
        printError("Compilation error of '%s':%s\n", fullName.c_str(), logBuffer.get());
        return nullptr;
    }

    return shader;
}

agpu_pipeline_state *SampleBase::buildPipeline(agpu_pipeline_builder *builder)
{
    auto pipeline = agpuBuildPipelineState(builder);

    // Check the link result.
    if(!pipeline)
    {
        auto logLength = agpuGetPipelineBuildingLogLength(builder);
        std::unique_ptr<char[]> logBuffer(new char[logLength + 1]);
        agpuGetPipelineBuildingLog(builder, logLength+1, logBuffer.get());
        printError("Pipeline building error: %s\n", logBuffer.get());
        return nullptr;
    }

    return pipeline;
}

agpu_buffer *SampleBase::createImmutableVertexBuffer(size_t capacity, size_t vertexSize, void *initialData)
{
    agpu_buffer_description desc;
    desc.size = agpu_uint(capacity * vertexSize);
    desc.usage = AGPU_STATIC;
    desc.binding = AGPU_ARRAY_BUFFER;
    desc.mapping_flags = 0;
    desc.stride = agpu_uint(vertexSize);
    return agpuCreateBuffer(device, &desc, initialData);
}

agpu_buffer *SampleBase::createImmutableIndexBuffer(size_t capacity, size_t indexSize, void *initialData)
{
    agpu_buffer_description desc;
    desc.size = agpu_uint(capacity * indexSize);
    desc.usage = AGPU_STATIC;
    desc.binding = AGPU_ELEMENT_ARRAY_BUFFER;
    desc.mapping_flags = 0;
    desc.stride = agpu_uint(indexSize);
    return agpuCreateBuffer(device, &desc, initialData);
}

agpu_buffer *SampleBase::createImmutableDrawBuffer(size_t capacity, void *initialData)
{
    size_t commandSize = sizeof(agpu_draw_elements_command);
    agpu_buffer_description desc;
    desc.size = agpu_uint(capacity * commandSize);
    desc.usage = AGPU_STATIC;
    desc.binding = AGPU_DRAW_INDIRECT_BUFFER;
    desc.mapping_flags = 0;
    desc.stride = agpu_uint(commandSize);
    return agpuCreateBuffer(device, &desc, initialData);
}

agpu_buffer *SampleBase::createUploadableUniformBuffer(size_t capacity, void *initialData)
{
    agpu_buffer_description desc;
    desc.size = agpu_uint(capacity);
    desc.usage = AGPU_DYNAMIC;
    desc.binding = AGPU_UNIFORM_BUFFER;
    desc.mapping_flags = AGPU_MAP_DYNAMIC_STORAGE_BIT | AGPU_MAP_WRITE_BIT;
    desc.stride = 0;
    return agpuCreateBuffer(device, &desc, initialData);
}

agpu_texture *SampleBase::loadTexture(const char *fileName)
{
    auto surface = SDL_LoadBMP(fileName);
    if (!surface)
        return nullptr;

    auto convertedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(surface);
    if (!convertedSurface)
        return nullptr;

    agpu_texture_description desc;
    memset(&desc, 0, sizeof(desc));
    desc.type = AGPU_TEXTURE_2D;
    desc.format = AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM_SRGB;
    desc.width = convertedSurface->w;
    desc.height = convertedSurface->h;
    desc.depthOrArraySize = 1;
    desc.miplevels = 1;
    desc.flags = AGPU_TEXTURE_FLAG_UPLOADED;
    auto texture = agpuCreateTexture(device, &desc);
    if (!texture)
        return nullptr;

    agpuUploadTextureData(texture, 0, 0, convertedSurface->pitch, convertedSurface->pitch*convertedSurface->h, convertedSurface->pixels);
    SDL_FreeSurface(convertedSurface);

    return texture;

}
