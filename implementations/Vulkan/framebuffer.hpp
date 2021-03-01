#ifndef AGPU_VULKAN_FRAMEBUFFER_HPP
#define AGPU_VULKAN_FRAMEBUFFER_HPP

#include "device.hpp"

namespace AgpuVulkan
{

class AVkFramebuffer : public agpu::framebuffer
{
public:
    AVkFramebuffer(const agpu::device_ref &device);
    ~AVkFramebuffer();

    static agpu::framebuffer_ref create(const agpu::device_ref &device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu::texture_view_ref* colorViews, const agpu::texture_view_ref &depthStencilView);

    virtual agpu_uint getWidth() override;
    virtual agpu_uint getHeight() override;

    agpu::device_ref device;

    agpu_bool swapChainFramebuffer;
    agpu_uint colorCount;
    agpu_bool hasDepthStencil;
    agpu_uint width;
    agpu_uint height;
    VkRenderPass renderPass;
    VkFramebuffer framebuffer;
    VkSemaphore waitSemaphore;
    VkSemaphore signalSemaphore;

    // We are keeping these references for life cycle management purposes.
    std::vector<agpu::texture_view_ref> attachmentViews;
    std::vector<agpu::texture_ref> attachmentTextures;

};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_FRAMEBUFFER_HPP
