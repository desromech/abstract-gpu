#include <stdio.h>
#include <vector>
#include <memory>
#include "SampleBase.hpp"
#include "SDL_syswm.h"

	agpu_uint binding;
	agpu_field_type type;
	agpu_uint components;
	agpu_bool normalized;
	agpu_size offset;
    
agpu_vertex_attrib_description SampleVertex::Description[] = {
    {0, AGPU_FLOAT, 3, false, offsetof(SampleVertex, position)},
    {1, AGPU_FLOAT, 4, false, offsetof(SampleVertex, color)},
    {2, AGPU_FLOAT, 3, false, offsetof(SampleVertex, normal)},
    {3, AGPU_FLOAT, 2, false, offsetof(SampleVertex, texcoord)},
};

const int SampleVertex::DescriptionSize = 4;
    
std::string readWholeFile(const char *fileName)
{
    FILE *file = fopen(fileName, "rb");
    if(!file)
    {
        fprintf(stderr, "Failed to open file %s\n", fileName);
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
        fprintf(stderr, "Failed to read file %s\n", fileName);
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

    SDL_Window * window = SDL_CreateWindow("AGPU Sample", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
    if(!window)
    {
        fprintf(stderr, "Failed to open window\n");
        return -1;
    }

    // Get the platform.
    agpu_platform *platform;
    agpuGetPlatforms(1, &platform, nullptr);
    if(!platform)
    {
        fprintf(stderr, "Failed to get AGPU platform\n");
        return -1;
    }

    // Get the window info.
    SDL_SysWMinfo windowInfo;
    SDL_VERSION(&windowInfo.version);
    SDL_GetWindowWMInfo(window, &windowInfo);

    // Open the device
    agpu_device_open_info openInfo;
    memset(&openInfo, 0, sizeof(openInfo));
    switch(windowInfo.subsystem)
    {
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
    case SDL_SYSWM_WINDOWS:
        openInfo.window = (agpu_pointer)windowInfo.info.win.window;
        openInfo.surface = (agpu_pointer)windowInfo.info.win.hdc;
        break;
#endif
#if defined(SDL_VIDEO_DRIVER_X11)
    case SDL_SYSWM_X11:
        openInfo.display = (agpu_pointer)windowInfo.info.x11.display;
        openInfo.window = (agpu_pointer)(uintptr_t)windowInfo.info.x11.window;
        break;
#endif
    default:
        fprintf(stderr, "Unsupported window system\n");
        return -1;
    }

    openInfo.red_size = 5;
    openInfo.blue_size = 5;
    openInfo.green_size = 5;
    openInfo.alpha_size = 5;
    openInfo.doublebuffer = 1;
#ifdef _DEBUG
    // Use the debug layer when debugging. This is useful for low level backends.
    openInfo.debugLayer = true;
#endif

    device = agpuOpenDevice(platform, &openInfo);
    if(!device)
    {
        fprintf(stderr, "failed to open the device");
        return false;
    }

    if(!initializeSample())
        return -1;

    quit = false;
    while(!quit)
    {
        processEvents();
        render();
    }

    shutdownSample();
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
            printf("Mouse down\n");
            break;
        case SDL_FINGERDOWN:
            printf("Finger down\n");
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
    agpuSwapBuffers(device);
}

agpu_shader *SampleBase::compileShaderFromFile(const char *fileName, agpu_shader_type type, agpu_shader_language language)
{
    // Read the source file
    auto source = readWholeFile(fileName);
    if(source.empty())
        return nullptr;

    // Create the shader and compile it.        
    auto shader = agpuCreateShader(device, type);
    agpuSetShaderSource(shader, language, source.c_str(), (agpu_string_length)source.size());
    if(agpuCompileShader(shader, nullptr) != AGPU_OK)
    {
        auto logLength = agpuGetShaderCompilationLogLength(shader);
        std::unique_ptr<char[]> logBuffer(new char[logLength+1]);
        agpuGetShaderCompilationLog(shader, logLength, logBuffer.get());
        agpuReleaseShader(shader);
        fprintf(stderr, "Compilation error of '%s':%s\n", fileName, logBuffer.get());
        return nullptr;
    }
    
    // Bind some attributes.  
    if (type == AGPU_VERTEX_SHADER)
    {
        agpuBindAttributeLocation(shader, "vPosition", 0);
        agpuBindAttributeLocation(shader, "vColor", 1);
        agpuBindAttributeLocation(shader, "vNormal", 2);
        agpuBindAttributeLocation(shader, "vTexCoord", 3);
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
        agpuGetPipelineBuildingLog(builder, logLength, logBuffer.get());
        fprintf(stderr, "Pipeline building error:%s\n", logBuffer.get());
        return nullptr;
    }
    
    return pipeline;
}

agpu_buffer *SampleBase::createImmutableVertexBuffer(size_t capacity, size_t vertexSize, void *initialData)
{
    agpu_buffer_description desc;
    desc.size = capacity * vertexSize;
    desc.usage = AGPU_STATIC;
    desc.binding = AGPU_ARRAY_BUFFER;
    desc.mapping_flags = 0;
    desc.stride = vertexSize;
    return agpuCreateBuffer(device, &desc, initialData);
}

agpu_buffer *SampleBase::createImmutableIndexBuffer(size_t capacity, size_t indexSize, void *initialData)
{
    agpu_buffer_description desc;
    desc.size = capacity * indexSize;
    desc.usage = AGPU_STATIC;
    desc.binding = AGPU_ELEMENT_ARRAY_BUFFER;
    desc.mapping_flags = 0;
    desc.stride = indexSize;
    return agpuCreateBuffer(device, &desc, initialData);
}

agpu_buffer *SampleBase::createImmutableDrawBuffer(size_t capacity, void *initialData)
{
    size_t commandSize = sizeof(agpu_draw_elements_command);
    agpu_buffer_description desc;
    desc.size = capacity * commandSize;
    desc.usage = AGPU_STATIC;
    desc.binding = AGPU_DRAW_INDIRECT_BUFFER;
    desc.mapping_flags = 0;
    desc.stride = commandSize;
    return agpuCreateBuffer(device, &desc, initialData);
}
