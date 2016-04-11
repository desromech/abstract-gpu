#include "framebuffer.hpp"
#include "texture.hpp"

_agpu_framebuffer::_agpu_framebuffer(agpu_device *device)
    : device(device)
{
    depthStencilBuffer = nullptr;
    ownedBySwapChain = false;
    drawable = nil;
}

void _agpu_framebuffer::lostReferences()
{
    for(auto &colorBuffer : colorBuffers)
    {
        if(colorBuffer)
            colorBuffer->release();
    }

    if(depthStencilBuffer)
        depthStencilBuffer->release();
}

agpu_framebuffer* _agpu_framebuffer::create ( agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView )
{
    if(colorCount > 0 && !colorViews)
        return nullptr;

    // Check the presence of the color buffers.
    for(int i = 0; i < colorCount; ++i)
    {
        if(!colorViews[0].texture)
            return nullptr;
    }

    // If there is a depth stencil view, it has to have a texture.
    if(depthStencilView && !depthStencilView->texture)
        return nullptr;

    // Create the framebuffer object.
    auto result = new agpu_framebuffer(device);

    // Add the color buffer references.
    result->colorBuffers.resize(colorCount);
    for(int i = 0; i < colorCount; ++i)
    {
        auto &view = colorViews[i];

        view.texture->retain();
        result->colorBuffers.push_back(view.texture);
    }

    // Add the depth stencil references.
    if(depthStencilView)
    {
        // TODO: Add the depth and the stencill attachments.
        result->depthStencilBuffer = depthStencilView->texture;
        result->depthStencilBuffer->retain();
    }

    return result;
}

agpu_framebuffer* _agpu_framebuffer::createForSwapChain ( agpu_device* device, agpu_uint width, agpu_uint height, agpu_texture_view_description* depthStencilView )
{
    // Create the framebuffer object.
    auto result = new agpu_framebuffer(device);

    // Add the depth stencil references.
    if(depthStencilView)
    {
        // TODO: Add the depth and the stencill attachments.
        result->depthStencilBuffer = depthStencilView->texture;
        result->depthStencilBuffer->retain();
    }

    result->ownedBySwapChain = true;
    return result;
}

void _agpu_framebuffer::releaseDrawable()
{
    if(drawable)
        [drawable release];
    drawable = nil;
}

void _agpu_framebuffer::setDrawable(id<MTLDrawable> drawable, id<MTLTexture> drawableTexture)
{
    this->drawable = drawable;
    this->drawableTexture = drawableTexture;
}

id<MTLTexture> _agpu_framebuffer::getColorTexture(agpu_uint index)
{
    if(ownedBySwapChain)
        return drawableTexture;
    return colorBuffers[index]->handle;
}

// The exported C interface
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
