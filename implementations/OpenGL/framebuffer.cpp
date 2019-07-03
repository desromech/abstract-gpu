#include "texture.hpp"
#include "texture_view.hpp"
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
    changed = true;
}

GLFramebuffer::~GLFramebuffer()
{
    deviceForGL->onMainContextBlocking([&] {
        deviceForGL->glDeleteFramebuffers(1, &handle);
    });
}

agpu::framebuffer_ref GLFramebuffer::create(const agpu::device_ref &device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu::texture_view_ref* colorViews, const agpu::texture_view_ref &depthStencilView)
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

	if(depthStencilView)
	{
		auto depthStencilFormat = depthStencilView.as<GLAbstractTextureView> ()->description.format;
		framebuffer->hasDepth = hasDepthComponent(depthStencilFormat);
		framebuffer->hasStencil = hasStencilComponent(depthStencilFormat);

	}
	else
	{
		framebuffer->hasDepth = false;
		framebuffer->hasStencil = false;
	}

	framebuffer->colorBufferViews.resize(colorCount);
	framebuffer->colorBufferTextures.resize(colorCount);
	for(size_t i = 0; i < colorCount; ++i)
	{
		auto &view = colorViews[i];
		framebuffer->colorBufferViews[i] = view;
		framebuffer->colorBufferTextures[i] = agpu::texture_ref(view->getTexture());

	}

	if(depthStencilView)
	{
		framebuffer->depthStencilView = depthStencilView;
		framebuffer->depthStencilTexture = agpu::texture_ref(depthStencilView->getTexture());
	}

	return result;
}

void GLFramebuffer::bind(GLenum target)
{
    deviceForGL->glBindFramebuffer(target, handle);
    if(changed)
        updateAttachments(target);
}

void GLFramebuffer::updateAttachments(GLenum target)
{
    for(size_t i = 0; i < colorBufferViews.size(); ++i)
        attachTo(target, colorBufferViews[i], GL_COLOR_ATTACHMENT0 + i);

    if(hasDepth && hasStencil)
        attachTo(target, depthStencilView, GL_DEPTH_STENCIL_ATTACHMENT);
    else if(hasDepth)
        attachTo(target, depthStencilView, GL_DEPTH_ATTACHMENT);
    else if(hasStencil)
        attachTo(target, depthStencilView, GL_STENCIL_ATTACHMENT);

    auto status = deviceForGL->glCheckFramebufferStatus(target);
    if(status != GL_FRAMEBUFFER_COMPLETE)
	{
        printError("Warning: %s incomplete framebuffer color: %d hasDepth: %d hasStencil: %d\n", framebufferStatusToString(status), (int)colorBufferViews.size(), hasDepth, hasStencil);
	}
	changed = false;
}

void GLFramebuffer::attachTo(GLenum target, const agpu::texture_view_ref &view, GLenum attachmentPoint)
{
	if(!view)
		return;

	auto glView = view.as<GLAbstractTextureView> ();
	//printf("texture width: %d height: %d\n", glTexture->description.width, glTexture->description.height);
	//printf("view->subresource_range.base_miplevel: %d\n", view->subresource_range.base_miplevel);
	glView->attachToFramebuffer(target, attachmentPoint);
}

} // End of namespace AgpuGL
