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
