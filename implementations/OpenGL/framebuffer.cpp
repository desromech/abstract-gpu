#include "texture.hpp"
#include "framebuffer.hpp"

_agpu_framebuffer::_agpu_framebuffer()
{
	for(int i = 0; i < MaxRenderTargetCount; ++i)
		colorBuffers[i] = nullptr;
	depthStencil = nullptr;
    dirtyCount = 0;
}

void _agpu_framebuffer::lostReferences()
{
    device->allContextDo([&](OpenGLContext *context) {
        context->framebufferDeleted(this);
    });

	for(int i = 0; i < MaxRenderTargetCount; ++i)
	{
		if(colorBuffers[i])
			colorBuffers[i]->release();
	}

	if(depthStencil)
		depthStencil->release();
}

agpu_framebuffer* _agpu_framebuffer::create(agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView)
{
    if (!colorViews)
        return nullptr;

    if (!depthStencilView)
        return nullptr;

	// Create the framebuffer object.
	auto framebuffer = new agpu_framebuffer();
	framebuffer->device = device;
	framebuffer->width = width;
	framebuffer->height = height;
	framebuffer->renderTargetCount = colorCount;
	framebuffer->hasDepth = depthStencilView != nullptr;
	framebuffer->hasStencil = depthStencilView != nullptr;

    for (agpu_size i = 0; i < colorCount; ++i)
        framebuffer->attachColorBuffer(i, &colorViews[i]);
    framebuffer->attachDepthStencilBuffer(depthStencilView);

	return framebuffer;
}

agpu_error _agpu_framebuffer::attachColorBuffer( agpu_int index, agpu_texture_view_description* colorView)
{
    CHECK_POINTER(colorView);
    CHECK_POINTER(colorView->texture);

    auto buffer = colorView->texture;
    buffer->retain();
    if(colorBuffers[index])
        colorBuffers[index]->release();
    colorBuffers[index] = buffer;
    ++dirtyCount;
    return AGPU_OK;
}

agpu_error _agpu_framebuffer::attachDepthStencilBuffer(agpu_texture_view_description* depthStencilView)
{
    CHECK_POINTER(depthStencilView);
    CHECK_POINTER(depthStencilView->texture);

    auto buffer = depthStencilView->texture;
    if(depthStencil)
        depthStencil->release();
    buffer->retain();
    depthStencil = buffer;
    ++dirtyCount;
    return AGPU_OK;
}

void _agpu_framebuffer::bind(GLenum target)
{
    // Get the FBO present in the current context.
    auto context = OpenGLContext::getCurrent();
    auto handleChangedPair = context->getFrameBufferObject(this, dirtyCount);
    auto handle = handleChangedPair.first;
    auto changed = handleChangedPair.second;

    device->glBindFramebuffer(target, handle);
    if(changed)
        updateAttachments(target);
}

void _agpu_framebuffer::updateAttachments(GLenum target)
{
    for(int i = 0; i < renderTargetCount; ++i)
        attachTo(target, colorBuffers[i], GL_COLOR_ATTACHMENT0 + i);

    if(hasDepth && hasStencil)
        attachTo(target, depthStencil, GL_DEPTH_STENCIL_ATTACHMENT);
    else if(hasDepth)
        attachTo(target, depthStencil, GL_DEPTH_ATTACHMENT);
    else if(hasStencil)
        attachTo(target, depthStencil, GL_STENCIL_ATTACHMENT);

    auto status = device->glCheckFramebufferStatus(target);
    if(status != GL_FRAMEBUFFER_COMPLETE)
        printError("Warning: using incomplete framebuffer.\n");
}

void _agpu_framebuffer::attachTo(GLenum target, agpu_texture *attachment, GLenum attachmentPoint)
{
    if(!attachment)
        return;

    switch(attachment->description.type)
    {
    case AGPU_TEXTURE_2D:
        device->glFramebufferTexture2D(target, attachmentPoint, attachment->target, attachment->handle, 0);
        break;
    default:
        abort();
    }

}

// Exported C api

AGPU_EXPORT agpu_error agpuAddFramebufferReference ( agpu_framebuffer* framebuffer )
{
	CHECK_POINTER(framebuffer);
	return framebuffer->retain();
}

AGPU_EXPORT agpu_error agpuReleaseFramebuffer ( agpu_framebuffer* framebuffer )
{
	CHECK_POINTER(framebuffer);
	return framebuffer->release();
}
