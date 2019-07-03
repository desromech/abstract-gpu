#include "texture.hpp"
#include "texture_view.hpp"
#include "texture_format.hpp"
#include "command_queue.hpp"

namespace AgpuMetal
{
    
AMtlTexture::AMtlTexture(const agpu::device_ref &device)
    : device(device)
{
    handle = nil;
}

AMtlTexture::~AMtlTexture()
{
    if(handle)
        [handle release];
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
        descriptor.textureType = isArray ? MTLTextureType2DArray : MTLTextureType2D;
        break;
    case AGPU_TEXTURE_CUBE:
        descriptor.textureType = MTLTextureTypeCube;
        descriptor.arrayLength = 1;
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
    descriptor.storageMode = MTLStorageModeManaged; // For upload texture.
    if(description->sample_count > 1)
    {
        descriptor.storageMode = MTLStorageModePrivate;
    }    

    auto usageModes = description->usage_modes;
    if (usageModes & (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT))
    {
        descriptor.usage = MTLTextureUsageRenderTarget;
        descriptor.storageMode = MTLStorageModePrivate;
    }
    else if (usageModes & AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT)
    {
        descriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
    }
    else
    {
        descriptor.usage = MTLTextureUsageShaderRead;
    }

    if (usageModes & AGPU_TEXTURE_USAGE_SAMPLED)
        descriptor.usage |= MTLTextureUsageShaderRead;

    auto handle = [deviceForMetal->device newTextureWithDescriptor: descriptor];
    [descriptor release];
    if(!handle)
        return agpu::texture_ref();

    auto result = agpu::makeObject<AMtlTexture> (device);
    auto amtlTexture = result.as<AMtlTexture> ();
    amtlTexture->description = *description;
    amtlTexture->handle = handle;
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

agpu_error AMtlTexture::readTextureData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer )
{
    CHECK_POINTER(buffer);

    // Synchronize this with the GPU
    // TODO: Avoid this when is not needed.
    id<MTLCommandBuffer> commandBuffer = [deviceForMetal->getDefaultCommandQueueHandle() commandBuffer];
    id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];
    [blitEncoder synchronizeResource: handle];
    [blitEncoder endEncoding];
    [commandBuffer commit];
    [commandBuffer waitUntilCompleted];
    [commandBuffer release];

    MTLRegion region = getLevelRegion(level);
    [handle getBytes: buffer
     bytesPerRow: pitch
   bytesPerImage: slicePitch
      fromRegion: region
     mipmapLevel: level
           slice: arrayIndex];
    return AGPU_OK;
}

agpu_error AMtlTexture::uploadTextureData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data )
{
    CHECK_POINTER(data);

    MTLRegion region = getLevelRegion(level);

    [handle replaceRegion:(MTLRegion)region
          mipmapLevel: level
                slice: arrayIndex
            withBytes: data
          bytesPerRow: pitch
        bytesPerImage: slicePitch];
    return AGPU_OK;
}

agpu_error AMtlTexture::uploadTextureSubData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_size3d* sourceSize, agpu_region3d* destRegion, agpu_pointer data )
{
    CHECK_POINTER(data);

    // TODO: Implement this properly.
    MTLRegion region = getLevelRegion(level);

    [handle replaceRegion:(MTLRegion)region
          mipmapLevel: level
                slice: arrayIndex
            withBytes: data
          bytesPerRow: pitch
        bytesPerImage: slicePitch];
    return AGPU_OK;
}

agpu_error AMtlTexture::discardUploadBuffer (  )
{
    return AGPU_OK;
}

agpu_error AMtlTexture::discardReadbackBuffer (  )
{
    return AGPU_OK;
}

agpu_error AMtlTexture::getFullViewDescription ( agpu_texture_view_description* viewDescription )
{
    CHECK_POINTER(viewDescription);
    memset(viewDescription, 0, sizeof(*viewDescription));
    viewDescription->type = description.type;
    viewDescription->format = description.format;
    viewDescription->components.r = AGPU_COMPONENT_SWIZZLE_R;
    viewDescription->components.g = AGPU_COMPONENT_SWIZZLE_G;
    viewDescription->components.b = AGPU_COMPONENT_SWIZZLE_B;
    viewDescription->components.a = AGPU_COMPONENT_SWIZZLE_A;
    viewDescription->subresource_range.usage_mode = description.usage_modes;
    viewDescription->subresource_range.base_miplevel = 0;
    viewDescription->subresource_range.level_count = description.miplevels;
    viewDescription->subresource_range.base_arraylayer = 0;
    viewDescription->subresource_range.layer_count = description.layers;
    if(viewDescription->subresource_range.layer_count == 1)
        viewDescription->subresource_range.layer_count = 0;

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
