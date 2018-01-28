#include "renderpass.hpp"

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
    result->colorAttachments.resize(description->color_attachment_count);
    for (size_t i = 0; i < description->color_attachment_count; ++i)
        result->colorAttachments[i] = description->color_attachments[i];
    result->hasDepthStencil = description->depth_stencil_attachment != nullptr;
    if (result->hasDepthStencil)
        result->depthStencilAttachment = *description->depth_stencil_attachment;
    return result;
}

void _agpu_renderpass::started()
{
    GLbitfield buffers = 0;

    for (auto &colorAttachment : colorAttachments)
    {
        if (colorAttachment.begin_action == AGPU_ATTACHMENT_CLEAR)
        {
            buffers |= GL_COLOR_BUFFER_BIT;
            glClearColor(colorAttachment.clear_value.r, colorAttachment.clear_value.g, colorAttachment.clear_value.b, colorAttachment.clear_value.a);
        }
    }

    if (hasDepthStencil)
    {
        if (depthStencilAttachment.begin_action == AGPU_ATTACHMENT_CLEAR)
        {
            buffers |= GL_DEPTH_BUFFER_BIT;
            glClearDepth(depthStencilAttachment.clear_value.depth);
        }

        if (depthStencilAttachment.stencil_begin_action == AGPU_ATTACHMENT_CLEAR)
        {
            buffers |= GL_STENCIL_BUFFER_BIT;
            glClearStencil(depthStencilAttachment.clear_value.stencil);
        }
    }

    glClear(buffers);
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

agpu_error _agpu_renderpass::setColorClearValueFrom ( agpu_uint attachment_index, agpu_color4f* value )
{
    CHECK_POINTER(value);
    return setColorClearValue(attachment_index, *value);
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

AGPU_EXPORT agpu_error agpuSetColorClearValueFrom ( agpu_renderpass* renderpass, agpu_uint attachment_index, agpu_color4f* value )
{
    CHECK_POINTER(renderpass);
    return renderpass->setColorClearValueFrom(attachment_index, value);
}
