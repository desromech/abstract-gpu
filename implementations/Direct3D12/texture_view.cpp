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

agpu_error ADXTextureView::getSampledTextureViewDescription(D3D12_SHADER_RESOURCE_VIEW_DESC* out)
{
	memset(out, 0, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
	out->Format = (DXGI_FORMAT)description.format;
	out->Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	bool isArray = description.subresource_range.layer_count > 1 || description.subresource_range.base_arraylayer > 0;

	auto lodClamp = float(description.subresource_range.base_miplevel);

	switch (description.type)
	{
	case AGPU_TEXTURE_BUFFER:
		return AGPU_UNIMPLEMENTED;
		break;
	case AGPU_TEXTURE_1D:
		if (isArray)
		{
			out->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
			out->Texture1DArray.MostDetailedMip = description.subresource_range.base_miplevel;
			out->Texture1DArray.MipLevels = description.subresource_range.level_count;
			out->Texture1DArray.FirstArraySlice = description.subresource_range.base_arraylayer;
			out->Texture1DArray.ArraySize = description.subresource_range.layer_count;
			out->Texture1DArray.ResourceMinLODClamp = lodClamp;
		}
		else
		{
			out->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
			out->Texture1D.MostDetailedMip = description.subresource_range.base_miplevel;
			out->Texture1D.MipLevels = description.subresource_range.level_count;
			out->Texture1D.ResourceMinLODClamp = lodClamp;
		}
		break;
	case AGPU_TEXTURE_2D:
		if (isArray)
		{
			out->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			out->Texture2DArray.MostDetailedMip = description.subresource_range.base_miplevel;
			out->Texture2DArray.MipLevels = description.subresource_range.level_count;
			out->Texture2DArray.FirstArraySlice = description.subresource_range.base_arraylayer;
			out->Texture2DArray.ArraySize = description.subresource_range.layer_count;
			out->Texture2DArray.ResourceMinLODClamp = lodClamp;
		}
		else
		{
			out->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			out->Texture2D.MostDetailedMip = description.subresource_range.base_miplevel;
			out->Texture2D.MipLevels = description.subresource_range.level_count;
			out->Texture2D.ResourceMinLODClamp = lodClamp;
		}
		break;
	case AGPU_TEXTURE_CUBE:
		isArray = description.subresource_range.layer_count > 6 || description.subresource_range.base_arraylayer > 0;
		if (isArray)
		{
			out->ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
			out->TextureCubeArray.MostDetailedMip = description.subresource_range.base_miplevel;
			out->TextureCubeArray.MipLevels = description.subresource_range.level_count;
			out->TextureCubeArray.First2DArrayFace = description.subresource_range.base_arraylayer;
			out->TextureCubeArray.NumCubes = description.subresource_range.layer_count;
			out->TextureCubeArray.ResourceMinLODClamp = lodClamp;
		}
		else
		{
			out->ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			out->TextureCube.MostDetailedMip = description.subresource_range.base_miplevel;
			out->TextureCube.MipLevels = description.subresource_range.level_count;
			out->TextureCube.ResourceMinLODClamp = lodClamp;
		}
		break;
	case AGPU_TEXTURE_3D:
		out->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
		out->Texture3D.MostDetailedMip = description.subresource_range.base_miplevel;
		out->Texture3D.MipLevels = description.subresource_range.level_count;
		out->Texture3D.ResourceMinLODClamp = lodClamp;
		break;
	default:
		abort();
	}

	return AGPU_OK;
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
