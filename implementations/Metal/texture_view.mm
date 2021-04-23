#include "texture_view.hpp"
#include "texture.hpp"
#include "texture_format.hpp"
#include "constants.hpp"

namespace AgpuMetal
{
AMtlTextureView::AMtlTextureView(const agpu::device_ref &device, const agpu::texture_ref &texture, const agpu_texture_view_description &description, id<MTLTexture> handle)
    : device(device), texture(texture), description(description), handle(handle)
{
}

AMtlTextureView::~AMtlTextureView()
{
    if(handle)
        [handle release];
}

agpu::texture_view_ref AMtlTextureView::create(const agpu::device_ref &device, const agpu::texture_ref &texture, agpu_texture_view_description *description)
{
    if (!description) return agpu::texture_view_ref();
    if (description->type == AGPU_TEXTURE_CUBE && description->subresource_range.layer_count % 6 != 0) return agpu::texture_view_ref();
        
    auto amtlTexture = texture.as<AMtlTexture> ();
    
    id<MTLTexture> handle = amtlTexture->handle;
    auto &range = description->subresource_range;
    bool isFullTexture =
        range.base_miplevel == 0
        && range.base_arraylayer == 0
        && range.aspect == amtlTexture->aspect
        && range.level_count >= amtlTexture->description.miplevels
        && range.layer_count >= amtlTexture->description.layers;
   
   if(!isFullTexture
        && (description->usage_mode & (AGPU_TEXTURE_USAGE_SAMPLED | AGPU_TEXTURE_USAGE_STORAGE)) != 0)
    {
        auto pixelFormat = mapTextureFormat(description->format);
        auto isArray = range.layer_count > 1;
        auto levelRange = NSMakeRange(description->subresource_range.base_miplevel, description->subresource_range.level_count);
        auto layerRange = NSMakeRange(description->subresource_range.base_arraylayer, description->subresource_range.layer_count);
        MTLTextureType textureType;
        switch(description->type)
        {
        case AGPU_TEXTURE_1D:
            textureType = isArray ? MTLTextureType1DArray : MTLTextureType1D;
            break;
        case AGPU_TEXTURE_2D:
            if(description->sample_count > 1)
                textureType = isArray ? MTLTextureType2DMultisampleArray : MTLTextureType2DMultisample;
            else
                textureType = isArray ? MTLTextureType2DArray : MTLTextureType2D;
            
            break;
        case AGPU_TEXTURE_CUBE:
            isArray = range.layer_count > 6;
            textureType = isArray ? MTLTextureTypeCube : MTLTextureTypeCubeArray;
            layerRange.length = range.layer_count / 6;
            break;
        case AGPU_TEXTURE_3D:
            textureType = MTLTextureType3D;
            layerRange.length = 1;
            break;
        default:
            return agpu::texture_view_ref();
        }

        handle = [amtlTexture->handle newTextureViewWithPixelFormat: pixelFormat
                                    textureType: textureType 
                                         levels: levelRange 
                                         slices: layerRange];
        if(!handle)
            return agpu::texture_view_ref();
    }
    else
    {
        [handle retain];
    }

    return agpu::makeObject<AMtlTextureView> (device, texture, *description, handle);
}

agpu::texture_ptr AMtlTextureView::getTexture()
{
    return texture.lock().disown();
}

void AMtlTextureView::activateOnRenderEncoder(id<MTLRenderCommandEncoder> encoder, agpu_uint slot)
{
    auto mtlTexture = texture.lock();
    if(!mtlTexture)
        return;
    
    [encoder setVertexTexture: handle atIndex: slot];
    [encoder setFragmentTexture: handle atIndex: slot];
}

void AMtlTextureView::activateOnComputeEncoder(id<MTLComputeCommandEncoder> encoder, agpu_uint slot)
{
    auto mtlTexture = texture.lock();
    if(!mtlTexture)
        return;
    
    [encoder setTexture: handle atIndex: slot];
}

} // End of namespace AgpuMetal
