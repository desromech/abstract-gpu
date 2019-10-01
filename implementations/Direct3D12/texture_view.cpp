#include "texture_view.hpp"

namespace AgpuD3D12
{

ADXTextureView::ADXTextureView(const agpu::device_ref &cdevice, const agpu::texture_ref &ctexture, const agpu_texture_view_description &cdescription)
    : device(cdevice), texture(ctexture), description(cdescription)
{
}

ADXTextureView::~ADXTextureView()
{
}

agpu::texture_view_ref ADXTextureView::create(const agpu::device_ref &device, const agpu::texture_ref &texture, const agpu_texture_view_description &description)
{
    return agpu::makeObject<ADXTextureView> (device, texture, description);
}

agpu::texture_ptr ADXTextureView::getTexture()
{
    return texture.lock().disown();
}

agpu_error ADXTextureView::getColorAttachmentViewDescription(D3D12_RENDER_TARGET_VIEW_DESC *out)
{
    memset(out, 0, sizeof(D3D12_RENDER_TARGET_VIEW_DESC));
    out->Format = (DXGI_FORMAT)description.format;
    if (description.sample_count > 1)
        out->ViewDimension = description.subresource_range.layer_count > 1 ? D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY : D3D12_RTV_DIMENSION_TEXTURE2DMS;
    else
        out->ViewDimension = description.subresource_range.layer_count > 1 ? D3D12_RTV_DIMENSION_TEXTURE2DARRAY : D3D12_RTV_DIMENSION_TEXTURE2D;
    return AGPU_OK;
}

agpu_error ADXTextureView::getDepthStencilViewDescription(D3D12_DEPTH_STENCIL_VIEW_DESC *out)
{
    memset(out, 0, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
    out->Format = (DXGI_FORMAT)description.format;
    if (description.sample_count > 1)
        out->ViewDimension = description.subresource_range.layer_count > 1 ? D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D12_DSV_DIMENSION_TEXTURE2DMS;
    else
        out->ViewDimension = description.subresource_range.layer_count > 1 ? D3D12_DSV_DIMENSION_TEXTURE2DARRAY : D3D12_DSV_DIMENSION_TEXTURE2D;
    return AGPU_OK;
}


} // End of namespace AgpuD3D12
