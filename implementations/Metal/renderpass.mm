#include "renderpass.hpp"
#include "framebuffer.hpp"

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
        auto &source = colorAttachments[i];
        auto &color = source.clear_value;
        dest.texture = framebuffer->getColorTexture(i);
        dest.clearColor = MTLClearColorMake(color.r, color.g, color.b, color.a);
        dest.storeAction = MTLStoreActionStore;
        dest.loadAction = MTLLoadActionClear;
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
