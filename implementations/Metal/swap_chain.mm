#include "swap_chain.hpp"
#include "framebuffer.hpp"
#include "command_queue.hpp"
#include "texture_format.hpp"
#include "texture.hpp"

@implementation AGPUSwapChainView
-(AGPUSwapChainView*) initWithSwapChain: (AgpuMetal::AMtlSwapChain*)theSwapChain
{
    swapChain = theSwapChain;
    auto &swapChainInfo = swapChain->swapChainInfo;

    if(![self initWithFrame: NSMakeRect(0, 0, swapChainInfo.width, swapChainInfo.height)])
        return nil;

    CGSize drawableSize;
    drawableSize.width = swapChainInfo.width;
    drawableSize.height = swapChainInfo.height;
    
    auto metalLayer = [CAMetalLayer new];
    metalLayer.device = theSwapChain->deviceForMetal->device;
    metalLayer.framebufferOnly = YES;
    metalLayer.drawableSize = drawableSize;
    metalLayer.pixelFormat = AgpuMetal::mapTextureFormat(swapChainInfo.colorbuffer_format);
    if(metalLayer.pixelFormat == MTLPixelFormatInvalid)
        metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    [self setLayer: metalLayer];
    [self setWantsLayer: YES];
    swapChain->metalLayer = metalLayer;
    return self;
}

@end

namespace AgpuMetal
{
    
AMtlSwapChain::AMtlSwapChain(const agpu::device_ref &device)
    : device(device)
{
    window = nil;
    view = nil;
    metalLayer = nil;
}

AMtlSwapChain::~AMtlSwapChain()
{
    if(window)
        [window release];
}

agpu::swap_chain_ref AMtlSwapChain::create(const agpu::device_ref &device, const agpu::command_queue_ref &presentQueue, agpu_swap_chain_create_info *createInfo)
{
    if(!createInfo || !createInfo->window)
        return agpu::swap_chain_ref();

    auto result = agpu::makeObject<AMtlSwapChain> (device);
    auto amtlSwapChain = result.as<AMtlSwapChain> ();

    auto window = (NSWindow*)createInfo->window;
    amtlSwapChain->window = window;
    amtlSwapChain->swapChainInfo = *createInfo;

    // Create the view
    amtlSwapChain->view = [[AGPUSwapChainView alloc] initWithSwapChain: amtlSwapChain];
    if(amtlSwapChain->view == nil)
        return agpu::swap_chain_ref();

    window.contentView = amtlSwapChain->view;

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
    memset(&depthStencilViewDesc, 0, sizeof(depthStencilViewDesc));
    auto depthStencilViewPointer = &depthStencilViewDesc;
    if (!hasDepth && !hasStencil)
        depthStencilViewPointer = nullptr;

    // Create the framebuffers
    amtlSwapChain->framebuffers.reserve(createInfo->buffer_count);
    bool failure = false;
    for(agpu_uint i = 0; i < createInfo->buffer_count; ++i)
    {
        // Create the depth stencil buffer
        agpu::texture_ref depthStencilBuffer;
        if(hasDepth || hasStencil)
        {
            depthStencilBuffer = AMtlTexture::create(device, &depthStencilDesc);
            if (!depthStencilBuffer)
            {
                failure = true;
                break;
            }

            // Get the depth stencil buffer view description.
            depthStencilBuffer->getFullViewDescription(&depthStencilViewDesc);
        }

        // Create the framebuffer
        auto framebuffer = AMtlFramebuffer::createForSwapChain(device, createInfo->width, createInfo->height, depthStencilViewPointer);
        if(!framebuffer)
        {
            failure = true;
            break;
        }

        amtlSwapChain->framebuffers.push_back(framebuffer);
        amtlSwapChain->presentCommands.push_back(nil);
    }

    // Set the drawable of the first framebuffer
    if(!failure)
    {
        auto drawable = [amtlSwapChain->metalLayer nextDrawable];
        if(drawable)
        {
            amtlSwapChain->currentFramebufferIndex = 0;
            const auto &fb = amtlSwapChain->framebuffers[amtlSwapChain->currentFramebufferIndex];
            fb.as<AMtlFramebuffer> ()->setDrawable(drawable, drawable.texture);
        }
        else
        {
            failure = true;
        }
    }

    if(failure)
        return agpu::swap_chain_ref();

    amtlSwapChain->presentQueue = presentQueue;
    return result;
}

agpu_error AMtlSwapChain::swapBuffers()
{
    auto currentFramebuffer = framebuffers[currentFramebufferIndex];
    auto &presentCommand = presentCommands[currentFramebufferIndex];
    if(presentCommand)
        [presentCommand release];
    presentCommand = [presentQueue.as<AMtlCommandQueue> ()->handle commandBuffer];
    [presentCommand presentDrawable: currentFramebuffer.as<AMtlFramebuffer> ()->drawable];
    [presentCommand commit];

    // Release the old drawable
    currentFramebufferIndex = (currentFramebufferIndex + 1) % framebuffers.size();
    framebuffers[currentFramebufferIndex].as<AMtlFramebuffer> ()->releaseDrawable();

    auto drawable = [metalLayer nextDrawable];
    framebuffers[currentFramebufferIndex].as<AMtlFramebuffer> ()->setDrawable(drawable, drawable.texture);
    if(!drawable)
        return AGPU_ERROR;

    return AGPU_OK;
}

agpu::framebuffer_ptr AMtlSwapChain::getCurrentBackBuffer()
{
    return framebuffers[currentFramebufferIndex].disownedNewRef();
}

agpu_size AMtlSwapChain::getCurrentBackBufferIndex ( )
{
    return currentFramebufferIndex;
}

agpu_size AMtlSwapChain::getFramebufferCount ( )
{
    return framebuffers.size();
}

} // End of namespace AgpuMetal
