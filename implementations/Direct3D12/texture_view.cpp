#include "texture_view.hpp"

namespace AgpuD3D12
{

inline D3D12_SHADER_COMPONENT_MAPPING mapSwizzleComponent(agpu_component_swizzle component, D3D12_SHADER_COMPONENT_MAPPING identity)
{
	switch (component)
	{
	case AGPU_COMPONENT_SWIZZLE_IDENTITY: return identity;
	case AGPU_COMPONENT_SWIZZLE_ONE: return D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_1;
	case AGPU_COMPONENT_SWIZZLE_ZERO: return D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0;
	case AGPU_COMPONENT_SWIZZLE_R: return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0;
	case AGPU_COMPONENT_SWIZZLE_G: return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1;
	case AGPU_COMPONENT_SWIZZLE_B: return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2;
	case AGPU_COMPONENT_SWIZZLE_A: return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3;
	default: abort();
	}
}


ADXTextureView::ADXTextureView(const agpu::device_ref &cdevice, const agpu::texture_ref &ctexture, const agpu_texture_view_description &cdescription, UINT cshader4ComponentMapping)
    : device(cdevice), texture(ctexture), description(cdescription), shader4ComponentMapping(cshader4ComponentMapping)
{
}

ADXTextureView::~ADXTextureView()
{
}

agpu::texture_view_ref ADXTextureView::create(const agpu::device_ref &device, const agpu::texture_ref &texture, const agpu_texture_view_description &description)
{
	auto shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(
		mapSwizzleComponent(description.components.r, D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0),
		mapSwizzleComponent(description.components.g, D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1),
		mapSwizzleComponent(description.components.b, D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2),
		mapSwizzleComponent(description.components.a, D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3)
	);

    return agpu::makeObject<ADXTextureView> (device, texture, description, shader4ComponentMapping);
}

agpu::texture_ptr ADXTextureView::getTexture()
{
    return texture.lock().disown();
}

agpu_error ADXTextureView::getSampledTextureViewDescription(D3D12_SHADER_RESOURCE_VIEW_DESC* out)
{
	memset(out, 0, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
	out->Format = (DXGI_FORMAT)description.format;
	out->Shader4ComponentMapping = shader4ComponentMapping;

	bool isArray = description.subresource_range.layer_count > 1 || description.subresource_range.base_arraylayer > 0;

	auto lodClamp = float(description.subresource_range.base_miplevel);

	switch (description.type)
	{
	case AGPU_TEXTURE_BUFFER:
		return AGPU_UNIMPLEMENTED;
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
		if (description.sample_count > 1)
		{
			if (isArray)
			{
				out->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
				out->Texture2DArray.FirstArraySlice = description.subresource_range.base_arraylayer;
				out->Texture2DArray.ArraySize = description.subresource_range.layer_count;
			}
			else
			{
				out->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
			}
		}
		else
		{
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
			out->TextureCubeArray.NumCubes = description.subresource_range.layer_count / 6;
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


agpu_error ADXTextureView::getUnorderedAccessTextureViewDescription(D3D12_UNORDERED_ACCESS_VIEW_DESC* out)
{
	memset(out, 0, sizeof(D3D12_RENDER_TARGET_VIEW_DESC));
	out->Format = (DXGI_FORMAT)description.format;

	bool isArray = description.subresource_range.layer_count > 1 || description.subresource_range.base_arraylayer > 0;

	switch (description.type)
	{
	case AGPU_TEXTURE_BUFFER:
		return AGPU_UNIMPLEMENTED;
		break;
	case AGPU_TEXTURE_1D:
		if (isArray)
		{
			out->ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
			out->Texture1DArray.MipSlice = description.subresource_range.base_miplevel;
			out->Texture1DArray.FirstArraySlice = description.subresource_range.base_arraylayer;
			out->Texture1DArray.ArraySize = description.subresource_range.layer_count;
		}
		else
		{
			out->ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
			out->Texture1D.MipSlice = description.subresource_range.base_miplevel;
		}
		break;
	case AGPU_TEXTURE_2D:
	case AGPU_TEXTURE_CUBE:
		if (isArray)
		{
			out->ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
			out->Texture2DArray.MipSlice = description.subresource_range.base_miplevel;
			out->Texture2DArray.FirstArraySlice = description.subresource_range.base_arraylayer;
			out->Texture2DArray.ArraySize = description.subresource_range.layer_count;
		}
		else
		{
			out->ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			out->Texture2D.MipSlice = description.subresource_range.base_miplevel;
		}
		break;
	case AGPU_TEXTURE_3D:
		out->ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
		out->Texture3D.MipSlice = description.subresource_range.base_miplevel;
		out->Texture3D.FirstWSlice = description.subresource_range.base_arraylayer;
		out->Texture3D.WSize = description.subresource_range.layer_count;
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
	{
		if (description.subresource_range.base_arraylayer > 0 || description.subresource_range.layer_count > 1)
		{
			out->ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
			out->Texture2DMSArray.FirstArraySlice = description.subresource_range.base_arraylayer;
			out->Texture2DMSArray.ArraySize = description.subresource_range.layer_count;
		}
		else
		{
			out->ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
		}
	}
	else
	{
		if (description.subresource_range.base_arraylayer > 0 || description.subresource_range.layer_count > 1)
		{
			out->ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
			out->Texture2DArray.FirstArraySlice = description.subresource_range.base_arraylayer;
			out->Texture2DArray.ArraySize = description.subresource_range.layer_count;
			out->Texture2DArray.MipSlice = description.subresource_range.base_miplevel;
		}
		else
		{
			out->ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			out->Texture2D.MipSlice = description.subresource_range.base_miplevel;
		}
	}
    return AGPU_OK;
}

agpu_error ADXTextureView::getDepthStencilViewDescription(D3D12_DEPTH_STENCIL_VIEW_DESC *out)
{
    memset(out, 0, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
    out->Format = (DXGI_FORMAT)description.format;
	if (description.sample_count > 1)
	{
		if (description.subresource_range.base_arraylayer > 0 || description.subresource_range.layer_count > 1)
		{
			out->ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
			out->Texture2DMSArray.FirstArraySlice = description.subresource_range.base_arraylayer;
			out->Texture2DMSArray.ArraySize = description.subresource_range.layer_count;
		}
		else
		{
			out->ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
		}
	}
	else
	{
		if (description.subresource_range.base_arraylayer > 0 || description.subresource_range.layer_count > 1)
		{
			out->ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
			out->Texture2DArray.FirstArraySlice = description.subresource_range.base_arraylayer;
			out->Texture2DArray.ArraySize = description.subresource_range.layer_count;
			out->Texture2DArray.MipSlice = description.subresource_range.base_miplevel;
		}
		else
		{
			out->ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			out->Texture2D.MipSlice = description.subresource_range.base_miplevel;
		}
	}

	return AGPU_OK;
}


} // End of namespace AgpuD3D12
