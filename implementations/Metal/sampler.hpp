#ifndef AGPU_METAL_SAMPLER_HPP
#define AGPU_METAL_SAMPLER_HPP

#include "device.hpp"

namespace AgpuMetal
{

class AMtlSampler : public agpu::sampler
{
public:    
    AMtlSampler(const agpu::device_ref &device, id<MTLSamplerState> handle);
    ~AMtlSampler();
    
    static agpu::sampler_ref create(const agpu::device_ref &device, agpu_sampler_description *description);
    
    agpu::device_ref device;
    id<MTLSamplerState> handle;
};

} // End of namespace AgpuMetal

#endif //AGPU_METAL_SAMPLER_HPP
