#include "renderpass.hpp"
#include "texture_formats.hpp"

_agpu_renderpass::_agpu_renderpass(agpu_device *device)
    : device(device)
{
}

void _agpu_renderpass::lostReferences()
{
}

agpu_renderpass *_agpu_renderpass::create(agpu_device *device, agpu_renderpass_description *description)
{
    if (!description)
        return nullptr;

    auto result = new agpu_renderpass(device);

    for (size_t i = 0; i < description->color_attachment_count; ++i)
        result->colorAttachments.push_back(description->color_attachments[i]);
    if (description->depth_stencil_attachment)
    {
        result->depthStencilAttachment = *description->depth_stencil_attachment;
        result->hasDepth = hasDepthComponent(result->depthStencilAttachment.format);
        result->hasStencil = hasStencilComponent(result->depthStencilAttachment.format);
    }

    return result;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddRenderPassReference(agpu_renderpass* renderpass)
{
    CHECK_POINTER(renderpass);
    return renderpass->retain();
}

AGPU_EXPORT agpu_error agpuReleaseRenderPass(agpu_renderpass* renderpass)
{
    CHECK_POINTER(renderpass);
    return renderpass->release();
}
