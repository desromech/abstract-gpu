#ifndef AGPU_VULKAN_RENDERPASS_HPP
#define AGPU_VULKAN_RENDERPASS_HPP

#include "device.hpp"

struct _agpu_renderpass : public Object<_agpu_renderpass>
{
public:
    _agpu_renderpass(agpu_device *device);
    void lostReferences();

    static agpu_renderpass *create(agpu_device *device, agpu_renderpass_description *description);

    agpu_error setDepthStencilClearValue(agpu_depth_stencil_value value);
    agpu_error setColorClearValue(agpu_uint attachment_index, agpu_color4f value);
    
    agpu_device *device;
    VkRenderPass handle;
    std::vector<VkClearValue> clearValues;
    bool hasDepthStencil;
};

#endif //AGPU_VULKAN_RENDERPASS_HPP
