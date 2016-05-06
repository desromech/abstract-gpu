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

agpu_error _agpu_renderpass::setDepthStencilClearValue(agpu_depth_stencil_value value)
{
    depthStencilAttachment.clear_value = value;
    return AGPU_OK;
}

agpu_error _agpu_renderpass::setColorClearValue(agpu_uint attachment_index, agpu_color4f value)
{
    if (attachment_index >= colorAttachments.size())
        return AGPU_OUT_OF_BOUNDS;

    colorAttachments[attachment_index].clear_value = value;
    return AGPU_OK;
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

AGPU_EXPORT agpu_error agpuSetDepthStencilClearValue(agpu_renderpass* renderpass, agpu_depth_stencil_value value)
{
    CHECK_POINTER(renderpass);
    return renderpass->setDepthStencilClearValue(value);
}

AGPU_EXPORT agpu_error agpuSetColorClearValue(agpu_renderpass* renderpass, agpu_uint attachment_index, agpu_color4f value)
{
    CHECK_POINTER(renderpass);
    return renderpass->setColorClearValue(attachment_index, value);
}
