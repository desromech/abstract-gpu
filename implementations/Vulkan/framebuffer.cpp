#include "framebuffer.hpp"
#include "texture.hpp"
#include "texture_format.hpp"

_agpu_framebuffer::_agpu_framebuffer(agpu_device *device)
    : device(device)
{
    renderPass = nullptr;
    framebuffer = nullptr;
    swapChainFramebuffer = false;
}

void _agpu_framebuffer::lostReferences()
{
    vkDestroyFramebuffer(device->device, framebuffer, nullptr);
    vkDestroyRenderPass(device->device, renderPass, nullptr);
    for (auto view : attachmentViews)
        vkDestroyImageView(device->device, view, nullptr);
    for (auto texture : attachmentTextures)
        texture->release();
}

agpu_framebuffer *_agpu_framebuffer::create(agpu_device *device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView)
{
    // Attachments
    std::vector<VkAttachmentDescription> attachments(colorCount + (depthStencilView != nullptr ? 1 : 0));
    for (agpu_uint i = 0; i < colorCount; ++i)
    {
        auto view = &colorViews[i];
        auto &attachment = attachments[i];
        if (!view || !view->texture)
            return nullptr;

        attachment.format = mapTextureFormat(view->format);
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    if (depthStencilView != nullptr)
    {
        if (!depthStencilView->texture)
            return nullptr;

        auto &attachment = attachments.back();
        attachment.format = mapTextureFormat(depthStencilView->format);
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
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
    subpass.pColorAttachments = &colorReference[0];
    subpass.pDepthStencilAttachment = &depthReference;
    if (!depthStencilView)
        subpass.pDepthStencilAttachment = nullptr;

    // Render pass
    VkRenderPassCreateInfo renderPassCreateInfo;
    memset(&renderPassCreateInfo, 0, sizeof(renderPassCreateInfo));
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = attachments.size();
    renderPassCreateInfo.pAttachments = &attachments[0];
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;

    VkRenderPass renderPass;
    auto error = vkCreateRenderPass(device->device, &renderPassCreateInfo, nullptr, &renderPass);
    if (error)
        return nullptr;

    // Create the framebuffer
    std::vector<VkImageView> attachmentViews(attachments.size());
    for (agpu_uint i = 0; i < colorCount; ++i)
    {
        auto view = agpu_texture::createImageView(device, &colorViews[i]);
        if (!view)
            goto failure;

        attachmentViews[i] = view;
    }

    if (depthStencilView)
    {
        auto view = agpu_texture::createImageView(device, depthStencilView);
        if (!view)
            goto failure;
        attachmentViews.back() = view;
    }

    VkFramebufferCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(createInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.attachmentCount = attachmentViews.size();
    createInfo.pAttachments = &attachmentViews[0];
    createInfo.renderPass = renderPass;
    createInfo.width = width;
    createInfo.height = height;
    createInfo.layers = 1;

    VkFramebuffer framebuffer;
    error = vkCreateFramebuffer(device->device, &createInfo, nullptr, &framebuffer);
    if (error)
        goto failure;

    auto result = new agpu_framebuffer(device);
    result->colorCount = colorCount;
    result->hasDepthStencil = depthStencilView != nullptr;

    result->width = width;
    result->height = height;
    result->renderPass = renderPass;
    result->framebuffer = framebuffer;
    result->attachmentViews = attachmentViews;
    result->attachmentTextures.resize(attachmentViews.size());
    for (agpu_uint i = 0; i < colorCount; ++i)
    {
        auto texture = colorViews[i].texture;
        texture->retain();
        result->attachmentTextures[i] = texture;
    }

    if (depthStencilView)
    {
        auto texture = depthStencilView->texture;
        texture->retain();
        result->attachmentTextures.back() = texture;
    }

    return result;

failure:
    vkDestroyRenderPass(device->device, renderPass, nullptr);
    for (auto view : attachmentViews)
    {
        if (view)
            vkDestroyImageView(device->device, view, nullptr);
    }

    return nullptr;
}
