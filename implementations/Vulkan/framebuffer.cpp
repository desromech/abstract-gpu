#include "framebuffer.hpp"
#include "texture.hpp"
#include "texture_view.hpp"
#include "texture_format.hpp"

namespace AgpuVulkan
{

AVkFramebuffer::AVkFramebuffer(const agpu::device_ref &device)
    : device(device)
{
    renderPass = VK_NULL_HANDLE;
    framebuffer = VK_NULL_HANDLE;
    swapChainFramebuffer = false;
}

AVkFramebuffer::~AVkFramebuffer()
{
    vkDestroyFramebuffer(deviceForVk->device, framebuffer, nullptr);
    vkDestroyRenderPass(deviceForVk->device, renderPass, nullptr);
}

agpu::framebuffer_ref AVkFramebuffer::create(const agpu::device_ref &device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu::texture_view_ref* colorViews, const agpu::texture_view_ref &depthStencilView)
{
    // Attachments
    std::vector<VkAttachmentDescription> attachments(colorCount + (depthStencilView ? 1 : 0));
    std::vector<agpu::texture_view_ref> attachmentViews(attachments.size());
    std::vector<agpu::texture_ref> attachmentTextures(attachments.size());
    std::vector<VkImageView> attachmentImageViews(attachments.size());
    for (agpu_uint i = 0; i < colorCount; ++i)
    {
        auto &view = colorViews[i];
        auto &attachment = attachments[i];
        if (!view)
            return agpu::framebuffer_ref();

        if(!(attachmentTextures[i] = agpu::texture_ref(view->getTexture())))
            return agpu::framebuffer_ref();

        auto avkView = view.as<AVkTextureView>();
        attachment.format = mapTextureFormat(avkView->description.format);
        attachment.samples = mapSampleCount(avkView->description.sample_count);
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachmentImageViews[i] = avkView->handle;
        attachmentViews[i] = view;
    }

    if (depthStencilView)
    {
        if(!(attachmentTextures.back() = agpu::texture_ref(depthStencilView->getTexture())))
            return agpu::framebuffer_ref();

        auto avkView = depthStencilView.as<AVkTextureView>();
        auto &attachment = attachments.back();
        attachment.format = mapTextureFormat(avkView->description.format);
        attachment.samples = mapSampleCount(avkView->description.sample_count);
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachmentImageViews.back() = avkView->handle;
        attachmentViews.back() = depthStencilView;

    }

    // Color reference
    std::vector<VkAttachmentReference> colorReference(colorCount);
    for (agpu_uint i = 0; i < colorCount; ++i)
    {
        colorReference[i].attachment = i;
        colorReference[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    // Depth reference
    VkAttachmentReference depthReference;
    memset(&depthReference, 0, sizeof(depthReference));
    depthReference.attachment = colorCount;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Sub pass
    VkSubpassDescription subpass;
    memset(&subpass, 0, sizeof(subpass));
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = colorCount;
    subpass.pColorAttachments = colorCount > 0 ? &colorReference[0] : nullptr;
    subpass.pDepthStencilAttachment = &depthReference;
    if (!depthStencilView)
        subpass.pDepthStencilAttachment = nullptr;

    // Render pass
    VkRenderPassCreateInfo renderPassCreateInfo;
    memset(&renderPassCreateInfo, 0, sizeof(renderPassCreateInfo));
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = (uint32_t)attachments.size();
    renderPassCreateInfo.pAttachments = attachments.empty() ? nullptr : &attachments[0];
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;

    VkRenderPass renderPass;
    auto error = vkCreateRenderPass(deviceForVk->device, &renderPassCreateInfo, nullptr, &renderPass);
    if (error)
        return agpu::framebuffer_ref();

    // Create the framebuffer
    VkFramebufferCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(createInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.attachmentCount = (uint32_t)attachmentImageViews.size();
    createInfo.pAttachments = attachmentImageViews.empty() ? nullptr : &attachmentImageViews[0];
    createInfo.renderPass = renderPass;
    createInfo.width = width;
    createInfo.height = height;
    createInfo.layers = 1;

    VkFramebuffer framebuffer;
    error = vkCreateFramebuffer(deviceForVk->device, &createInfo, nullptr, &framebuffer);
    if (error)
    {
        vkDestroyRenderPass(deviceForVk->device, renderPass, nullptr);
        return agpu::framebuffer_ref();
    }

    auto result = agpu::makeObject<AVkFramebuffer> (device);
    auto avkFramebuffer = result.as<AVkFramebuffer> ();
    avkFramebuffer->colorCount = colorCount;
    avkFramebuffer->hasDepthStencil = (bool)depthStencilView;

    avkFramebuffer->width = width;
    avkFramebuffer->height = height;
    avkFramebuffer->renderPass = renderPass;
    avkFramebuffer->framebuffer = framebuffer;
    avkFramebuffer->attachmentViews = attachmentViews;
    avkFramebuffer->attachmentTextures = attachmentTextures;
    return result;
}

} // End of namespace AgpuVulkan
