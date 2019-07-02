#include "texture_view.hpp"
#include "texture.hpp"

namespace AgpuGL
{

GLAbstractTextureView::GLAbstractTextureView(const agpu::device_ref &device, const agpu::texture_ref &texture, const agpu_texture_view_description &description)
    : device(device), texture(texture), description(description)
{
}

GLAbstractTextureView::~GLAbstractTextureView()
{
}

agpu::texture_ptr GLAbstractTextureView::getTexture()
{
    return texture.lock().disown();
}

GLFullTextureView::GLFullTextureView(const agpu::device_ref &device, const agpu::texture_ref &texture, const agpu_texture_view_description &description)
    : GLAbstractTextureView(device, texture, description)
{
}

GLFullTextureView::~GLFullTextureView()
{
}

void GLFullTextureView::activateInSampledSlot(int slotIndex)
{
    auto glTexture = texture.lock().as<GLTexture> ();
    deviceForGL->glActiveTexture(GLenum(GL_TEXTURE0 + slotIndex));
    glBindTexture(glTexture->target, glTexture->handle);
}

void GLFullTextureView::attachToFramebuffer(GLenum target, GLenum attachmentPoint)
{
    auto glTexture = texture.lock().as<GLTexture> ();
    switch(description.type)
    {
    case AGPU_TEXTURE_2D:
		if(glTexture->description.layers > 1)
			deviceForGL->glFramebufferTextureLayer(target, attachmentPoint, glTexture->handle, description.subresource_range.base_miplevel, description.subresource_range.base_arraylayer);
		else
        	deviceForGL->glFramebufferTexture2D(target, attachmentPoint, glTexture->target, glTexture->handle, description.subresource_range.base_miplevel);
        break;
    default:
        abort();
    }
}

} // End of namespace AgpuGL
