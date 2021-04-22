#ifndef AGPU_D3D12_TEXTURE_VIEW_HPP
#define AGPU_D3D12_TEXTURE_VIEW_HPP

#include "device.hpp"

namespace AgpuD3D12
{

class ADXTextureView : public agpu::texture_view
{
public:
    ADXTextureView(const agpu::device_ref &cdevice, const agpu::texture_ref &ctexture, const agpu_texture_view_description &cdescription, UINT cshader4ComponentMapping);
    ~ADXTextureView();

    static agpu::texture_view_ref create(const agpu::device_ref &device, const agpu::texture_ref &texture, const agpu_texture_view_description &description);

    virtual agpu::texture_ptr getTexture() override;
	agpu_error getSampledTextureViewDescription(D3D12_SHADER_RESOURCE_VIEW_DESC* out);
    agpu_error getUnorderedAccessTextureViewDescription(D3D12_UNORDERED_ACCESS_VIEW_DESC* out);
	agpu_error getColorAttachmentViewDescription(D3D12_RENDER_TARGET_VIEW_DESC *out);
    agpu_error getDepthStencilViewDescription(D3D12_DEPTH_STENCIL_VIEW_DESC *out);

    agpu::device_ref device;
    agpu::texture_weakref texture;
    agpu_texture_view_description description;
    UINT shader4ComponentMapping;
};

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_TEXTURE_VIEW_HPP
