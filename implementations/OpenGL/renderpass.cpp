#include "renderpass.hpp"

namespace AgpuGL
{

GLRenderPass::GLRenderPass(const agpu::device_ref &cdevice)
    : device(cdevice)
{
}

GLRenderPass::~GLRenderPass()
{
}

agpu::renderpass_ref GLRenderPass::create(const agpu::device_ref &device, agpu_renderpass_description *description)
{
    if (!description)
        return agpu::renderpass_ref();

    auto result = agpu::makeObject<GLRenderPass> (device);
    auto renderpass = result.as<GLRenderPass> ();

    renderpass->sampleCount = 1;
    renderpass->sampleQuality = 0;

    renderpass->colorAttachments.resize(description->color_attachment_count);
    for (size_t i = 0; i < description->color_attachment_count; ++i)
    {
        auto &attachment = description->color_attachments[i];
        renderpass->colorAttachments[i] = attachment;
        renderpass->sampleCount = attachment.sample_count;
        renderpass->sampleQuality = attachment.sample_quality;
    }
    renderpass->hasDepthStencil = description->depth_stencil_attachment != nullptr;
    if (renderpass->hasDepthStencil)
    {
        auto &attachment = *description->depth_stencil_attachment;
        renderpass->depthStencilAttachment = attachment;
        renderpass->sampleCount = attachment.sample_count;
        renderpass->sampleQuality = attachment.sample_quality;
    }
    return result;
}

void GLRenderPass::started()
{
    GLbitfield buffers = 0;

    for (auto &colorAttachment : colorAttachments)
    {
        if (colorAttachment.begin_action == AGPU_ATTACHMENT_CLEAR)
        {
            buffers |= GL_COLOR_BUFFER_BIT;
            glClearColor(colorAttachment.clear_value.r, colorAttachment.clear_value.g, colorAttachment.clear_value.b, colorAttachment.clear_value.a);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        }
    }

    if (hasDepthStencil)
    {
        if (depthStencilAttachment.begin_action == AGPU_ATTACHMENT_CLEAR)
        {
            buffers |= GL_DEPTH_BUFFER_BIT;
            glClearDepth(depthStencilAttachment.clear_value.depth);
            glDepthMask(GL_TRUE);
        }

        if (depthStencilAttachment.stencil_begin_action == AGPU_ATTACHMENT_CLEAR)
        {
            buffers |= GL_STENCIL_BUFFER_BIT;
            glClearStencil(depthStencilAttachment.clear_value.stencil);
            glStencilMask(GL_TRUE);
        }
    }

    glClear(buffers);
}

agpu_error GLRenderPass::setDepthStencilClearValue(agpu_depth_stencil_value value)
{
    depthStencilAttachment.clear_value = value;
    return AGPU_OK;
}

agpu_error GLRenderPass::setColorClearValue(agpu_uint attachment_index, agpu_color4f value)
{
    if (attachment_index >= colorAttachments.size())
        return AGPU_OUT_OF_BOUNDS;

    colorAttachments[attachment_index].clear_value = value;
    return AGPU_OK;
}

agpu_error GLRenderPass::setColorClearValueFrom ( agpu_uint attachment_index, agpu_color4f* value )
{
    CHECK_POINTER(value);
    return setColorClearValue(attachment_index, *value);
}

agpu_error GLRenderPass::getColorAttachmentFormats(agpu_uint* color_attachment_count, agpu_texture_format* formats)
{
    CHECK_POINTER(color_attachment_count);
    if(formats)
    {
        if(*color_attachment_count < colorAttachments.size())
            return AGPU_INVALID_PARAMETER;

        for(size_t i = 0; i < colorAttachments.size(); ++i)
            formats[i] = colorAttachments[i].format;
    }

    *color_attachment_count = colorAttachments.size();
    return AGPU_OK;
}

agpu_texture_format GLRenderPass::getDepthStencilAttachmentFormat()
{
    return hasDepthStencil ? depthStencilAttachment.format : AGPU_TEXTURE_FORMAT_UNKNOWN;
}

agpu_uint GLRenderPass::getSampleCount()
{
    return sampleCount;
}

agpu_uint GLRenderPass::getSampleQuality()
{
    return sampleQuality;
}

} // End of namespace AgpuGL
