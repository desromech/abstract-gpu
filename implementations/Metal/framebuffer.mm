#include "framebuffer.hpp"
#include "texture.hpp"

namespace AgpuMetal
{
    
AMtlFramebuffer::AMtlFramebuffer(const agpu::device_ref &device)
    : device(device)
{
    ownedBySwapChain = false;
    drawable = nil;
}

AMtlFramebuffer::~AMtlFramebuffer()
{
}

agpu::framebuffer_ref AMtlFramebuffer::create(const agpu::device_ref &device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView )
{
    if(colorCount > 0 && !colorViews)
        return agpu::framebuffer_ref();;

    // Check the presence of the color buffers.
    for(int i = 0; i < colorCount; ++i)
    {
        if(!colorViews[0].texture)
            return agpu::framebuffer_ref();
    }

    // If there is a depth stencil view, it has to have a texture.
    if(depthStencilView && !depthStencilView->texture)
        return agpu::framebuffer_ref();

    // Create the framebuffer object.
    auto result = agpu::makeObject<AMtlFramebuffer> (device);
    auto framebuffer = result.as<AMtlFramebuffer> ();

    // Add the color buffer references.
    framebuffer->colorBuffers.resize(colorCount);
    framebuffer->colorBufferDescriptions.resize(colorCount);
    for(int i = 0; i < colorCount; ++i)
    {
        auto &view = colorViews[i];

        framebuffer->colorBuffers[i] = agpu::texture_ref::import(view.texture);
        framebuffer->colorBufferDescriptions[i] = view;
    }

    // Add the depth stencil references.
    if(depthStencilView)
    {
        // TODO: Add the depth and the stencill attachments.
        framebuffer->depthStencilBuffer = agpu::texture_ref::import(depthStencilView->texture);
        framebuffer->depthStencilBufferDescription = *depthStencilView;
    }

    return result;
}

agpu::framebuffer_ref AMtlFramebuffer::createForSwapChain(const agpu::device_ref &device, agpu_uint width, agpu_uint height, agpu_texture_view_description* depthStencilView)
{
    // Create the framebuffer object.
    auto result = agpu::makeObject<AMtlFramebuffer> (device);
    auto framebuffer = result.as<AMtlFramebuffer> ();

    // Add the depth stencil references.
    if(depthStencilView)
    {
        // TODO: Add the depth and the stencill attachments.
        framebuffer->depthStencilBuffer = agpu::texture_ref::import(depthStencilView->texture);
        framebuffer->depthStencilBufferDescription = *depthStencilView;
    }

    framebuffer->ownedBySwapChain = true;
    return result;
}

void AMtlFramebuffer::releaseDrawable()
{
    if(drawable)
        [drawable release];
    drawable = nil;
}

void AMtlFramebuffer::setDrawable(id<MTLDrawable> drawable, id<MTLTexture> drawableTexture)
{
    this->drawable = drawable;
    this->drawableTexture = drawableTexture;
}

id<MTLTexture> AMtlFramebuffer::getColorTexture(agpu_uint index)
{
    if(ownedBySwapChain)
        return drawableTexture;
    return colorBuffers[index].as<AMtlTexture> ()->handle;
}

} // End of namespace AgpuMetal
