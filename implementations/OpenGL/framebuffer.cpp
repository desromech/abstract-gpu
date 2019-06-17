#include "texture.hpp"
#include "framebuffer.hpp"
#include "../Common/texture_formats_common.hpp"
#include <string.h>

namespace AgpuGL
{

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
GLFramebuffer::GLFramebuffer()
{
	memset(colorBuffers, 0, sizeof(colorBuffers));
	memset(&depthStencil, 0, sizeof(depthStencil));
    changed = true;
}

GLFramebuffer::~GLFramebuffer()
{
    deviceForGL->onMainContextBlocking([&] {
        deviceForGL->glDeleteFramebuffers(1, &handle);
    });
}

agpu::framebuffer_ref GLFramebuffer::create(const agpu::device_ref &device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView)
{
    if (!colorViews && colorCount > 0)
        return agpu::framebuffer_ref();

	// Create the framebuffer object.
    GLuint handle;
    deviceForGL->onMainContextBlocking([&] {
        deviceForGL->glGenFramebuffers(1, &handle);
    });

	auto result = agpu::makeObject<GLFramebuffer> ();
	auto framebuffer = result.as<GLFramebuffer> ();
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

	return result;
}

agpu_error GLFramebuffer::attachColorBuffer( agpu_int index, agpu_texture_view_description* colorView)
{
    CHECK_POINTER(colorView);
    CHECK_POINTER(colorView->texture);

    colorBuffers[index] = *colorView;
	colorBufferTextures[index] = agpu::texture_ref::import(colorView->texture);

    changed = true;
    return AGPU_OK;
}

agpu_error GLFramebuffer::attachDepthStencilBuffer(agpu_texture_view_description* depthStencilView)
{
    CHECK_POINTER(depthStencilView);
    CHECK_POINTER(depthStencilView->texture);

    depthStencil = *depthStencilView;
	depthStencilTexture = agpu::texture_ref::import(depthStencilView->texture);

    changed = true;
    return AGPU_OK;
}

void GLFramebuffer::bind(GLenum target)
{
    deviceForGL->glBindFramebuffer(target, handle);
    if(changed)
        updateAttachments(target);
}

void GLFramebuffer::updateAttachments(GLenum target)
{
    for(int i = 0; i < renderTargetCount; ++i)
        attachTo(target, &colorBuffers[i], colorBufferTextures[i], GL_COLOR_ATTACHMENT0 + i);

    if(hasDepth && hasStencil)
        attachTo(target, &depthStencil, depthStencilTexture, GL_DEPTH_STENCIL_ATTACHMENT);
    else if(hasDepth)
        attachTo(target, &depthStencil, depthStencilTexture, GL_DEPTH_ATTACHMENT);
    else if(hasStencil)
        attachTo(target, &depthStencil, depthStencilTexture, GL_STENCIL_ATTACHMENT);

    auto status = deviceForGL->glCheckFramebufferStatus(target);
    if(status != GL_FRAMEBUFFER_COMPLETE)
	{
        printError("Warning: %s incomplete framebuffer color: %d hasDepth: %d hasStencil: %d\n", framebufferStatusToString(status), renderTargetCount, hasDepth, hasStencil);
	}
	changed = false;
}

void GLFramebuffer::attachTo(GLenum target, agpu_texture_view_description *view, const agpu::texture_ref &texture, GLenum attachmentPoint)
{
	if(!view)
		return;

	auto glTexture = texture.as<GLTexture> ();
	//printf("texture width: %d height: %d\n", glTexture->description.width, glTexture->description.height);
	//printf("view->subresource_range.base_miplevel: %d\n", view->subresource_range.base_miplevel);

    switch(glTexture->description.type)
    {
    case AGPU_TEXTURE_2D:
		if(glTexture->description.layers > 1)
			deviceForGL->glFramebufferTextureLayer(target, attachmentPoint, glTexture->handle, view->subresource_range.base_miplevel, view->subresource_range.base_arraylayer);
		else
        	deviceForGL->glFramebufferTexture2D(target, attachmentPoint, glTexture->target, glTexture->handle, view->subresource_range.base_miplevel);
        break;
    default:
        abort();
    }

}

} // End of namespace AgpuGL
