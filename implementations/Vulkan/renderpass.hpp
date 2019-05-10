#ifndef AGPU_VULKAN_RENDERPASS_HPP
#define AGPU_VULKAN_RENDERPASS_HPP

#include "device.hpp"

namespace AgpuVulkan
{

struct AVkRenderPass : public agpu::renderpass
{
public:
    AVkRenderPass(const agpu::device_ref &device);
    ~AVkRenderPass();

    static agpu::renderpass_ref create(const agpu::device_ref &device, agpu_renderpass_description *description);

    agpu_error setDepthStencilClearValue(agpu_depth_stencil_value value);
    agpu_error setColorClearValue(agpu_uint attachment_index, agpu_color4f value);
    agpu_error setColorClearValueFrom(agpu_uint attachment_index, agpu_color4f *value);

    agpu::device_ref device;
    VkRenderPass handle;
    std::vector<VkClearValue> clearValues;
    bool hasDepthStencil;
};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_RENDERPASS_HPP
