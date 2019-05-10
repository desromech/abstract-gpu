#include "renderpass.hpp"
#include "framebuffer.hpp"
#include "texture.hpp"

namespace AgpuMetal
{
    
inline MTLLoadAction mapLoadAction(agpu_renderpass_attachment_action action)
{
    switch(action)
    {
    case AGPU_ATTACHMENT_KEEP: return MTLLoadActionLoad;
    case AGPU_ATTACHMENT_CLEAR: return MTLLoadActionClear;
    case AGPU_ATTACHMENT_DISCARD:
    default: return MTLLoadActionDontCare;
    }
}

inline MTLStoreAction mapStoreAction(agpu_renderpass_attachment_action action)
{
    switch(action)
    {
    case AGPU_ATTACHMENT_KEEP: return MTLStoreActionStore;
    case AGPU_ATTACHMENT_CLEAR:
    case AGPU_ATTACHMENT_DISCARD:
    default: return MTLStoreActionDontCare;
    }
}

AMtlRenderPass::AMtlRenderPass(const agpu::device_ref &device)
    : device(device)
{
    hasDepthStencil = false;
    hasStencil = false;
}

AMtlRenderPass::~AMtlRenderPass()
{
}

agpu::renderpass_ref AMtlRenderPass::create(const agpu::device_ref &device, agpu_renderpass_description *description)
{
    if(!description)
        return agpu::renderpass_ref();
    if(description->color_attachment_count > 0 && !description->color_attachments)
        return agpu::renderpass_ref();

    auto result = agpu::makeObject<AMtlRenderPass> (device);
    auto renderpass = result.as<AMtlRenderPass> ();

    // Store the color attachments.
    renderpass->colorAttachments.reserve(description->color_attachment_count);
    for(int i = 0; i < description->color_attachment_count; ++i)
        renderpass->colorAttachments.push_back(description->color_attachments[i]);

    renderpass->hasDepthStencil = description->depth_stencil_attachment != nullptr;
    if(renderpass->hasDepthStencil)
    {
        renderpass->depthStencil = *description->depth_stencil_attachment;
        auto depthStencilFormat = renderpass->depthStencil.format;
        renderpass->hasStencil = depthStencilFormat == AGPU_TEXTURE_FORMAT_D32_FLOAT_S8X24_UINT ||
            depthStencilFormat == AGPU_TEXTURE_FORMAT_D24_UNORM_S8_UINT;
    }
    return result;
}

MTLRenderPassDescriptor *AMtlRenderPass::createDescriptor(const agpu::framebuffer_ref &framebuffer)
{
    if(!framebuffer)
        return nullptr;

    // Validate the color attachments from the framebuffer.
    auto amtlFramebuffer = framebuffer.as<AMtlFramebuffer> ();
    if(amtlFramebuffer->ownedBySwapChain)
    {
        if(colorAttachments.size() != 1)
            return nullptr;
    }
    else
    {
        if(amtlFramebuffer->colorBuffers.size() != colorAttachments.size())
            return nullptr;
    }

    // TODO: Validate the depth stencil attachments.
    auto descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    for(size_t i = 0; i < colorAttachments.size(); ++i)
    {
        auto dest = descriptor.colorAttachments[i];

        auto &source = colorAttachments[i];
        auto &color = source.clear_value;
        dest.texture = amtlFramebuffer->getColorTexture(i);
        dest.clearColor = MTLClearColorMake(color.r, color.g, color.b, color.a);
        dest.loadAction = mapLoadAction(source.begin_action);
        dest.storeAction = mapStoreAction(source.end_action);
        if(!amtlFramebuffer->ownedBySwapChain)
        {
            auto &view = amtlFramebuffer->colorBufferDescriptions[i];
            dest.level = view.subresource_range.base_miplevel;
            dest.slice = view.subresource_range.base_arraylayer;            
        }
    }

    if(hasDepthStencil)
    {
        auto &view = amtlFramebuffer->depthStencilBufferDescription;

        auto depthAttachment = descriptor.depthAttachment;
        auto depthStencilBufferHandle = amtlFramebuffer->depthStencilBuffer.as<AMtlTexture> ()->handle;
        depthAttachment.texture = depthStencilBufferHandle;
        depthAttachment.level = view.subresource_range.base_miplevel;
        depthAttachment.slice = view.subresource_range.base_arraylayer;
        depthAttachment.clearDepth = depthStencil.clear_value.depth;
        depthAttachment.loadAction = mapLoadAction(depthStencil.begin_action);
        depthAttachment.storeAction = mapStoreAction(depthStencil.end_action);

        if(hasStencil)
        {
            auto stencilAttachment = descriptor.stencilAttachment;
            stencilAttachment.texture = depthStencilBufferHandle;
            stencilAttachment.level = view.subresource_range.base_miplevel;
            stencilAttachment.slice = view.subresource_range.base_arraylayer;
            stencilAttachment.clearStencil = depthStencil.clear_value.stencil;
            stencilAttachment.loadAction = mapLoadAction(depthStencil.stencil_begin_action);
            stencilAttachment.storeAction = mapStoreAction(depthStencil.stencil_end_action);            
        }
    }

    return descriptor;
}

agpu_error AMtlRenderPass::setDepthStencilClearValue ( agpu_depth_stencil_value value )
{
    depthStencil.clear_value = value;
    return AGPU_OK;
}

agpu_error AMtlRenderPass::setColorClearValue ( agpu_uint attachment_index, agpu_color4f value )
{
    if(attachment_index >= colorAttachments.size())
        return AGPU_OUT_OF_BOUNDS;

    colorAttachments[attachment_index].clear_value = value;
    return AGPU_OK;
}

agpu_error AMtlRenderPass::setColorClearValueFrom(agpu_uint attachment_index, agpu_color4f *value)
{
    CHECK_POINTER(value);
    return setColorClearValue(attachment_index, *value);
}

} // End of namespace AgpuMetal
