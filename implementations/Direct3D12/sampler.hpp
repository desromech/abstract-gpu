#ifndef AGPU_D3D12_SAMPLER_HPP
#define AGPU_D3D12_SAMPLER_HPP

#include "device.hpp"

namespace AgpuD3D12
{
class ADXSampler : public agpu::sampler
{
public:
    ADXSampler();
    ~ADXSampler();

    static agpu::sampler_ref create(const agpu::device_ref &device, agpu_sampler_description *description);

	agpu_error getSamplerDesc(D3D12_SAMPLER_DESC* out);

	D3D12_SAMPLER_DESC d3dDescription;
};
} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_SAMPLER_HPP
