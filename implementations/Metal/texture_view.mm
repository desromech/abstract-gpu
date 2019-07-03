#include "texture_view.hpp"
#include "texture.hpp"

namespace AgpuMetal
{
AMtlTextureView::AMtlTextureView(const agpu::device_ref &device, const agpu::texture_ref &texture, const agpu_texture_view_description &description)
    : device(device), texture(texture), description(description)
{
}

AMtlTextureView::~AMtlTextureView()
{
    
}

agpu::texture_view_ref AMtlTextureView::create(const agpu::device_ref &device, const agpu::texture_ref &texture, agpu_texture_view_description *description)
{
    if(!description)
        return agpu::texture_view_ref();
    return agpu::makeObject<AMtlTextureView> (device, texture, *description);
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
    
    auto handle = mtlTexture.as<AMtlTexture> ()->handle;
    [encoder setVertexTexture: handle atIndex: slot];
    [encoder setFragmentTexture: handle atIndex: slot];
}

void AMtlTextureView::activateOnComputeEncoder(id<MTLComputeCommandEncoder> encoder, agpu_uint slot)
{
    auto mtlTexture = texture.lock();
    if(!mtlTexture)
        return;
    
    auto handle = mtlTexture.as<AMtlTexture> ()->handle;
    [encoder setTexture: handle atIndex: slot];
}

} // End of namespace AgpuMetal
