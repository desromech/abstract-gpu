#include "texture.hpp"
#include "framebuffer.hpp"
#include "../Common/texture_formats_common.hpp"
#include <string.h>

inline const char *framebufferStatusToString(GLenum status)
{
	switch(status)
	{
#define MAP_ENUM_NAME(x) case x: return #x;
MAP_ENUM_NAME(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
MAP_ENUM_NAME(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
MAP_ENUM_NAME(GL_FRAMEBUFFER_UNSUPPORTED)
#undef MAP_ENUM_NAME
	default: return "unknown;";
	}
}
_agpu_framebuffer::_agpu_framebuffer()
{
	memset(colorBuffers, 0, sizeof(colorBuffers));
	memset(&depthStencil, 0, sizeof(depthStencil));
    changed = true;
}

void _agpu_framebuffer::lostReferences()
{
    device->onMainContextBlocking([&] {
        device->glDeleteFramebuffers(1, &handle);
    });

	for(int i = 0; i < MaxRenderTargetCount; ++i)
	{
		if(colorBuffers[i].texture)
			colorBuffers[i].texture->release();
	}

	if(depthStencil.texture)
		depthStencil.texture->release();
}

agpu_framebuffer* _agpu_framebuffer::create(agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView)
{
    if (!colorViews && colorCount > 0)
        return nullptr;

	// Create the framebuffer object.
    GLuint handle;
    device->onMainContextBlocking([&] {
        device->glGenFramebuffers(1, &handle);
    });

	auto framebuffer = new agpu_framebuffer();
    framebuffer->handle = handle;
	framebuffer->device = device;
	framebuffer->width = width;
	framebuffer->height = height;
	framebuffer->renderTargetCount = colorCount;
	framebuffer->hasDepth = depthStencilView != nullptr && hasDepthComponent(depthStencilView->format);
	framebuffer->hasStencil = depthStencilView != nullptr && hasStencilComponent(depthStencilView->format);

	if(colorViews)
	{
		for (agpu_size i = 0; i < colorCount; ++i)
	        framebuffer->attachColorBuffer(i, &colorViews[i]);
	}
	if(depthStencilView)
    	framebuffer->attachDepthStencilBuffer(depthStencilView);

	return framebuffer;
}

agpu_error _agpu_framebuffer::attachColorBuffer( agpu_int index, agpu_texture_view_description* colorView)
{
    CHECK_POINTER(colorView);
    CHECK_POINTER(colorView->texture);

    auto buffer = colorView->texture;
    buffer->retain();
    if(colorBuffers[index].texture)
        colorBuffers[index].texture->release();
    colorBuffers[index] = *colorView;
    changed = true;
    return AGPU_OK;
}

agpu_error _agpu_framebuffer::attachDepthStencilBuffer(agpu_texture_view_description* depthStencilView)
{
    CHECK_POINTER(depthStencilView);
    CHECK_POINTER(depthStencilView->texture);

    auto buffer = depthStencilView->texture;
    if(depthStencil.texture)
        depthStencil.texture->release();
    buffer->retain();
    depthStencil = *depthStencilView;
    changed = true;
    return AGPU_OK;
}

void _agpu_framebuffer::bind(GLenum target)
{
    device->glBindFramebuffer(target, handle);
    if(changed)
        updateAttachments(target);
}

void _agpu_framebuffer::updateAttachments(GLenum target)
{
    for(int i = 0; i < renderTargetCount; ++i)
        attachTo(target, &colorBuffers[i], GL_COLOR_ATTACHMENT0 + i);

    if(hasDepth && hasStencil)
        attachTo(target, &depthStencil, GL_DEPTH_STENCIL_ATTACHMENT);
    else if(hasDepth)
        attachTo(target, &depthStencil, GL_DEPTH_ATTACHMENT);
    else if(hasStencil)
        attachTo(target, &depthStencil, GL_STENCIL_ATTACHMENT);

    auto status = device->glCheckFramebufferStatus(target);
    if(status != GL_FRAMEBUFFER_COMPLETE)
	{
        printError("Warning: %s incomplete framebuffer color: %d hasDepth: %d hasStencil: %d\n", framebufferStatusToString(status), renderTargetCount, hasDepth, hasStencil);
	}
	changed = false;
}

void _agpu_framebuffer::attachTo(GLenum target, agpu_texture_view_description *view, GLenum attachmentPoint)
{
	if(!view)
		return;

	auto texture = view->texture;
	//printf("texture width: %d height: %d\n", texture->description.width, texture->description.height);
	//printf("view->subresource_range.base_miplevel: %d\n", view->subresource_range.base_miplevel);
    switch(texture->description.type)
    {
    case AGPU_TEXTURE_2D:
		if(texture->description.depthOrArraySize > 1)
			device->glFramebufferTextureLayer(target, attachmentPoint, texture->handle, view->subresource_range.base_miplevel, view->subresource_range.base_arraylayer);
		else
        	device->glFramebufferTexture2D(target, attachmentPoint, texture->target, texture->handle, view->subresource_range.base_miplevel);
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
