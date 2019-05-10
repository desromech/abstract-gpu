#ifndef AGPU_VULKAN_FENCE_HPP
#define AGPU_VULKAN_FENCE_HPP

#include "device.hpp"

namespace AgpuVulkan
{

class AVkFence : public agpu::fence
{
public:
    AVkFence(const agpu::device_ref &device);
    ~AVkFence();

    static agpu::fence_ref create(const agpu::device_ref &device);

    virtual agpu_error waitOnClient() override;

    agpu::device_ref device;
    VkFence fence;
};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_FENCE_HPP
