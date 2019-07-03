#ifndef AGPU_SAMPLER_HPP
#define AGPU_SAMPLER_HPP

#include "device.hpp"

namespace AgpuVulkan
{

class AVkSampler : public agpu::sampler
{
public:
    AVkSampler(const agpu::device_ref &device);
    ~AVkSampler();

    static agpu::sampler_ref create(const agpu::device_ref &device, agpu_sampler_description *description);

    agpu::device_ref device;
    VkSampler handle;
};

} // End of namespace AgpuVulkan

#endif //AGPU_TEXTURE_HPP
