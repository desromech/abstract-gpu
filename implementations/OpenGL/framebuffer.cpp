#include "texture.hpp"
#include "framebuffer.hpp"

_agpu_framebuffer::_agpu_framebuffer()
{
	for(int i = 0; i < MaxRenderTargetCount; ++i)
		colorBuffers[i] = nullptr;
	depthStencil = nullptr;
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

void _agpu_framebuffer::bind()
{
	// Bind this
    // TODO: Make a FBO for the current context
	//device->glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
