#include "texture.hpp"
#include "texture_view.hpp"
#include "texture_format.hpp"
#include "command_queue.hpp"
#include "constants.hpp"
#include "../Common/memory_profiler.hpp"

namespace AgpuMetal
{
    
AMtlTexture::AMtlTexture(const agpu::device_ref &device)
    : device(device)
{
    AgpuProfileConstructor(AMtlTexture);
}

AMtlTexture::~AMtlTexture()
{
    AgpuProfileDestructor(AMtlTexture);
}

agpu::texture_ref AMtlTexture::create(const agpu::device_ref &device, agpu_texture_description* description)
{
    if(!description)
        return agpu::texture_ref();

    auto descriptor = [MTLTextureDescriptor new];
    bool isArray = description->layers > 1;

    descriptor.arrayLength = description->layers;
    switch(description->type)
    {
    case AGPU_TEXTURE_1D:
        descriptor.textureType = isArray ? MTLTextureType1DArray : MTLTextureType1D;
        break;
    case AGPU_TEXTURE_2D:
        if(description->sample_count > 1)
            descriptor.textureType = isArray ? MTLTextureType2DMultisampleArray : MTLTextureType2DMultisample;
        else
            descriptor.textureType = isArray ? MTLTextureType2DArray : MTLTextureType2D;
        
        break;
    case AGPU_TEXTURE_CUBE:
        descriptor.textureType = isArray ? MTLTextureTypeCubeArray : MTLTextureTypeCube;
        break;
    case AGPU_TEXTURE_3D:
        descriptor.textureType = MTLTextureType3D;
        descriptor.arrayLength = 1;
        descriptor.depth = description->depth;
        break;
    default:
        return agpu::texture_ref();
    }

    descriptor.pixelFormat = mapTextureFormat(description->format);
    descriptor.width = description->width;
    descriptor.height = description->height;
    descriptor.mipmapLevelCount = description->miplevels;
    descriptor.sampleCount = std::max(description->sample_count, 1u);
    descriptor.storageMode = mapTextureStorageMode(description->heap_type); // For upload texture.

    auto usageModes = description->usage_modes;
    agpu_texture_aspect aspect = AGPU_TEXTURE_ASPECT_COLOR;
    if (usageModes & (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT))
    {
        descriptor.usage = MTLTextureUsageRenderTarget;
        aspect = agpu_texture_aspect(0);
        if(usageModes & AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT)
            aspect = AGPU_TEXTURE_ASPECT_DEPTH;
        if(usageModes & AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT)
            aspect = agpu_texture_aspect(aspect | AGPU_TEXTURE_ASPECT_STENCIL);
    }
    else if (usageModes & AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT)
    {
        descriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
    }
    else if(usageModes & AGPU_TEXTURE_USAGE_STORAGE)
    {
        descriptor.usage = MTLTextureUsageShaderRead | MTLTextureUsageShaderWrite;
    }
    else
    {
        descriptor.usage = MTLTextureUsageShaderRead;
    }

    if((usageModes & (AGPU_TEXTURE_USAGE_COPY_SOURCE | AGPU_TEXTURE_USAGE_COPY_DESTINATION)) != 0
        && isSRGBTextureFormat(description->format))
    {
        descriptor.usage |= MTLTextureUsagePixelFormatView;
    }

    if (usageModes & AGPU_TEXTURE_USAGE_SAMPLED)
        descriptor.usage |= MTLTextureUsageShaderRead;

    auto handle = [deviceForMetal->device newTextureWithDescriptor: descriptor];
    if(!handle)
        return agpu::texture_ref();

    // We need a common format for texture copy commands.
    id<MTLTexture> linearViewHandle = nil;
    if(usageModes & (AGPU_TEXTURE_USAGE_COPY_SOURCE | AGPU_TEXTURE_USAGE_COPY_DESTINATION))
    {
        if(isSRGBTextureFormat(description->format))
        {
            auto linearPixelFormat = mapTextureFormat(srgbTextureFormatToLinear(description->format));
            linearViewHandle = [handle newTextureViewWithPixelFormat: linearPixelFormat];
            if(!linearViewHandle)
                return agpu::texture_ref();
        }
        else
        {
            linearViewHandle = handle;
        }

    }

    auto result = agpu::makeObject<AMtlTexture> (device);
    auto amtlTexture = result.as<AMtlTexture> ();
    amtlTexture->description = *description;
    amtlTexture->handle = handle;
    amtlTexture->linearViewHandle = linearViewHandle;
    amtlTexture->aspect = aspect;
    return result;
}

agpu_error AMtlTexture::getDescription(agpu_texture_description* description)
{
    CHECK_POINTER(description);
    *description = this->description;
    return AGPU_OK;
}

agpu_pointer AMtlTexture::mapLevel ( agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags, agpu_region3d *region )
{
    // TODO: Implement this, if possible
    return nullptr;
}

agpu_error AMtlTexture::unmapLevel (  )
{
    return AGPU_UNSUPPORTED;
}

agpu_error AMtlTexture::readTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer)
{
    return readTextureSubData(level, arrayIndex, pitch, slicePitch, nullptr, nullptr, buffer);
}

agpu_error AMtlTexture::readTextureSubData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_region3d* sourceRegion, agpu_size3d* destSize, agpu_pointer buffer)
{
    CHECK_POINTER(buffer);
    if ((description.usage_modes & AGPU_TEXTURE_USAGE_READED_BACK) == 0)
    {
        return AGPU_INVALID_OPERATION;
    }

    // TODO: Compute properly the region and the transfer size.
    MTLRegion region = getLevelRegion(level);
    
    size_t transferSize = slicePitch*region.size.depth;
    if(transferSize == 0)
        return AGPU_OK;

    if(description.heap_type != AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL)
    {
#if TARGET_OS_OSX
        // Synchronize this with the GPU when using a managed texture.
        // TODO: Avoid this when is not needed.
        @autoreleasepool {
            id<MTLCommandBuffer> commandBuffer = [deviceForMetal->getDefaultCommandQueueHandle() commandBuffer];
            id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];
            [blitEncoder synchronizeResource: handle];
            [blitEncoder endEncoding];
            [commandBuffer commit];
            [commandBuffer waitUntilCompleted];
        }
#endif

        [handle getBytes: buffer bytesPerRow: pitch bytesPerImage: slicePitch 
            fromRegion: region mipmapLevel: level slice: arrayIndex];
        return AGPU_OK;
    }
    
    agpu_error resultCode = AGPU_ERROR;
    deviceForMetal->withReadbackCommandListDo(transferSize, 1, [&](AMtlImplicitResourceReadbackCommandList &readbackList) {
        if(readbackList.currentStagingBufferSize < transferSize)
        {
            resultCode = AGPU_OUT_OF_MEMORY;
            return;
        }

        // Copy the image data into staging buffer.
        auto success = readbackList.setupCommandBuffer() &&
            readbackList.readbackImageDataToBuffer(handle, level, arrayIndex, region, pitch, slicePitch) &&
            readbackList.submitCommandBuffer();

        if(success)
        {
            auto readbackPointer = readbackList.currentStagingBufferPointer;
            memcpy(buffer, readbackPointer, slicePitch*region.size.depth);
        }
        resultCode = success ? AGPU_OK : AGPU_ERROR;
    });

    return resultCode;
}

agpu_error AMtlTexture::uploadTextureData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data )
{
    return uploadTextureSubData(level, arrayIndex, pitch, slicePitch, nullptr, nullptr, data);
}

agpu_error AMtlTexture::uploadTextureSubData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_size3d* sourceSize, agpu_region3d* destRegion, agpu_pointer data )
{
    CHECK_POINTER(data);

    // TODO: Compute properly the region and the transfer size.
    MTLRegion region = getLevelRegion(level);
    
    size_t transferSize = slicePitch*region.size.depth;
    if(transferSize == 0)
        return AGPU_OK;

    if(description.heap_type != AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL)
    {
        [handle replaceRegion:(MTLRegion)region mipmapLevel: level slice: arrayIndex
                withBytes: data bytesPerRow: pitch bytesPerImage: slicePitch];
        return AGPU_OK;
    }
        
    agpu_error resultCode = AGPU_OK;
    deviceForMetal->withUploadCommandListDo(transferSize, 1, [&](AMtlImplicitResourceUploadCommandList &uploadList) {
        if(uploadList.currentStagingBufferSize < transferSize)
        {
            resultCode = AGPU_OUT_OF_MEMORY;
            return;
        }

        // Copy the image data into the staging buffer.
        auto bufferPointer = uploadList.currentStagingBufferPointer;
        memcpy(bufferPointer, data, slicePitch*region.size.depth);

        auto success = uploadList.setupCommandBuffer() &&
            uploadList.uploadBufferDataToImage(handle, level, arrayIndex, region, pitch, slicePitch) &&
            uploadList.submitCommandBuffer();
        resultCode = success ? AGPU_OK : AGPU_ERROR;
    });

    return resultCode;
}

agpu_error AMtlTexture::getFullViewDescription ( agpu_texture_view_description* viewDescription )
{
    CHECK_POINTER(viewDescription);
    memset(viewDescription, 0, sizeof(*viewDescription));
    viewDescription->type = description.type;
    viewDescription->format = description.format;
    viewDescription->sample_count = description.sample_count;
    viewDescription->usage_mode = description.main_usage_mode;
    viewDescription->components.r = AGPU_COMPONENT_SWIZZLE_R;
    viewDescription->components.g = AGPU_COMPONENT_SWIZZLE_G;
    viewDescription->components.b = AGPU_COMPONENT_SWIZZLE_B;
    viewDescription->components.a = AGPU_COMPONENT_SWIZZLE_A;
    viewDescription->subresource_range.aspect = aspect;
    viewDescription->subresource_range.base_miplevel = 0;
    viewDescription->subresource_range.level_count = description.miplevels;
    viewDescription->subresource_range.base_arraylayer = 0;
    viewDescription->subresource_range.layer_count = description.layers;
    
    return AGPU_OK;
}

agpu::texture_view_ptr AMtlTexture::createView(agpu_texture_view_description* description)
{
    return AMtlTextureView::create(device, refFromThis<agpu::texture> (), description).disown();
}

agpu::texture_view_ptr AMtlTexture::getOrCreateFullView()
{
    if(!fullTextureView)
    {
        agpu_texture_view_description fullViewDescription;
        getFullViewDescription(&fullViewDescription);
        fullTextureView = agpu::texture_view_ref(createView(&fullViewDescription));
    }
    
    return fullTextureView.disownedNewRef();
}

} // End of namespace AgpuMetal
