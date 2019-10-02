#include "sampler.hpp"

namespace AgpuD3D12
{

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
        
    auto result = agpu::makeObject<ADXSampler> ();
    result.as<ADXSampler> ()->description = *description;
    return result;
}

agpu_error ADXSampler::getSamplerDesc(D3D12_SAMPLER_DESC* out)
{
	memset(out, 0, sizeof(D3D12_SAMPLER_DESC));
	out->Filter = (D3D12_FILTER)description.filter;
	out->AddressU = (D3D12_TEXTURE_ADDRESS_MODE)description.address_u;
	out->AddressV = (D3D12_TEXTURE_ADDRESS_MODE)description.address_v;
	out->AddressW = (D3D12_TEXTURE_ADDRESS_MODE)description.address_w;
	out->MipLODBias = description.mip_lod_bias;
	out->MaxAnisotropy = (UINT)description.maxanisotropy;
	out->BorderColor[0] = description.border_color.r;
	out->BorderColor[1] = description.border_color.g;
	out->BorderColor[2] = description.border_color.b;
	out->BorderColor[3] = description.border_color.a;
	out->MinLOD = description.min_lod;
	out->MaxLOD = description.max_lod;

	return AGPU_OK;
}


} // End of namespace AgpuD3D12
