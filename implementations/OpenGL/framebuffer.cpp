#include "framebuffer.hpp"

_agpu_framebuffer::_agpu_framebuffer()
{
	isMainFrameBuffer = false;
	
}

void _agpu_framebuffer::lostReferences()
{
	if(handle)
		device->glDeleteFramebuffers(1, &handle);
}

agpu_framebuffer* _agpu_framebuffer::create(agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint renderTargetCount, agpu_bool hasDepth, agpu_bool hasStencil)
{
	// Create the handle.
	GLuint handle;
	device->glGenFramebuffers(1, &handle);
	
	// Create the framebuffer object.
	auto framebuffer = new agpu_framebuffer();
	framebuffer->device = device;
	framebuffer->handle = handle;
	framebuffer->width = width;
	framebuffer->height = height;
	framebuffer->renderTargetCount = renderTargetCount;
	framebuffer->hasDepth = hasDepth;
	framebuffer->hasStencil = hasStencil;
	return framebuffer;
}

agpu_framebuffer* _agpu_framebuffer::createMain(agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint renderTargetCount, agpu_bool hasDepth, agpu_bool hasStencil)
{
	auto framebuffer = new agpu_framebuffer();
	framebuffer->device = device;
	framebuffer->handle = 0;
	framebuffer->isMainFrameBuffer = true;
	framebuffer->width = width;
	framebuffer->height = height;
	framebuffer->renderTargetCount = renderTargetCount;
	framebuffer->hasDepth = hasDepth;
	framebuffer->hasStencil = hasStencil;
	return framebuffer;
}

agpu_error _agpu_framebuffer::createImplicitDepthStencil(agpu_uint depthSize, agpu_uint stencilSize)
{
	return AGPU_UNIMPLEMENTED;
}

void _agpu_framebuffer::bind()
{
	// Bind this
	device->glBindFramebuffer(GL_FRAMEBUFFER, handle);
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

AGPU_EXPORT agpu_bool agpuIsMainFrameBuffer ( agpu_framebuffer* framebuffer )
{
	if(!framebuffer)
		return false;
	return framebuffer->isMainFrameBuffer;
}
