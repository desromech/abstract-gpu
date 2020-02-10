#include "framebuffer.hpp"
#include "texture.hpp"
#include "texture_view.hpp"
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

agpu::framebuffer_ref AMtlFramebuffer::create(const agpu::device_ref &device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu::texture_view_ref *colorViews, const agpu::texture_view_ref &depthStencilView)
{
    if(colorCount > 0 && !colorViews)
        return agpu::framebuffer_ref();;

    // Check the presence of the color buffers.
    for(int i = 0; i < colorCount; ++i)
    {
        if(!colorViews[0])
            return agpu::framebuffer_ref();
    }

    // Create the framebuffer object.
    auto result = agpu::makeObject<AMtlFramebuffer> (device);
    auto framebuffer = result.as<AMtlFramebuffer> ();

    // Add the color buffer references.
    framebuffer->colorBuffers.resize(colorCount);
    framebuffer->colorBufferViews.resize(colorCount);
    for(int i = 0; i < colorCount; ++i)
    {
        auto &view = colorViews[i];

        framebuffer->colorBufferViews[i] = view;
        framebuffer->colorBuffers[i] = agpu::texture_ref(view->getTexture());
    }

    // Add the depth stencil references.
    if(depthStencilView)
    {
        // TODO: Add the depth and the stencill attachments.
        framebuffer->depthStencilBufferView = depthStencilView;
        framebuffer->depthStencilBuffer = agpu::texture_ref(depthStencilView->getTexture());
    }

    return result;
}

agpu::framebuffer_ref AMtlFramebuffer::createForSwapChain(const agpu::device_ref &device, agpu_uint width, agpu_uint height, const agpu::texture_view_ref &depthStencilView)
{
    // Create the framebuffer object.
    auto result = agpu::makeObject<AMtlFramebuffer> (device);
    auto framebuffer = result.as<AMtlFramebuffer> ();

    // Add the depth stencil references.
    if(depthStencilView)
    {
        // TODO: Add the depth and the stencill attachments.
        framebuffer->depthStencilBufferView = depthStencilView;
        framebuffer->depthStencilBuffer = agpu::texture_ref(depthStencilView->getTexture());
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

id<MTLDrawable> AMtlFramebuffer::borrowDrawable()
{
    auto result = drawable;
    drawable = nil;
    return result;
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
