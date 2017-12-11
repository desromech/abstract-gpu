#include "renderpass.hpp"
#include "framebuffer.hpp"
#include "texture.hpp"

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

_agpu_renderpass::_agpu_renderpass(agpu_device *device)
    : device(device)
{
    hasDepthStencil = false;
}

void _agpu_renderpass::lostReferences()
{
}

agpu_renderpass *_agpu_renderpass::create(agpu_device *device, agpu_renderpass_description *description)
{
    if(!description)
        return nullptr;
    if(description->color_attachment_count > 0 && !description->color_attachments)
        return nullptr;

    auto result = new agpu_renderpass(device);

    // Store the color attachments.
    result->colorAttachments.reserve(description->color_attachment_count);
    for(int i = 0; i < description->color_attachment_count; ++i)
        result->colorAttachments.push_back(description->color_attachments[i]);

    result->hasDepthStencil = description->depth_stencil_attachment != nullptr;
    if(result->hasDepthStencil)
        result->depthStencil = *description->depth_stencil_attachment;
    return result;
}

MTLRenderPassDescriptor *_agpu_renderpass::createDescriptor(agpu_framebuffer *framebuffer)
{
    if(!framebuffer)
        return nullptr;

    // Validate the color attachments from the framebuffer.
    if(framebuffer->ownedBySwapChain)
    {
        if(colorAttachments.size() != 1)
            return nullptr;
    }
    else
    {
        if(framebuffer->colorBuffers.size() != colorAttachments.size())
            return nullptr;
    }

    // TODO: Validate the depth stencil attachments.
    auto descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    for(size_t i = 0; i < colorAttachments.size(); ++i)
    {
        auto dest = descriptor.colorAttachments[i];
        auto &view = framebuffer->colorBufferDescriptions[i];
        
        auto &source = colorAttachments[i];
        auto &color = source.clear_value;
        dest.texture = framebuffer->getColorTexture(i);
        dest.clearColor = MTLClearColorMake(color.r, color.g, color.b, color.a);
        dest.loadAction = mapLoadAction(source.begin_action);
        dest.storeAction = mapStoreAction(source.end_action);
        dest.level = view.subresource_range.base_miplevel;
        dest.slice = view.subresource_range.base_arraylayer;
    }

    if(hasDepthStencil)
    {
        auto &view = framebuffer->depthStencilBufferDescription;
        
        auto depthAttachment = descriptor.depthAttachment;
        depthAttachment.texture = framebuffer->depthStencilBuffer->handle;
        depthAttachment.level = view.subresource_range.base_miplevel;
        depthAttachment.slice = view.subresource_range.base_arraylayer;
        depthAttachment.clearDepth = depthStencil.clear_value.depth;
        depthAttachment.loadAction = mapLoadAction(depthStencil.begin_action);
        depthAttachment.storeAction = mapStoreAction(depthStencil.end_action);

        auto stencilAttachment = descriptor.stencilAttachment;
        stencilAttachment.texture = framebuffer->depthStencilBuffer->handle;
        stencilAttachment.level = view.subresource_range.base_miplevel;
        stencilAttachment.slice = view.subresource_range.base_arraylayer;
        stencilAttachment.clearStencil = depthStencil.clear_value.stencil;
        stencilAttachment.loadAction = mapLoadAction(depthStencil.stencil_begin_action);
        stencilAttachment.storeAction = mapStoreAction(depthStencil.stencil_end_action);
    }

    return descriptor;
}

agpu_error _agpu_renderpass::setDepthStencilClearValue ( agpu_depth_stencil_value value )
{
    depthStencil.clear_value = value;
    return AGPU_OK;
}

agpu_error _agpu_renderpass::setColorClearValue ( agpu_uint attachment_index, agpu_color4f value )
{
    if(attachment_index >= colorAttachments.size())
        return AGPU_OUT_OF_BOUNDS;

    colorAttachments[attachment_index].clear_value = value;
    return AGPU_OK;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddRenderPassReference ( agpu_renderpass* renderpass )
{
    CHECK_POINTER(renderpass);
    return renderpass->retain();
}

AGPU_EXPORT agpu_error agpuReleaseRenderPass ( agpu_renderpass* renderpass )
{
    CHECK_POINTER(renderpass);
    return renderpass->release();
}

AGPU_EXPORT agpu_error agpuSetDepthStencilClearValue ( agpu_renderpass* renderpass, agpu_depth_stencil_value value )
{
    CHECK_POINTER(renderpass);
    return renderpass->setDepthStencilClearValue(value);
}

AGPU_EXPORT agpu_error agpuSetColorClearValue ( agpu_renderpass* renderpass, agpu_uint attachment_index, agpu_color4f value )
{
    CHECK_POINTER(renderpass);
    return renderpass->setColorClearValue(attachment_index, value);
}
