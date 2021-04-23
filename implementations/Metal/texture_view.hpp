#ifndef AGPU_TEXTURE_VIEW_HPP
#define AGPU_TEXTURE_VIEW_HPP

#include "device.hpp"

namespace AgpuMetal
{

class AMtlTextureView : public agpu::texture_view
{
public:    
    AMtlTextureView(const agpu::device_ref &device, const agpu::texture_ref &texture, const agpu_texture_view_description &description,  id<MTLTexture> handle);
    ~AMtlTextureView();
    
    static agpu::texture_view_ref create(const agpu::device_ref &device, const agpu::texture_ref &texture, agpu_texture_view_description *description);
    
	virtual agpu::texture_ptr getTexture() override;
    
    void activateOnRenderEncoder(id<MTLRenderCommandEncoder> encoder, agpu_uint slot);
    void activateOnComputeEncoder(id<MTLComputeCommandEncoder> encoder, agpu_uint slot);
    
    agpu::device_ref device;
    agpu::texture_weakref texture;
    agpu_texture_view_description description;
    id<MTLTexture> handle;
};

} // End of namespace AgpuMetal

#endif //AGPU_TEXTURE_VIEW_HPP
