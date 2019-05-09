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

    static agpu::framebuffer_ref create(const agpu::device_ref &device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView);

    agpu::device_ref device;

    agpu_bool swapChainFramebuffer;
    agpu_uint colorCount;
    agpu_bool hasDepthStencil;
    agpu_uint width;
    agpu_uint height;
    VkRenderPass renderPass;
    VkFramebuffer framebuffer;
    std::vector<agpu::texture_ref> attachmentTextures;
    std::vector<VkImageView> attachmentViews;
    std::vector<agpu_texture_view_description> attachmentDescriptions;

};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_FRAMEBUFFER_HPP
