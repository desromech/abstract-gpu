#include <stdio.h>
#include <vector>
#include <memory>
#include "SampleBase.hpp"
#include "SDL_syswm.h"

agpu_vertex_attrib_description SampleVertex::Description[] = {
    {0, AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_POSITION, AGPU_TEXTURE_FORMAT_R32G32B32_FLOAT, offsetof(SampleVertex, position), 0},
    {0, AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_COLOR, AGPU_TEXTURE_FORMAT_R32G32B32A32_FLOAT, offsetof(SampleVertex, color), 0},
    {0, AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_NORMAL, AGPU_TEXTURE_FORMAT_R32G32B32_FLOAT, offsetof(SampleVertex, normal), 0},
    {0, AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_TEXCOORD, AGPU_TEXTURE_FORMAT_R32G32_FLOAT, offsetof(SampleVertex, texcoord), 0},
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

agpu_shader *AbstractSampleBase::compileShaderFromFile(const char *fileName, agpu_shader_type type)
{
    auto source = readWholeFile(fileName);
    if(source.empty())
        return nullptr;

    // Create the offline shader compiler.
    auto shaderCompiler = agpuCreateOfflineShaderCompilerForDevice(device);
    agpuSetOfflineShaderCompilerSource(shaderCompiler, AGPU_SHADER_LANGUAGE_GLSL, type, source.c_str(), (agpu_string_length)source.size());
    if(agpuCompileOfflineShader(shaderCompiler, AGPU_SHADER_LANGUAGE_DEVICE_SHADER, nullptr) != AGPU_OK)
    {
        auto logLength = agpuGetOfflineShaderCompilationLogLength(shaderCompiler);
        std::unique_ptr<char[]> logBuffer(new char[logLength+1]);
        agpuGetOfflineShaderCompilationLog(shaderCompiler, logLength+1, logBuffer.get());
        agpuReleaseOfflineShaderCompiler(shaderCompiler);
        printError("Compilation error of '%s':%s\n", fileName, logBuffer.get());
        return nullptr;
    }

    // Get the shader result.
    auto shader = agpuGetOfflineShaderCompilerResultAsShader(shaderCompiler);
    agpuReleaseOfflineShaderCompiler(shaderCompiler);
    return shader;
}

agpu_pipeline_state *AbstractSampleBase::buildPipeline(agpu_pipeline_builder *builder)
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

agpu_pipeline_state *AbstractSampleBase::buildComputePipeline(agpu_compute_pipeline_builder *builder)
{
	auto pipeline = agpuBuildComputePipelineState(builder);

	// Check the link result.
	if (!pipeline)
	{
		auto logLength = agpuGetComputePipelineBuildingLogLength(builder);
		std::unique_ptr<char[]> logBuffer(new char[logLength + 1]);
		agpuGetComputePipelineBuildingLog(builder, logLength + 1, logBuffer.get());
		printError("Pipeline building error: %s\n", logBuffer.get());
		return nullptr;
	}

	return pipeline;
}

agpu_buffer *AbstractSampleBase::createImmutableVertexBuffer(size_t capacity, size_t vertexSize, void *initialData)
{
    agpu_buffer_description desc;
    desc.size = agpu_uint(capacity * vertexSize);
    desc.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
    desc.usage_modes = desc.main_usage_mode = AGPU_ARRAY_BUFFER;
    desc.mapping_flags = 0;
    desc.stride = agpu_uint(vertexSize);
    return agpuCreateBuffer(device, &desc, initialData);
}

agpu_buffer *AbstractSampleBase::createImmutableIndexBuffer(size_t capacity, size_t indexSize, void *initialData)
{
    agpu_buffer_description desc;
    desc.size = agpu_uint(capacity * indexSize);
    desc.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
    desc.usage_modes = desc.main_usage_mode = AGPU_ELEMENT_ARRAY_BUFFER;
    desc.mapping_flags = 0;
    desc.stride = agpu_uint(indexSize);
    return agpuCreateBuffer(device, &desc, initialData);
}

agpu_buffer *AbstractSampleBase::createImmutableDrawBuffer(size_t capacity, void *initialData)
{
    size_t commandSize = sizeof(agpu_draw_elements_command);
    agpu_buffer_description desc;
    desc.size = agpu_uint(capacity * commandSize);
    desc.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
    desc.usage_modes = desc.main_usage_mode = AGPU_DRAW_INDIRECT_BUFFER;
    desc.mapping_flags = 0;
    desc.stride = agpu_uint(commandSize);
    return agpuCreateBuffer(device, &desc, initialData);
}

agpu_buffer *AbstractSampleBase::createUploadableUniformBuffer(size_t capacity, void *initialData)
{
    agpu_buffer_description desc;
    desc.size = agpu_uint(capacity);
    desc.heap_type = AGPU_MEMORY_HEAP_TYPE_HOST_TO_DEVICE;
    desc.usage_modes = desc.main_usage_mode = AGPU_UNIFORM_BUFFER;
    desc.mapping_flags = AGPU_MAP_DYNAMIC_STORAGE_BIT | AGPU_MAP_WRITE_BIT;
    desc.stride = 0;
    return agpuCreateBuffer(device, &desc, initialData);
}

agpu_buffer *AbstractSampleBase::createMappableUploadBuffer(size_t capacity, void *initialData)
{
    agpu_buffer_description desc;
	desc.size = agpu_uint(capacity);
	desc.heap_type = AGPU_MEMORY_HEAP_TYPE_HOST_TO_DEVICE;
	desc.usage_modes = desc.main_usage_mode = AGPU_COPY_SOURCE_BUFFER;
	desc.mapping_flags = AGPU_MAP_WRITE_BIT | AGPU_MAP_READ_BIT;
	desc.stride = 0;
	if (hasPersistentCoherentMapping)
		desc.mapping_flags |= AGPU_MAP_PERSISTENT_BIT | AGPU_MAP_COHERENT_BIT;

	return agpuCreateBuffer(device, &desc, initialData);
}

agpu_buffer *AbstractSampleBase::createMappableReadbackBuffer(size_t capacity, void *initialData)
{
    agpu_buffer_description desc;
	desc.size = agpu_uint(capacity);
	desc.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_TO_HOST;
	desc.usage_modes = desc.main_usage_mode = AGPU_COPY_DESTINATION_BUFFER;
	desc.mapping_flags = AGPU_MAP_WRITE_BIT | AGPU_MAP_READ_BIT;
	desc.stride = 0;
	if (hasPersistentCoherentMapping)
		desc.mapping_flags |= AGPU_MAP_PERSISTENT_BIT | AGPU_MAP_COHERENT_BIT;

	return agpuCreateBuffer(device, &desc, initialData);
}

agpu_buffer *AbstractSampleBase::createStorageBuffer(size_t capacity, size_t stride, void *initialData)
{
    agpu_buffer_description desc;
	desc.size = agpu_uint(capacity);
	desc.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
	desc.usage_modes = desc.main_usage_mode = AGPU_STORAGE_BUFFER;
	desc.stride = stride;

	return agpuCreateBuffer(device, &desc, initialData);
}

agpu_texture *AbstractSampleBase::loadTexture(const char *fileName)
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
    desc.format = AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM;
    desc.width = convertedSurface->w;
    desc.height = convertedSurface->h;
    desc.depth = 1;
    desc.layers = 1;
    desc.miplevels = 1;
    desc.sample_count = 1;
    desc.sample_quality = 0;
    desc.usage_modes = agpu_texture_usage_mode_mask(AGPU_TEXTURE_USAGE_SAMPLED | AGPU_TEXTURE_USAGE_UPLOADED);
    desc.main_usage_mode = AGPU_TEXTURE_USAGE_SAMPLED;
    auto texture = agpuCreateTexture(device, &desc);
    if (!texture)
        return nullptr;

    agpuUploadTextureData(texture, 0, 0, convertedSurface->pitch, convertedSurface->pitch*convertedSurface->h, convertedSurface->pixels);
    SDL_FreeSurface(convertedSurface);

    return texture;
}

int SampleBase::main(int argc, const char **argv)
{
    char nameBuffer[256];
    SDL_Init(SDL_INIT_VIDEO);

    screenWidth = 640;
    screenHeight = 480;

    int flags = 0;

    // Get the platform.
    agpu_platform *platform;
    agpuGetPlatforms(1, &platform, nullptr);
    if (!platform)
    {
        printError("Failed to get AGPU platform\n");
        return -1;
    }

    printMessage("Choosen platform: %s\n", agpuGetPlatformName(platform));
    snprintf(nameBuffer, sizeof(nameBuffer), "AGPU Sample - %s Platform", agpuGetPlatformName(platform));
    SDL_Window * window = SDL_CreateWindow(nameBuffer, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, flags);
    if(!window)
    {
        printError("Failed to open window\n");
        return -1;
    }

    // Get the window info.
    SDL_SysWMinfo windowInfo;
    SDL_VERSION(&windowInfo.version);
    SDL_GetWindowWMInfo(window, &windowInfo);

    // Open the device
	agpu_swap_chain_create_info swapChainCreateInfo = {};
	agpu_device_open_info openInfo = {};
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
#if defined(SDL_VIDEO_DRIVER_COCOA)
    case SDL_SYSWM_COCOA:
        swapChainCreateInfo.window = (agpu_pointer)windowInfo.info.cocoa.window;
        break;
#endif
    default:
        printError("Unsupported window system\n");
        return -1;
    }

    swapChainCreateInfo.colorbuffer_format = AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM;
    swapChainCreateInfo.depth_stencil_format = AGPU_TEXTURE_FORMAT_D32_FLOAT_S8X24_UINT;
    swapChainCreateInfo.width = screenWidth;
    swapChainCreateInfo.height = screenHeight;
    swapChainCreateInfo.buffer_count = 3;
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

	hasPersistentCoherentMapping = agpuIsFeatureSupportedOnDevice(device, AGPU_FEATURE_PERSISTENT_COHERENT_MEMORY_MAPPING);

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
    preferredShaderLanguage = agpuGetPreferredIntermediateShaderLanguage(device);
    if(preferredShaderLanguage == AGPU_SHADER_LANGUAGE_NONE)
    {
        preferredShaderLanguage = agpuGetPreferredHighLevelShaderLanguage(device);
        if(preferredShaderLanguage == AGPU_SHADER_LANGUAGE_NONE)
            preferredShaderLanguage = agpuGetPreferredShaderLanguage(device);
    }

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

agpu_renderpass *SampleBase::createMainPass(const glm::vec4 &clearColor)
{
    // Color attachment
    agpu_renderpass_color_attachment_description colorAttachment;
    memset(&colorAttachment, 0, sizeof(colorAttachment));
    colorAttachment.format = AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM;
    colorAttachment.begin_action = AGPU_ATTACHMENT_CLEAR;
    colorAttachment.end_action = AGPU_ATTACHMENT_KEEP;
    colorAttachment.clear_value.r = clearColor.r;
    colorAttachment.clear_value.g = clearColor.g;
    colorAttachment.clear_value.b = clearColor.b;
    colorAttachment.clear_value.a = clearColor.a;

    // Depth stencil
    agpu_renderpass_depth_stencil_description depthStencil;
    memset(&depthStencil, 0, sizeof(depthStencil));
    depthStencil.format = AGPU_TEXTURE_FORMAT_D32_FLOAT_S8X24_UINT;
    depthStencil.begin_action = AGPU_ATTACHMENT_CLEAR;
    depthStencil.end_action = AGPU_ATTACHMENT_KEEP;
    depthStencil.clear_value.depth = 1.0;

    agpu_renderpass_description description;
    memset(&description, 0, sizeof(description));
    description.color_attachment_count = 1;
    description.color_attachments = &colorAttachment;
    description.depth_stencil_attachment = &depthStencil;
    return agpuCreateRenderPass(device, &description);
}


int ComputeSampleBase::main(int argc, const char **argv)
{
    // Get the platform.
    agpu_platform *platform;
    agpuGetPlatforms(1, &platform, nullptr);
    if (!platform)
    {
        printError("Failed to get AGPU platform\n");
        return -1;
    }

    printMessage("Choosen platform: %s\n", agpuGetPlatformName(platform));

    // Open the device
	agpu_device_open_info openInfo = {};
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

	hasPersistentCoherentMapping = agpuIsFeatureSupportedOnDevice(device, AGPU_FEATURE_PERSISTENT_COHERENT_MEMORY_MAPPING);

    // Get the default command queue
    commandQueue = agpuGetDefaultCommandQueue(device);

    // Get the preferred shader language.
    preferredShaderLanguage = agpuGetPreferredIntermediateShaderLanguage(device);
    if(preferredShaderLanguage == AGPU_SHADER_LANGUAGE_NONE)
    {
        preferredShaderLanguage = agpuGetPreferredHighLevelShaderLanguage(device);
        if(preferredShaderLanguage == AGPU_SHADER_LANGUAGE_NONE)
            preferredShaderLanguage = agpuGetPreferredShaderLanguage(device);
    }

    return run(argc, argv);
}

int ComputeSampleBase::run(int argc, const char **argv)
{
    return 0;
}
