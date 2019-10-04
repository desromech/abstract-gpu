#include "sampler.hpp"

namespace AgpuD3D12
{
inline D3D12_FILTER mapFilter(agpu_filter filter)
{
	return (D3D12_FILTER)filter;
}

inline D3D12_TEXTURE_ADDRESS_MODE mapAddressMode(agpu_texture_address_mode mode)
{
	if (mode == 0) return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	return (D3D12_TEXTURE_ADDRESS_MODE)mode;
}

ADXSampler::ADXSampler()
{
}

ADXSampler::~ADXSampler()
{
}

agpu::sampler_ref ADXSampler::create(const agpu::device_ref &device, agpu_sampler_description *description)
{
    if(!description)
        return agpu::sampler_ref();

	D3D12_SAMPLER_DESC desc;
	desc.Filter = mapFilter(description->filter);
	desc.AddressU = mapAddressMode(description->address_u);
	desc.AddressV = mapAddressMode(description->address_v);
	desc.AddressW = mapAddressMode(description->address_w);
	desc.MipLODBias = description->mip_lod_bias;
	desc.MaxAnisotropy = (UINT)description->maxanisotropy;
	desc.BorderColor[0] = description->border_color.r;
	desc.BorderColor[1] = description->border_color.g;
	desc.BorderColor[2] = description->border_color.b;
	desc.BorderColor[3] = description->border_color.a;
	desc.MinLOD = description->min_lod;
	desc.MaxLOD = description->max_lod;

    auto result = agpu::makeObject<ADXSampler> ();
    result.as<ADXSampler> ()->d3dDescription = desc;
    return result;
}

agpu_error ADXSampler::getSamplerDesc(D3D12_SAMPLER_DESC* out)
{
	*out = d3dDescription;
	return AGPU_OK;
}


} // End of namespace AgpuD3D12
