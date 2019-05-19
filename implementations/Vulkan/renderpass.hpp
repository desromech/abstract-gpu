#ifndef AGPU_VULKAN_RENDERPASS_HPP
#define AGPU_VULKAN_RENDERPASS_HPP

#include "device.hpp"

namespace AgpuVulkan
{

struct AVkRenderPass : public agpu::renderpass
{
public:
    static constexpr size_t MaxRenderTargetAttachmentCount = 16;

    AVkRenderPass(const agpu::device_ref &device);
    ~AVkRenderPass();

    static agpu::renderpass_ref create(const agpu::device_ref &device, agpu_renderpass_description *description);

    virtual agpu_error setDepthStencilClearValue(agpu_depth_stencil_value value) override;
    virtual agpu_error setColorClearValue(agpu_uint attachment_index, agpu_color4f value) override;
    virtual agpu_error setColorClearValueFrom(agpu_uint attachment_index, agpu_color4f *value) override;
    virtual agpu_error getColorAttachmentFormats(agpu_uint* color_attachment_count, agpu_texture_format* formats) override;
    virtual agpu_texture_format getDepthStencilAttachmentFormat() override;

    agpu::device_ref device;
    VkRenderPass handle;
    std::vector<VkClearValue> clearValues;
    bool hasDepthStencil;

    agpu_uint colorAttachmentCount;
    std::array<agpu_texture_format, MaxRenderTargetAttachmentCount> colorAttachmentFormats;
    agpu_texture_format depthStencilFormat;

};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_RENDERPASS_HPP
