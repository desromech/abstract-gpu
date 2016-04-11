#include "swap_chain.hpp"
#include "framebuffer.hpp"
#include "command_queue.hpp"
#include "texture_format.hpp"
#include "texture.hpp"

@implementation AGPUSwapChainView
-(AGPUSwapChainView*) initWithSwapChain: (agpu_swap_chain*)theSwapChain
{
    swapChain = theSwapChain;
    auto &swapChainInfo = swapChain->swapChainInfo;

    if(![self initWithFrame: NSMakeRect(0, 0, swapChainInfo.width, swapChainInfo.height)])
        return nil;

    auto metalLayer = [CAMetalLayer new];
    metalLayer.device = theSwapChain->device->device;
    metalLayer.framebufferOnly = YES;
    metalLayer.drawableSize = NSMakeSize(swapChainInfo.width, swapChainInfo.height);
    metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    [self setLayer: metalLayer];
    [self setWantsLayer: YES];
    swapChain->metalLayer = metalLayer;
    return self;
}

@end

_agpu_swap_chain::_agpu_swap_chain(agpu_device *device)
    : device(device)
{
    window = nil;
    view = nil;
    metalLayer = nil;
    presentQueue = nullptr;
}

void _agpu_swap_chain::lostReferences()
{
    if(window)
        [window release];
    if(presentQueue)
        presentQueue->release();

    for(auto &framebuffer : framebuffers)
        framebuffer->release();
}

agpu_swap_chain *_agpu_swap_chain::create(agpu_device *device, agpu_command_queue *presentQueue, agpu_swap_chain_create_info *createInfo)
{
    if(!createInfo || !createInfo->window)
        return nullptr;

    std::unique_ptr<agpu_swap_chain> result(new agpu_swap_chain(device));
    auto window = (NSWindow*)createInfo->window;
    result->window = window;
    result->swapChainInfo = *createInfo;

    // Create the view
    result->view = [[AGPUSwapChainView alloc] initWithSwapChain: result.get()];
    if(result->view == nil)
        return nullptr;

    window.contentView = result->view;

    // Depth stencil buffer descriptions.
    agpu_texture_description depthStencilDesc;
    memset(&depthStencilDesc, 0, sizeof(depthStencilDesc));
    depthStencilDesc.type = AGPU_TEXTURE_2D;
    depthStencilDesc.width = createInfo->width;
    depthStencilDesc.height = createInfo->height;
    depthStencilDesc.depthOrArraySize = 1;
    depthStencilDesc.format = createInfo->depth_stencil_format;
    depthStencilDesc.flags = agpu_texture_flags(AGPU_TEXTURE_FLAG_RENDERBUFFER_ONLY);
    depthStencilDesc.miplevels = 1;

    bool hasDepth = hasDepthComponent(createInfo->depth_stencil_format);
    bool hasStencil = hasStencilComponent(createInfo->depth_stencil_format);
    if (hasDepth)
        depthStencilDesc.flags = agpu_texture_flags(depthStencilDesc.flags | AGPU_TEXTURE_FLAG_DEPTH);
    if (hasStencil)
        depthStencilDesc.flags = agpu_texture_flags(depthStencilDesc.flags | AGPU_TEXTURE_FLAG_STENCIL);

    agpu_texture_view_description depthStencilViewDesc;
    auto depthStencilViewPointer = &depthStencilViewDesc;
    if (!hasDepth && !hasStencil)
        depthStencilViewPointer = nullptr;

    // Create the framebuffers
    result->framebuffers.reserve(createInfo->buffer_count);
    bool failure = false;
    for(agpu_uint i = 0; i < createInfo->buffer_count; ++i)
    {
        // Create the depth stencil buffer
        agpu_texture *depthStencilBuffer = nullptr;
        if(hasDepth || hasStencil)
        {
            depthStencilBuffer = agpu_texture::create(device, &depthStencilDesc);
            if (!depthStencilBuffer)
            {
                failure = true;
                break;
            }

            // Get the depth stencil buffer view description.
            depthStencilBuffer->getFullViewDescription(&depthStencilViewDesc);
        }

        // Create the framebuffer
        auto framebuffer = agpu_framebuffer::createForSwapChain(device, createInfo->width, createInfo->height, depthStencilViewPointer);
        if(depthStencilBuffer)
            depthStencilBuffer->release();

        if(!framebuffer)
        {
            failure = true;
            break;
        }

        result->framebuffers.push_back(framebuffer);
        result->presentCommands.push_back(nil);
    }

    // Set the drawable of the first framebuffer
    if(!failure)
    {
        auto drawable = [result->metalLayer nextDrawable];
        if(drawable)
        {
            result->currentFramebufferIndex = 0;
            result->framebuffers[result->currentFramebufferIndex]->setDrawable(drawable);
        }
        else
        {
            failure = true;
        }
    }

    if(failure)
    {
        for(auto fb : result->framebuffers)
        {
            if(fb)
                fb->release();
        }

        return nullptr;
    }

    result->presentQueue = presentQueue;
    result->presentQueue->retain();
    return result.release();
}

agpu_error _agpu_swap_chain::swapBuffers (  )
{
    auto currentFramebuffer = framebuffers[currentFramebufferIndex];
    auto &presentCommand = presentCommands[currentFramebufferIndex];
    if(presentCommand)
        [presentCommand release];
    presentCommand = [presentQueue->handle commandBuffer];
    [presentCommand presentDrawable: currentFramebuffer->drawable];
    [presentCommand commit];

    // Release the old drawable
    currentFramebufferIndex = (currentFramebufferIndex + 1) % framebuffers.size();
    framebuffers[currentFramebufferIndex]->releaseDrawable();

    auto drawable = [metalLayer nextDrawable];
    framebuffers[currentFramebufferIndex]->setDrawable(drawable);
    if(!drawable)
        return AGPU_ERROR;

    return AGPU_OK;
}

agpu_framebuffer* _agpu_swap_chain::getCurrentBackBuffer (  )
{
    auto result = framebuffers[currentFramebufferIndex];
    result->retain();
    return result;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddSwapChainReference ( agpu_swap_chain* swap_chain )
{
    CHECK_POINTER(swap_chain);
    return swap_chain->retain();
}

AGPU_EXPORT agpu_error agpuReleaseSwapChain ( agpu_swap_chain* swap_chain )
{
    CHECK_POINTER(swap_chain);
    return swap_chain->release();
}

AGPU_EXPORT agpu_error agpuSwapBuffers ( agpu_swap_chain* swap_chain )
{
    CHECK_POINTER(swap_chain)
    return swap_chain->swapBuffers();
}

AGPU_EXPORT agpu_framebuffer* agpuGetCurrentBackBuffer ( agpu_swap_chain* swap_chain )
{
    if(!swap_chain)
        return nullptr;
    return swap_chain->getCurrentBackBuffer();
}
