#ifndef AGPU_VULKAN_FRAMEBUFFER_HPP
#define AGPU_VULKAN_FRAMEBUFFER_HPP

#include "device.hpp"

struct _agpu_framebuffer : public Object<_agpu_framebuffer>
{
    _agpu_framebuffer(agpu_device *device);
    void lostReferences();

    static agpu_framebuffer *create(agpu_device *device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView);

    agpu_device *device;

    agpu_bool swapChainFramebuffer;
    agpu_uint colorCount;
    agpu_bool hasDepthStencil;
    agpu_uint width;
    agpu_uint height;
    VkRenderPass renderPass;
    VkFramebuffer framebuffer;
    std::vector<agpu_texture*> attachmentTextures;
    std::vector<VkImageView> attachmentViews;

};
#endif //AGPU_VULKAN_FRAMEBUFFER_HPP