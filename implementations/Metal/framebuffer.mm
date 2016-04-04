#include "framebuffer.hpp"
#include "texture.hpp"

_agpu_framebuffer::_agpu_framebuffer(agpu_device *device)
    : device(device)
{
    depthStencilBuffer = nullptr;
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

    // Create the render pass descriptor
    result->renderPass = [MTLRenderPassDescriptor renderPassDescriptor];
    auto &renderPass = result->renderPass;

    // Add the color buffer references.
    result->colorBuffers.resize(colorCount);
    for(int i = 0; i < colorCount; ++i)
    {
        auto &view = colorViews[i];
        renderPass.colorAttachments[i].texture = view.texture->handle;
        renderPass.colorAttachments[i].clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 0);
        renderPass.colorAttachments[i].storeAction = MTLStoreActionStore;
        renderPass.colorAttachments[i].loadAction = MTLLoadActionLoad;

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
