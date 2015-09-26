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
	for(int i = 0; i < MaxRenderTargetCount; ++i)
	{
		if(colorBuffers[i])
			colorBuffers[i]->release();
	}

	if(depthStencil)
		depthStencil->release();
}

agpu_framebuffer* _agpu_framebuffer::create(agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint renderTargetCount, agpu_bool hasDepth, agpu_bool hasStencil)
{
	// Create the framebuffer object.
	auto framebuffer = new agpu_framebuffer();
	framebuffer->device = device;
	framebuffer->width = width;
	framebuffer->height = height;
	framebuffer->renderTargetCount = renderTargetCount;
	framebuffer->hasDepth = hasDepth;
	framebuffer->hasStencil = hasStencil;
	return framebuffer;
}

agpu_error _agpu_framebuffer::attachColorBuffer( agpu_int index, agpu_texture* buffer )
{
    buffer->retain();
    if(colorBuffers[index])
        colorBuffers[index]->release();
    colorBuffers[index] = buffer;
    ++dirtyCount;
    return AGPU_OK;
}

agpu_error _agpu_framebuffer::attachDepthStencilBuffer( agpu_texture* buffer )
{
    if(depthStencil)
        depthStencil->release();
    buffer->retain();
    depthStencil = buffer;
    ++dirtyCount;
    return AGPU_OK;
}

void _agpu_framebuffer::bind(GLenum target)
{
    auto context = OpenGLContext::getCurrent();
    auto &allFbos = context->framebufferObjects;
    auto it = allFbos.find(this);
    if(it != allFbos.end())
    {
        auto &data = it->second;
        device->glBindFramebuffer(target, data.first);
        if(data.second != dirtyCount)
        {
            updateAttachments(target);
            data.second = dirtyCount;
        }
    }
    else
    {
        GLuint handle;
        device->glGenFramebuffers(1, &handle);
        allFbos.insert(std::make_pair(this, std::make_pair(handle, dirtyCount)));
        device->glBindFramebuffer(target, handle);
        updateAttachments(target);
    }

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
        fprintf(stderr, "Warning: using incomplete framebuffer.\n");
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

AGPU_EXPORT agpu_error agpuAttachColorBuffer ( agpu_framebuffer* framebuffer, agpu_int index, agpu_texture* buffer )
{
    CHECK_POINTER(framebuffer);
    return framebuffer->attachColorBuffer(index, buffer);
}

AGPU_EXPORT agpu_error agpuAttachDepthStencilBuffer ( agpu_framebuffer* framebuffer, agpu_texture* buffer )
{
    CHECK_POINTER(framebuffer);
    return framebuffer->attachDepthStencilBuffer(buffer);
}
