#define NOMINMAX
#include <stdio.h>
#include <vector>
#include <memory>
#include <algorithm>
#include "SampleBase.hpp"
#include "SampleVertex.hpp"
#include "SDL_syswm.h"

agpu_vertex_attrib_description SampleVertex::Description[] = {
    {0, AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_POSITION, AGPU_TEXTURE_FORMAT_R32G32B32_FLOAT, offsetof(SampleVertex, position), 0},
    {0, AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_COLOR, AGPU_TEXTURE_FORMAT_R32G32B32A32_FLOAT, offsetof(SampleVertex, color), 0},
    {0, AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_NORMAL, AGPU_TEXTURE_FORMAT_R32G32B32_FLOAT, offsetof(SampleVertex, normal), 0},
    {0, AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_TEXCOORD, AGPU_TEXTURE_FORMAT_R32G32_FLOAT, offsetof(SampleVertex, texcoord), 0},
    {0, AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_BONE_INDICES, AGPU_TEXTURE_FORMAT_R32G32B32A32_UINT, offsetof(SampleVertex, boneIndices), 0},
    {0, AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_BONE_WEIGHTS, AGPU_TEXTURE_FORMAT_R32G32B32A32_FLOAT, offsetof(SampleVertex, boneWeights), 0},
    {0, AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_TANGENT_4, AGPU_TEXTURE_FORMAT_R32G32B32A32_FLOAT, offsetof(SampleVertex, tangent4), 0},
};

const int SampleVertex::DescriptionSize = sizeof(Description) / sizeof(Description[0]);

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

agpu_shader_ref AbstractSampleBase::compileShaderFromFile(const char *fileName, agpu_shader_type type)
{
    auto source = readWholeFile(fileName);
    if(source.empty())
        return nullptr;

    // Create the shader compiler.
    agpu_offline_shader_compiler_ref shaderCompiler = device->createOfflineShaderCompiler();
    shaderCompiler->setShaderSource(AGPU_SHADER_LANGUAGE_VGLSL, type, source.c_str(), (agpu_string_length)source.size());
    try
    {
        shaderCompiler->compileShader(AGPU_SHADER_LANGUAGE_DEVICE_SHADER, nullptr);
    }
    catch(agpu_exception &e)
    {
        auto logLength = shaderCompiler->getCompilationLogLength();
        std::unique_ptr<char[]> logBuffer(new char[logLength+1]);
        shaderCompiler->getCompilationLog(logLength+1, logBuffer.get());
        printError("Compilation error of '%s':%s\n", fileName, logBuffer.get());
        return nullptr;
    }

    // Create the shader and compile it.
    return shaderCompiler->getResultAsShader();
}

agpu_pipeline_state_ref AbstractSampleBase::buildPipeline(const agpu_pipeline_builder_ref &builder)
{
    agpu_pipeline_state_ref pipeline = builder->build();

    // Check the link result.
    if(!pipeline)
    {
        auto logLength = builder->getBuildingLogLength();
        std::unique_ptr<char[]> logBuffer(new char[logLength + 1]);
        builder->getBuildingLog(logLength+1, logBuffer.get());
        printError("Pipeline building error: %s\n", logBuffer.get());
        return nullptr;
    }

    return pipeline;
}

agpu_pipeline_state_ref AbstractSampleBase::buildComputePipeline(const agpu_compute_pipeline_builder_ref &builder)
{
    agpu_pipeline_state_ref pipeline = builder->build();

	// Check the link result.
	if (!pipeline)
	{
		auto logLength = builder->getBuildingLogLength();
		std::unique_ptr<char[]> logBuffer(new char[logLength + 1]);
        builder->getBuildingLog(logLength+1, logBuffer.get());
		printError("Pipeline building error: %s\n", logBuffer.get());
		return nullptr;
	}

	return pipeline;
}

agpu_buffer_ref AbstractSampleBase::createImmutableVertexBuffer(size_t capacity, size_t vertexSize, void *initialData)
{
    agpu_buffer_description desc;
    desc.size = agpu_uint(capacity * vertexSize);
    desc.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
    desc.usage_modes = desc.main_usage_mode = AGPU_ARRAY_BUFFER;
    desc.mapping_flags = 0;
    desc.stride = agpu_uint(vertexSize);
    return device->createBuffer(&desc, initialData);
}

agpu_buffer_ref AbstractSampleBase::createImmutableIndexBuffer(size_t capacity, size_t indexSize, void *initialData)
{
    agpu_buffer_description desc;
    desc.size = agpu_uint(capacity * indexSize);
    desc.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
    desc.usage_modes = desc.main_usage_mode = AGPU_ELEMENT_ARRAY_BUFFER;
    desc.mapping_flags = 0;
    desc.stride = agpu_uint(indexSize);
    return device->createBuffer(&desc, initialData);
}

agpu_buffer_ref AbstractSampleBase::createImmutableDrawBuffer(size_t capacity, void *initialData)
{
    size_t commandSize = sizeof(agpu_draw_elements_command);
    agpu_buffer_description desc;
    desc.size = agpu_uint(capacity * commandSize);
    desc.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
    desc.usage_modes = desc.main_usage_mode = AGPU_DRAW_INDIRECT_BUFFER;
    desc.mapping_flags = 0;
    desc.stride = agpu_uint(commandSize);
    return device->createBuffer(&desc, initialData);
}

agpu_buffer_ref AbstractSampleBase::createUploadableUniformBuffer(size_t capacity, void *initialData)
{
    agpu_buffer_description desc;
    desc.size = agpu_uint(capacity);
    desc.heap_type = AGPU_MEMORY_HEAP_TYPE_HOST_TO_DEVICE;
    desc.usage_modes = desc.main_usage_mode = AGPU_UNIFORM_BUFFER;
    desc.mapping_flags = AGPU_MAP_DYNAMIC_STORAGE_BIT | AGPU_MAP_WRITE_BIT;
    desc.stride = 0;
    return device->createBuffer(&desc, initialData);
}

agpu_buffer_ref AbstractSampleBase::createMappableUploadBuffer(size_t capacity, void *initialData)
{
    agpu_buffer_description desc;
	desc.size = agpu_uint(capacity);
	desc.heap_type = AGPU_MEMORY_HEAP_TYPE_HOST_TO_DEVICE;
	desc.usage_modes = desc.main_usage_mode = AGPU_COPY_SOURCE_BUFFER;
	desc.mapping_flags = AGPU_MAP_WRITE_BIT | AGPU_MAP_READ_BIT;
	desc.stride = 0;
	if (hasPersistentCoherentMapping)
		desc.mapping_flags |= AGPU_MAP_PERSISTENT_BIT | AGPU_MAP_COHERENT_BIT;

	return device->createBuffer(&desc, initialData);
}

agpu_buffer_ref AbstractSampleBase::createMappableReadbackBuffer(size_t capacity, void *initialData)
{
    agpu_buffer_description desc;
	desc.size = agpu_uint(capacity);
	desc.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_TO_HOST;
	desc.usage_modes = desc.main_usage_mode = AGPU_COPY_DESTINATION_BUFFER;
	desc.mapping_flags = AGPU_MAP_WRITE_BIT | AGPU_MAP_READ_BIT;
	desc.stride = 0;
	if (hasPersistentCoherentMapping)
		desc.mapping_flags |= AGPU_MAP_PERSISTENT_BIT | AGPU_MAP_COHERENT_BIT;

	return device->createBuffer(&desc, initialData);
}

agpu_buffer_ref AbstractSampleBase::createStorageBuffer(size_t capacity, size_t stride, void *initialData)
{
    agpu_buffer_description desc;
	desc.size = agpu_size(capacity);
	desc.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
	desc.usage_modes = desc.main_usage_mode = AGPU_STORAGE_BUFFER;
	desc.stride = agpu_size(stride);

	return device->createBuffer(&desc, initialData);
}

agpu_texture_ref AbstractSampleBase::loadTexture(const char *fileName, bool nonColorData)
{
    auto surface = SDL_LoadBMP(fileName);
    if (!surface)
        return nullptr;

    auto convertedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(surface);
    if (!convertedSurface)
        return nullptr;

    bool generateMipmaps = isPowerOfTwo(convertedSurface->w) && isPowerOfTwo(convertedSurface->h);
    size_t miplevelCount = 1;
    if(generateMipmaps)
    {
        size_t currentWidth = convertedSurface->w;
        size_t currentHeight = convertedSurface->h;
        while(currentWidth > 1 || currentHeight > 1)
        {
            ++miplevelCount;
            currentWidth = std::max(currentWidth / 2, size_t(1));
            currentHeight = std::max(currentHeight / 2, size_t(1));
        }
    }

    auto format = nonColorData ? AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM : AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM_SRGB;
    agpu_texture_description desc = {};
    desc.type = AGPU_TEXTURE_2D;
    desc.format = format;
    desc.width = convertedSurface->w;
    desc.height = convertedSurface->h;
    desc.depth = 1;
    desc.layers = 1;
    desc.miplevels = agpu_size(miplevelCount);
    desc.sample_count = 1;
    desc.sample_quality = 0;
    desc.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
    desc.usage_modes = agpu_texture_usage_mode_mask(AGPU_TEXTURE_USAGE_SAMPLED | AGPU_TEXTURE_USAGE_COPY_DESTINATION);
    desc.main_usage_mode = AGPU_TEXTURE_USAGE_SAMPLED;

    if(miplevelCount > 1)
    {
        if(useComputeShadersForMipmapGeneration && nonColorData)
            desc.usage_modes = agpu_texture_usage_mode_mask(desc.usage_modes | AGPU_TEXTURE_USAGE_STORAGE);
        else
            desc.usage_modes = agpu_texture_usage_mode_mask(desc.usage_modes | AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT);
    };

    agpu_texture_ref texture = device->createTexture(&desc);
    if (!texture)
    {
        SDL_FreeSurface(convertedSurface);
        return nullptr;
    }

    if(miplevelCount > 1)
    {
        auto uploadTexture = texture;
        auto uploadTextureMainUsage = AGPU_TEXTURE_USAGE_SAMPLED;
        if(useComputeShadersForMipmapGeneration && !nonColorData)
        {
            auto uploadTextureDesc = desc;
            uploadTextureDesc.format = AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM;
            uploadTextureDesc.usage_modes = agpu_texture_usage_mode_mask(AGPU_TEXTURE_USAGE_STORAGE | AGPU_TEXTURE_USAGE_COPY_SOURCE | AGPU_TEXTURE_USAGE_COPY_DESTINATION);
            uploadTextureDesc.main_usage_mode = AGPU_TEXTURE_USAGE_STORAGE;
            uploadTextureMainUsage = AGPU_TEXTURE_USAGE_STORAGE;

            uploadTexture = device->createTexture(&uploadTextureDesc);
            if(!uploadTexture)
            {
                SDL_FreeSurface(convertedSurface);
                return nullptr;
            }
        }

        size_t width = convertedSurface->w;
        size_t height = convertedSurface->h;
        auto uploadPitch = alignedTo(convertedSurface->w*4, device->getLimitValue(AGPU_LIMIT_MIN_TEXTURE_DATA_PITCH_ALIGNMENT));
        auto bufferSize = alignedTo(uploadPitch * convertedSurface->h, device->getLimitValue(AGPU_LIMIT_MIN_TEXTURE_DATA_OFFSET_ALIGNMENT));

        agpu_buffer_description desc = {};
        desc.size = agpu_size(bufferSize);
        desc.heap_type = AGPU_MEMORY_HEAP_TYPE_HOST_TO_DEVICE;
        desc.usage_modes = desc.main_usage_mode = AGPU_COPY_SOURCE_BUFFER;
        desc.mapping_flags = AGPU_MAP_WRITE_BIT;

        auto uploadBuffer = device->createBuffer(&desc, nullptr);
        if(!uploadBuffer)
        {
            SDL_FreeSurface(convertedSurface);
            return nullptr;
        }

        auto uploadDestRow = reinterpret_cast<uint8_t*> (uploadBuffer->mapBuffer(AGPU_WRITE_ONLY));
        auto sourceRow = reinterpret_cast<const uint8_t*> (convertedSurface->pixels);
        auto rowCopySize = convertedSurface->w*4;
        for(size_t y = 0; y < size_t(convertedSurface->h); ++y)
        {
            memcpy(uploadDestRow, sourceRow, rowCopySize);
            uploadDestRow += uploadPitch;
            sourceRow += convertedSurface->pitch;
        }
        uploadBuffer->unmapBuffer();
        SDL_FreeSurface(convertedSurface);

        auto commandAllocator = device->createCommandAllocator(AGPU_COMMAND_LIST_TYPE_DIRECT, commandQueue);
        auto commandList = device->createCommandList(AGPU_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr);

        {
            // Upload the first level.
            agpu_texture_subresource_range uploadRange = {};
            uploadRange.aspect = AGPU_TEXTURE_ASPECT_COLOR;
            uploadRange.level_count = 1;
            uploadRange.layer_count = 1;

            agpu_buffer_image_copy_region bufferImageCopyRegion = {};
            bufferImageCopyRegion.buffer_pitch = agpu_size(uploadPitch);
            bufferImageCopyRegion.buffer_slice_pitch = agpu_size(uploadPitch*height);
            bufferImageCopyRegion.texture_usage_mode = AGPU_TEXTURE_USAGE_COPY_DESTINATION;
            bufferImageCopyRegion.texture_subresource_level.aspect = AGPU_TEXTURE_ASPECT_COLOR;
            bufferImageCopyRegion.texture_subresource_level.layer_count = 1;
            bufferImageCopyRegion.texture_region.width = agpu_size(width);
            bufferImageCopyRegion.texture_region.height = agpu_size(height);
            bufferImageCopyRegion.texture_region.depth = 1;

            commandList->pushTextureTransitionBarrier(uploadTexture, uploadTextureMainUsage, AGPU_TEXTURE_USAGE_COPY_DESTINATION, &uploadRange);
            commandList->copyBufferToTexture(uploadBuffer, uploadTexture, &bufferImageCopyRegion);
            commandList->popTextureTransitionBarrier();
        }

        std::vector<agpu_shader_resource_binding_ref> resourceBindings;
        std::vector<agpu_texture_view_ref> textureViews;
        std::vector<agpu_framebuffer_ref> framebuffers;
        if(useComputeShadersForMipmapGeneration)
        {
            if(!mipmapComputationShaderSignature)
            {
                auto shaderSignatureBuilder = device->createShaderSignatureBuilder();
                shaderSignatureBuilder->beginBindingBank(32);
                shaderSignatureBuilder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE, 1);
                shaderSignatureBuilder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE, 1);
                mipmapComputationShaderSignature = shaderSignatureBuilder->build();
                if (!mipmapComputationShaderSignature)
                    return nullptr;

                {
                    auto computeShader = compileShaderFromFile("data/shaders/computeMipmap.glsl", AGPU_COMPUTE_SHADER);
                    if (!computeShader)
                        return nullptr;

                    auto pipelineBuilder = device->createComputePipelineBuilder();
            		pipelineBuilder->setShaderSignature(mipmapComputationShaderSignature);
            		pipelineBuilder->attachShader(computeShader);
                    mipmapComputationNonColorPipelineState = pipelineBuilder->build();
                    if(!mipmapComputationNonColorPipelineState)
                        return nullptr;
                }

                {
                    auto computeShader = compileShaderFromFile("data/shaders/computeMipmapSRGB.glsl", AGPU_COMPUTE_SHADER);
                    if (!computeShader)
                        return nullptr;

                    auto pipelineBuilder = device->createComputePipelineBuilder();
            		pipelineBuilder->setShaderSignature(mipmapComputationShaderSignature);
            		pipelineBuilder->attachShader(computeShader);
                    mipmapComputationColorPipelineState = pipelineBuilder->build();
                    if(!mipmapComputationColorPipelineState)
                        return nullptr;
                }
            }

            agpu_texture_view_description viewDescription = {};
            uploadTexture->getFullViewDescription(&viewDescription);
            viewDescription.usage_mode = AGPU_TEXTURE_USAGE_STORAGE;
            viewDescription.subresource_range.level_count = 1;

            auto lastView = uploadTexture->createView(&viewDescription);
            if(!lastView)
                return nullptr;

            textureViews.push_back(lastView);
            auto currentWidth = width;
            auto currentHeight = height;
            commandList->setShaderSignature(mipmapComputationShaderSignature);
            commandList->usePipelineState(nonColorData ? mipmapComputationNonColorPipelineState : mipmapComputationColorPipelineState);

            // Transition the texture for computation.
            {
                agpu_texture_subresource_range computeRange = {};
                computeRange.aspect = AGPU_TEXTURE_ASPECT_COLOR;
                computeRange.level_count = agpu_size(miplevelCount);
                computeRange.layer_count = 1;

                commandList->pushTextureTransitionBarrier(texture, uploadTextureMainUsage, AGPU_TEXTURE_USAGE_STORAGE, &computeRange);
            }

            while(currentWidth > 1 || currentHeight > 1)
            {
                auto nextWidth = std::max(currentWidth / 2, size_t(1));
                auto nextHeight = std::max(currentHeight / 2, size_t(1));

                ++viewDescription.subresource_range.base_miplevel;
                auto nextView = uploadTexture->createView(&viewDescription);
                textureViews.push_back(nextView);
                if(!nextView)
                    return nullptr;

                auto binding = mipmapComputationShaderSignature->createShaderResourceBinding(0);
                resourceBindings.push_back(binding);
                if(!binding)
                    return nullptr;

                binding->bindStorageImageView(0, lastView);
                binding->bindStorageImageView(1, nextView);
                commandList->useComputeShaderResources(binding);
                commandList->dispatchCompute(agpu_uint((currentWidth + 15) / 16), agpu_uint((currentHeight + 15) / 16), 1);

                // Make visible the miplevel.
                commandList->textureMemoryBarrier(uploadTexture, AGPU_PIPELINE_STAGE_COMPUTE_SHADER, agpu_pipeline_stage_flags(AGPU_PIPELINE_STAGE_COMPUTE_SHADER | AGPU_PIPELINE_STAGE_FRAGMENT_SHADER),
                        AGPU_ACCESS_SHADER_WRITE, AGPU_ACCESS_SHADER_READ,
                        AGPU_TEXTURE_USAGE_STORAGE, AGPU_TEXTURE_USAGE_STORAGE,
                        &viewDescription.subresource_range);

                currentWidth = nextWidth;
                currentHeight = nextHeight;
                lastView = nextView;
            }

            commandList->popTextureTransitionBarrier();

            // If the upload and sampled textures are different, copy the different levels.
            if(uploadTexture != texture)
            {
                {
                    agpu_texture_subresource_range copyRange = {};
                    copyRange.aspect = AGPU_TEXTURE_ASPECT_COLOR;
                    copyRange.level_count = agpu_size(miplevelCount);
                    copyRange.layer_count = 1;

                    commandList->pushTextureTransitionBarrier(uploadTexture, AGPU_TEXTURE_USAGE_STORAGE, AGPU_TEXTURE_USAGE_COPY_SOURCE, &copyRange);
                    commandList->pushTextureTransitionBarrier(texture, AGPU_TEXTURE_USAGE_SAMPLED, AGPU_TEXTURE_USAGE_COPY_DESTINATION, &copyRange);
                }

                for(size_t i = 0; i < miplevelCount; ++i)
                {
                    agpu_texture_subresource_level subresourceLevel = {};
                    subresourceLevel.aspect = AGPU_TEXTURE_ASPECT_COLOR;
                    subresourceLevel.miplevel = agpu_size(i);
                    subresourceLevel.layer_count = 1;

                    agpu_image_copy_region copyRegion = {};
                    copyRegion.source_usage_mode = AGPU_TEXTURE_USAGE_COPY_SOURCE;
                    copyRegion.source_subresource_level = subresourceLevel;
                    copyRegion.destination_usage_mode = AGPU_TEXTURE_USAGE_COPY_DESTINATION;
                    copyRegion.destination_subresource_level = subresourceLevel;
                    copyRegion.extent.width = agpu_uint(std::max(width >> i, size_t(1)));
                    copyRegion.extent.height = agpu_uint(std::max(height >> i, size_t(1)));
                    copyRegion.extent.depth = 1;

                    commandList->copyTexture(uploadTexture, texture, &copyRegion);
                }

                commandList->popTextureTransitionBarrier();
                commandList->popTextureTransitionBarrier();
            }
        }
        else
        {
            if(!mipmapComputationShaderSignature)
            {
                auto shaderSignatureBuilder = device->createShaderSignatureBuilder();
                shaderSignatureBuilder->beginBindingBank(1);
                shaderSignatureBuilder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_SAMPLER, 1);

                shaderSignatureBuilder->beginBindingBank(32);
                shaderSignatureBuilder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE, 1);
                mipmapComputationShaderSignature = shaderSignatureBuilder->build();
                if (!mipmapComputationShaderSignature)
                    return nullptr;

                auto vertexShader = compileShaderFromFile(device->hasTopLeftNdcOrigin() == device->hasBottomLeftTextureCoordinates() ? "data/shaders/screenQuadFlippedY.glsl" :
                    "data/shaders/screenQuad.glsl", AGPU_VERTEX_SHADER);
                if (!vertexShader)
                    return nullptr;

                auto fragmentShader = compileShaderFromFile("data/shaders/mipmapScreenQuad.glsl", AGPU_FRAGMENT_SHADER);
                if (!fragmentShader)
                    return nullptr;

                {
                    auto pipelineBuilder = device->createPipelineBuilder();
                    pipelineBuilder->setShaderSignature(mipmapComputationShaderSignature);
                    pipelineBuilder->setRenderTargetFormat(0, AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM_SRGB);
                    pipelineBuilder->setDepthStencilFormat(AGPU_TEXTURE_FORMAT_UNKNOWN);
                    pipelineBuilder->attachShader(vertexShader);
                    pipelineBuilder->attachShader(fragmentShader);
                    pipelineBuilder->setPrimitiveType(AGPU_TRIANGLES);
                    mipmapComputationColorPipelineState = pipelineBuilder->build();
                    if(!mipmapComputationColorPipelineState)
                        return nullptr;

                    agpu_renderpass_color_attachment_description colorAttachment = {};
                    colorAttachment.format = AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM_SRGB;
                    colorAttachment.begin_action = AGPU_ATTACHMENT_DISCARD;
                    colorAttachment.end_action = AGPU_ATTACHMENT_KEEP;
                    colorAttachment.sample_count = 1;

                    agpu_renderpass_description description = {};
                    description.color_attachment_count = 1;
                    description.color_attachments = &colorAttachment;

                    mipmapComputationColorRenderpass = device->createRenderPass(&description);
                    if(!mipmapComputationColorRenderpass)
                        return nullptr;
                }

                {
                    auto pipelineBuilder = device->createPipelineBuilder();
                    pipelineBuilder->setShaderSignature(mipmapComputationShaderSignature);
                    pipelineBuilder->setRenderTargetFormat(0, AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM);
                    pipelineBuilder->setDepthStencilFormat(AGPU_TEXTURE_FORMAT_UNKNOWN);
                    pipelineBuilder->attachShader(vertexShader);
                    pipelineBuilder->attachShader(fragmentShader);
                    pipelineBuilder->setPrimitiveType(AGPU_TRIANGLES);
                    mipmapComputationNonColorPipelineState = pipelineBuilder->build();
                    if(!mipmapComputationNonColorPipelineState)
                        return nullptr;

                    agpu_renderpass_color_attachment_description colorAttachment = {};
                    colorAttachment.format = AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM;
                    colorAttachment.begin_action = AGPU_ATTACHMENT_DISCARD;
                    colorAttachment.end_action = AGPU_ATTACHMENT_KEEP;
                    colorAttachment.sample_count = 1;

                    agpu_renderpass_description description = {};
                    description.color_attachment_count = 1;
                    description.color_attachments = &colorAttachment;

                    mipmapComputationNonColorRenderpass = device->createRenderPass(&description);
                    if(!mipmapComputationNonColorRenderpass)
                        return nullptr;
                }

                {
                    agpu_sampler_description samplerDesc = {};
                    samplerDesc.filter = AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST;
                    samplerDesc.address_u = AGPU_TEXTURE_ADDRESS_MODE_WRAP;
                    samplerDesc.address_v = AGPU_TEXTURE_ADDRESS_MODE_WRAP;
                    samplerDesc.address_w = AGPU_TEXTURE_ADDRESS_MODE_WRAP;
                    samplerDesc.max_lod = 0;
                    mipmapComputationSampler = device->createSampler(&samplerDesc);
                    if(!mipmapComputationSampler)
                        return nullptr;

                    mipmapComputationSamplerBinding = mipmapComputationShaderSignature->createShaderResourceBinding(0);
                    mipmapComputationSamplerBinding->bindSampler(0, mipmapComputationSampler);
                }
            }

            auto pipeline = nonColorData ? mipmapComputationNonColorPipelineState : mipmapComputationColorPipelineState;
            auto renderpass = nonColorData ? mipmapComputationNonColorRenderpass : mipmapComputationColorRenderpass;

            agpu_texture_view_description viewDescription = {};
            texture->getFullViewDescription(&viewDescription);
            viewDescription.subresource_range.level_count = 1;

            auto currentWidth = width;
            auto currentHeight = height;
            commandList->setShaderSignature(mipmapComputationShaderSignature);

            while(currentWidth > 1 || currentHeight > 1)
            {
                auto nextWidth = std::max(currentWidth / 2, size_t(1));
                auto nextHeight = std::max(currentHeight / 2, size_t(1));

                auto sourceView = texture->createView(&viewDescription);
                textureViews.push_back(sourceView);
                if(!sourceView)
                    return nullptr;

                ++viewDescription.subresource_range.base_miplevel;
                auto destViewDescription = viewDescription;
                destViewDescription.usage_mode = AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT;

                auto destView = texture->createView(&destViewDescription);
                textureViews.push_back(destView);
                if(!destView)
                    return nullptr;

                auto framebuffer = device->createFrameBuffer(agpu_uint(nextWidth), agpu_uint(nextHeight), 1, &destView, nullptr);
                framebuffers.push_back(framebuffer);
                if(!framebuffer)
                    return nullptr;

                auto binding = mipmapComputationShaderSignature->createShaderResourceBinding(1);
                resourceBindings.push_back(binding);
                if(!binding)
                    return nullptr;

                binding->bindSampledTextureView(0, sourceView);

                commandList->beginRenderPass(renderpass, framebuffer, false);
                commandList->setViewport(0, 0, agpu_uint(nextWidth), agpu_uint(nextHeight));
                commandList->setScissor(0, 0, agpu_uint(nextWidth), agpu_uint(nextHeight));
                commandList->usePipelineState(pipeline);
                commandList->useShaderResources(mipmapComputationSamplerBinding);
                commandList->useShaderResources(binding);
                commandList->drawArrays(3, 1, 0, 0);
                commandList->endRenderPass();

                currentWidth = nextWidth;
                currentHeight = nextHeight;
            }
        }

        commandList->close();
        auto commandQueue = device->getDefaultCommandQueue();
        commandQueue->addCommandList(commandList);
        commandQueue->finishExecution();
    }
    else
    {
        texture->uploadTextureData(0, 0, convertedSurface->pitch, convertedSurface->pitch*convertedSurface->h, convertedSurface->pixels);
        SDL_FreeSurface(convertedSurface);
    }

    return texture;
}

const agpu_vertex_layout_ref &AbstractSampleBase::getSampleVertexLayout()
{
    if(!sampleVertexLayout)
    {
        // Create the vertex layout.
        sampleVertexLayout = device->createVertexLayout();
        agpu_size vertexStride = sizeof(SampleVertex);
        sampleVertexLayout->addVertexAttributeBindings(1, &vertexStride, SampleVertex::DescriptionSize, SampleVertex::Description);
    }

    return sampleVertexLayout;
}


int SampleBase::main(int argc, const char **argv)
{
    bool vsyncDisabled = false;
    bool debugLayerEnabled = false;
#ifdef _DEBUG
    debugLayerEnabled= true;
#endif
    agpu_uint platformIndex = 0;
    agpu_uint gpuIndex = 0;
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-no-vsync")
        {
            vsyncDisabled = true;
        }
        else if (arg == "-platform")
        {
            platformIndex = agpu_uint(atoi(argv[++i]));
        }
        else if (arg == "-gpu")
        {
            gpuIndex = agpu_uint(atoi(argv[++i]));
        }
        else if (arg == "-debug")
        {
            debugLayerEnabled = true;
        }
    }

    char nameBuffer[256];
    SDL_Init(SDL_INIT_VIDEO);

    screenWidth = 640;
    screenHeight = 480;

    int flags = SDL_WINDOW_RESIZABLE;

    // Get the platform.
    agpu_uint numPlatforms;
    agpuGetPlatforms(0, nullptr, &numPlatforms);
    if (numPlatforms == 0)
    {
        printError("No agpu platforms are available.\n");
        return 1;
    }
    else if (platformIndex >= numPlatforms)
    {
        printError("Selected platform index is not available.\n");
        return 1;
    }

    std::vector<agpu_platform*> platforms;
    platforms.resize(numPlatforms);
    agpuGetPlatforms(numPlatforms, &platforms[0], nullptr);
    auto platform = platforms[platformIndex];

    printMessage("Choosen platform: %s\n", agpuGetPlatformName(platform));
    window = SDL_CreateWindow("Agpu Sample", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, flags);
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
    agpu_device_open_info openInfo = {};
    openInfo.gpu_index = gpuIndex;
    openInfo.debug_layer = debugLayerEnabled;
    memset(&currentSwapChainCreateInfo, 0, sizeof(currentSwapChainCreateInfo));
    switch(windowInfo.subsystem)
    {
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
    case SDL_SYSWM_WINDOWS:
        currentSwapChainCreateInfo.window = (agpu_pointer)windowInfo.info.win.window;
        break;
#endif
#if defined(SDL_VIDEO_DRIVER_X11)
    case SDL_SYSWM_X11:
        openInfo.display = (agpu_pointer)windowInfo.info.x11.display;
        currentSwapChainCreateInfo.window = (agpu_pointer)(uintptr_t)windowInfo.info.x11.window;
        break;
#endif
#if defined(SDL_VIDEO_DRIVER_COCOA)
    case SDL_SYSWM_COCOA:
        currentSwapChainCreateInfo.window = (agpu_pointer)windowInfo.info.cocoa.window;
        break;
#endif
    default:
        printError("Unsupported window system\n");
        return -1;
    }

    currentSwapChainCreateInfo.colorbuffer_format = ColorBufferFormat;
    currentSwapChainCreateInfo.depth_stencil_format = DepthStencilBufferFormat;
    currentSwapChainCreateInfo.width = screenWidth;
    currentSwapChainCreateInfo.height = screenHeight;
    currentSwapChainCreateInfo.buffer_count = 3;
    currentSwapChainCreateInfo.flags = AGPU_SWAP_CHAIN_FLAG_APPLY_SCALE_FACTOR_FOR_HI_DPI;
    if (UseOverlayWindow)
        currentSwapChainCreateInfo.flags = agpu_swap_chain_flags(currentSwapChainCreateInfo.flags | AGPU_SWAP_CHAIN_FLAG_OVERLAY_WINDOW);
    if (vsyncDisabled)
    {
        currentSwapChainCreateInfo.presentation_mode = AGPU_SWAP_CHAIN_PRESENTATION_MODE_MAILBOX;
        currentSwapChainCreateInfo.fallback_presentation_mode = AGPU_SWAP_CHAIN_PRESENTATION_MODE_IMMEDIATE;
    }

    device = platform->openDevice(&openInfo);
    if(!device)
    {
        printError("Failed to open the device\n");
        return false;
    }

    snprintf(nameBuffer, sizeof(nameBuffer), "AGPU Sample - [%s] %s", agpuGetPlatformName(platform), device->getName());
    SDL_SetWindowTitle(window, nameBuffer);


	hasPersistentCoherentMapping = device->isFeatureSupported(AGPU_FEATURE_PERSISTENT_COHERENT_MEMORY_MAPPING);
    useComputeShadersForMipmapGeneration = false && device->isFeatureSupported(AGPU_FEATURE_COMPUTE_SHADER);

    // Get the default command queue
    commandQueue = device->getDefaultCommandQueue();

    // Create the swap chain.
    swapChain = device->createSwapChain(commandQueue, &currentSwapChainCreateInfo);
    if(!swapChain)
    {
        printError("Failed to create the swap chain\n");
        return false;
    }

    screenWidth = swapChain->getWidth();
    screenHeight = swapChain->getHeight();

    // Get the preferred shader language.
    preferredShaderLanguage = device->getPreferredIntermediateShaderLanguage();
    if(preferredShaderLanguage == AGPU_SHADER_LANGUAGE_NONE)
    {
        preferredShaderLanguage = device->getPreferredHighLevelShaderLanguage();
        if(preferredShaderLanguage == AGPU_SHADER_LANGUAGE_NONE)
            preferredShaderLanguage = device->getPreferredShaderLanguage();
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
    swapChain.reset();
    commandQueue.reset();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


void SampleBase::update(float deltaTime)
{
}

void SampleBase::processEvents()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_MOUSEBUTTONDOWN:
            onMouseButtonDown(event.button);
            break;
        case SDL_MOUSEBUTTONUP:
            onMouseButtonUp(event.button);
            break;
        case SDL_MOUSEMOTION:
            onMouseMotion(event.motion);
            break;
        case SDL_MOUSEWHEEL:
            onMouseWheel(event.wheel);
            break;
        case SDL_KEYDOWN:
            onKeyDown(event.key);
            break;
        case SDL_KEYUP:
            onKeyUp(event.key);
            break;
        case SDL_QUIT:
            quit = true;
            break;
        case SDL_WINDOWEVENT:
            {
                switch(event.window.event)
                {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    recreateSwapChain();
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            //ignore
            break;
        }
    }
}

void SampleBase::onMouseButtonDown(const SDL_MouseButtonEvent &event)
{
}

void SampleBase::onMouseButtonUp(const SDL_MouseButtonEvent &event)
{
}

void SampleBase::onMouseMotion(const SDL_MouseMotionEvent &event)
{
}

void SampleBase::onMouseWheel(const SDL_MouseWheelEvent &event)
{
}

void SampleBase::onKeyDown(const SDL_KeyboardEvent &event)
{
    switch(event.keysym.sym)
    {
    case SDLK_ESCAPE:
        quit = true;
        break;
    case SDLK_f:
        toggleFullscren();
        break;
    default:
        // ignore
        break;
    }
}

void SampleBase::onKeyUp(const SDL_KeyboardEvent &event)
{
}

bool SampleBase::initializeSample()
{
    return true;
}

void SampleBase::shutdownSample()
{
}

void SampleBase::toggleFullscren()
{
    auto isFullscreen = (SDL_GetWindowFlags(window) & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) != 0;
    SDL_SetWindowFullscreen(window, isFullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
    recreateSwapChain();
}

void SampleBase::recreateSwapChain()
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    device->finishExecution();
    auto newSwapChainCreateInfo = currentSwapChainCreateInfo;
    newSwapChainCreateInfo.width = w;
    newSwapChainCreateInfo.height = h;
    newSwapChainCreateInfo.old_swap_chain = swapChain.get();
    swapChain = device->createSwapChain(commandQueue, &newSwapChainCreateInfo);

    screenWidth = swapChain->getWidth();
    screenHeight = swapChain->getHeight();
}

void SampleBase::render()
{
}

void SampleBase::swapBuffers()
{
    try
    {
        swapChain->swapBuffers();
    }
    catch(agpu_exception &e)
    {
        auto errorCode = e.getErrorCode();
        if(errorCode == AGPU_OUT_OF_DATE)
        {
            // We must recreate the swap chain.
            recreateSwapChain();
        }
        else if(errorCode == AGPU_SUBOPTIMAL)
        {
            // Ignore this case.
        }
        else
        {
            throw e;
        }
    }
}

agpu_renderpass_ref SampleBase::createMainPass(const glm::vec4 &clearColor)
{
    // Color attachment
    agpu_renderpass_color_attachment_description colorAttachment = {};
    colorAttachment.format = ColorBufferFormat;
    colorAttachment.begin_action = AGPU_ATTACHMENT_CLEAR;
    colorAttachment.end_action = AGPU_ATTACHMENT_KEEP;
    colorAttachment.clear_value.r = clearColor.r;
    colorAttachment.clear_value.g = clearColor.g;
    colorAttachment.clear_value.b = clearColor.b;
    colorAttachment.clear_value.a = clearColor.a;
    colorAttachment.sample_count = 1;

    // Depth stencil
    agpu_renderpass_depth_stencil_description depthStencil = {};
    depthStencil.format = AGPU_TEXTURE_FORMAT_D32_FLOAT_S8X24_UINT;
    depthStencil.begin_action = AGPU_ATTACHMENT_CLEAR;
    depthStencil.end_action = AGPU_ATTACHMENT_KEEP;
    depthStencil.clear_value.depth = 1.0;
    depthStencil.sample_count = 1;

    agpu_renderpass_description description = {};
    description.color_attachment_count = 1;
    description.color_attachments = &colorAttachment;
    description.depth_stencil_attachment = &depthStencil;

    agpu_renderpass_ref result = device->createRenderPass(&description);
    return result;
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

    device = platform->openDevice(&openInfo);
    if(!device)
    {
        printError("Failed to open the device\n");
        return false;
    }

	hasPersistentCoherentMapping = device->isFeatureSupported(AGPU_FEATURE_PERSISTENT_COHERENT_MEMORY_MAPPING);

    // Get the default command queue
    commandQueue = device->getDefaultCommandQueue();

    // Get the preferred shader language.
    preferredShaderLanguage = device->getPreferredIntermediateShaderLanguage();
    if(preferredShaderLanguage == AGPU_SHADER_LANGUAGE_NONE)
    {
        preferredShaderLanguage = device->getPreferredHighLevelShaderLanguage();
        if(preferredShaderLanguage == AGPU_SHADER_LANGUAGE_NONE)
            preferredShaderLanguage = device->getPreferredShaderLanguage();
    }

    return run(argc, argv);
}

int ComputeSampleBase::run(int argc, const char **argv)
{
    return 0;
}
