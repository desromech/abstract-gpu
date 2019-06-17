#include "renderpass.hpp"
#include "texture_format.hpp"

namespace AgpuVulkan
{

inline enum VkAttachmentLoadOp mapLoadOp(agpu_renderpass_attachment_action action)
{
    switch (action)
    {
    case AGPU_ATTACHMENT_KEEP: return VK_ATTACHMENT_LOAD_OP_LOAD;
    case AGPU_ATTACHMENT_CLEAR: return VK_ATTACHMENT_LOAD_OP_CLEAR;
    case AGPU_ATTACHMENT_DISCARD: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    default: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }
}

inline enum VkAttachmentStoreOp mapStoreOp(agpu_renderpass_attachment_action action)
{
    switch (action)
    {
    case AGPU_ATTACHMENT_KEEP: return VK_ATTACHMENT_STORE_OP_STORE;
    case AGPU_ATTACHMENT_CLEAR: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    case AGPU_ATTACHMENT_DISCARD: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    default: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }
}

AVkRenderPass::AVkRenderPass(const agpu::device_ref &device)
    : device(device)
{
    handle = VK_NULL_HANDLE;
}

AVkRenderPass::~AVkRenderPass()
{
    if (handle)
        vkDestroyRenderPass(deviceForVk->device, handle, nullptr);
}

agpu::renderpass_ref AVkRenderPass::create(const agpu::device_ref &device, agpu_renderpass_description *description)
{
    if (!description)
        return agpu::renderpass_ref();

    // Attachments
    auto colorCount = description->color_attachment_count;
    if(colorCount > MaxRenderTargetAttachmentCount)
        return agpu::renderpass_ref();

    std::array<agpu_texture_format, MaxRenderTargetAttachmentCount> colorAttachmentFormats;
    agpu_texture_format depthStencilFormat = AGPU_TEXTURE_FORMAT_UNKNOWN;

    bool hasDepthStencil = description->depth_stencil_attachment != nullptr;
    std::vector<VkAttachmentDescription> attachments(colorCount + (hasDepthStencil ? 1 : 0));
    std::vector<VkClearValue> clearValues;
    clearValues.reserve(attachments.size());
    for (agpu_uint i = 0; i < colorCount; ++i)
    {
        auto desc = description->color_attachments[i];
        auto &attachment = attachments[i];
        colorAttachmentFormats[i] = desc.format;

        attachment.format = mapTextureFormat(desc.format);
        attachment.samples = mapSampleCount(desc.sample_count);
        attachment.loadOp = mapLoadOp(desc.begin_action);
        attachment.storeOp = mapStoreOp(desc.end_action);
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkClearValue clearValue;
        clearValue.color.float32[0] = desc.clear_value.r;
        clearValue.color.float32[1] = desc.clear_value.g;
        clearValue.color.float32[2] = desc.clear_value.b;
        clearValue.color.float32[3] = desc.clear_value.a;
        clearValues.push_back(clearValue);
    }

    if (hasDepthStencil)
    {
        auto desc = description->depth_stencil_attachment;
        auto &attachment = attachments.back();
        auto hasStencil = hasStencilComponent(desc->format);
        depthStencilFormat = desc->format;
        attachment.format = mapTextureFormat(desc->format);
        attachment.samples = mapSampleCount(desc->sample_count);
        attachment.loadOp = mapLoadOp(desc->begin_action);
        attachment.storeOp = mapStoreOp(desc->end_action);
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        if (hasStencil)
        {
            attachment.stencilLoadOp = mapLoadOp(desc->stencil_begin_action);
            attachment.stencilStoreOp = mapStoreOp(desc->stencil_end_action);
        }

        attachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkClearValue clearValue;
        clearValue.depthStencil.depth = desc->clear_value.depth;
        clearValue.depthStencil.stencil = desc->clear_value.stencil;
        clearValues.push_back(clearValue);
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
    subpass.pColorAttachments = colorReference.empty() ? nullptr : &colorReference[0];
    subpass.pDepthStencilAttachment = &depthReference;
    if (!hasDepthStencil)
        subpass.pDepthStencilAttachment = nullptr;

    // Render pass
    VkRenderPassCreateInfo renderPassCreateInfo;
    memset(&renderPassCreateInfo, 0, sizeof(renderPassCreateInfo));
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = (uint32_t)attachments.size();
    renderPassCreateInfo.pAttachments = &attachments[0];
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;

    VkRenderPass renderPass;
    auto error = vkCreateRenderPass(deviceForVk->device, &renderPassCreateInfo, nullptr, &renderPass);
    if (error)
        return agpu::renderpass_ref();

    auto result = agpu::makeObject<AVkRenderPass> (device);
    auto avkRenderpass = result.as<AVkRenderPass> ();
    avkRenderpass->handle = renderPass;
    avkRenderpass->clearValues = clearValues;
    avkRenderpass->hasDepthStencil = description->depth_stencil_attachment != nullptr;
    avkRenderpass->colorAttachmentCount = colorCount;
    avkRenderpass->colorAttachmentFormats = colorAttachmentFormats;
    avkRenderpass->depthStencilFormat = depthStencilFormat;

    return result;
}

agpu_error AVkRenderPass::setDepthStencilClearValue(agpu_depth_stencil_value value)
{
    if (hasDepthStencil)
    {
        auto &dest = clearValues.back();
        dest.depthStencil.depth = value.depth;
        dest.depthStencil.stencil = value.stencil;
    }

    return AGPU_OK;
}

agpu_error AVkRenderPass::setColorClearValue(agpu_uint attachment_index, agpu_color4f value)
{
    auto colorCount = clearValues.size();
    if (hasDepthStencil)
        --colorCount;
    if (attachment_index >= colorCount)
        return AGPU_OUT_OF_BOUNDS;

    auto &dest = clearValues[attachment_index];
    dest.color.float32[0] = value.r;
    dest.color.float32[1] = value.g;
    dest.color.float32[2] = value.b;
    dest.color.float32[3] = value.a;
    return AGPU_OK;
}

agpu_error AVkRenderPass::setColorClearValueFrom(agpu_uint attachment_index, agpu_color4f *value)
{
    CHECK_POINTER(value);
    return setColorClearValue(attachment_index, *value);
}

agpu_error AVkRenderPass::getColorAttachmentFormats(agpu_uint* color_attachment_count, agpu_texture_format* formats)
{
    CHECK_POINTER(color_attachment_count);
    if(formats)
    {
        if(*color_attachment_count < this->colorAttachmentCount)
            return AGPU_INVALID_PARAMETER;

        for(agpu_uint i = 0; i < colorAttachmentCount; ++i)
            formats[i] = this->colorAttachmentFormats[i];
    }

    *color_attachment_count = this->colorAttachmentCount;
    return AGPU_OK;
}

agpu_texture_format AVkRenderPass::getDepthStencilAttachmentFormat()
{
    return depthStencilFormat;
}

} // End of namespace AgpuVulkan
