#include "swap_chain.hpp"
#include "framebuffer.hpp"
#include "command_queue.hpp"
#include "texture_format.hpp"
#include "texture.hpp"
#include "../Common/memory_profiler.hpp"

@implementation AGPUSwapChainView
-(AGPUSwapChainView*) initWithSwapChain: (AgpuMetal::AMtlSwapChain*)theSwapChain
{
    auto &swapChainInfo = theSwapChain->swapChainInfo;

    if(![self initWithFrame: NSMakeRect(swapChainInfo.x, swapChainInfo.y, swapChainInfo.width, swapChainInfo.height)])
        return nil;

    [self createLayerFor: theSwapChain];
    
    return self;
}

-(void) createLayerFor: (AgpuMetal::AMtlSwapChain*)theSwapChain
{
    swapChain = theSwapChain;
    auto &swapChainInfo = swapChain->swapChainInfo;

    auto metalLayer = [CAMetalLayer layer];
    metalLayer.device = theSwapChain->deviceForMetal->device;
    metalLayer.framebufferOnly = YES;
    metalLayer.pixelFormat = AgpuMetal::mapTextureFormat(swapChainInfo.colorbuffer_format);
    if(metalLayer.pixelFormat == MTLPixelFormatInvalid)
        metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    [self setLayer: metalLayer];
    [self setWantsLayer: YES];
    swapChain->metalLayer = metalLayer;

    CGFloat backingScaleFactor = 1.0;
    if((swapChainInfo.flags & AGPU_SWAP_CHAIN_FLAG_APPLY_SCALE_FACTOR_FOR_HI_DPI) != 0)
    {
        backingScaleFactor = [[NSScreen mainScreen] backingScaleFactor];
    }
    metalLayer.contentsScale = backingScaleFactor;

    CGSize boundsSize = {};
    boundsSize.width = swapChainInfo.width;
    boundsSize.height = swapChainInfo.height;

    CGRect bounds = {};
    bounds.size = boundsSize;
    metalLayer.bounds = bounds;

    CGSize drawableSize = {};
    drawableSize.width = swapChainInfo.width*backingScaleFactor;
    drawableSize.height = swapChainInfo.height*backingScaleFactor;
    metalLayer.drawableSize = drawableSize;
}

- (void) swapChainRecreatedInto: (AgpuMetal::AMtlSwapChain*)theNewSwapChain
{
    swapChain->metalLayer = nil;

    auto &swapChainInfo = theNewSwapChain->swapChainInfo;
    self.frame = NSMakeRect(swapChainInfo.x, swapChainInfo.y, swapChainInfo.width, swapChainInfo.height);
    [self createLayerFor: theNewSwapChain];
}

@end

namespace AgpuMetal
{
    
AMtlSwapChain::AMtlSwapChain(const agpu::device_ref &device)
    : device(device)
{
    AgpuProfileConstructor(AMtlSwapChain);
}

AMtlSwapChain::~AMtlSwapChain()
{
    AgpuProfileDestructor(AMtlSwapChain);
    if(view)
        [view removeFromSuperview];
}

agpu::swap_chain_ref AMtlSwapChain::create(const agpu::device_ref &device, const agpu::command_queue_ref &presentQueue, agpu_swap_chain_create_info *createInfo)
{
    if(!createInfo || !createInfo->window)
        return agpu::swap_chain_ref();

    @autoreleasepool {
        auto result = agpu::makeObject<AMtlSwapChain> (device);
        auto amtlSwapChain = result.as<AMtlSwapChain> ();
        amtlSwapChain->swapChainInfo = *createInfo;
        
        // If there is an old swap chain, we should just borrow its view and resize its drawable.
        AMtlSwapChain *oldMtlSwapChain = nullptr;
        if(createInfo->old_swap_chain)
        {
            auto oldSwapChainRef = agpu::swap_chain_ref::import(createInfo->old_swap_chain);
            oldMtlSwapChain = oldSwapChainRef.as<AMtlSwapChain> ();
            amtlSwapChain->window = oldMtlSwapChain->window;
            amtlSwapChain->view = oldMtlSwapChain->view;

            oldMtlSwapChain->window = nil;
            oldMtlSwapChain->view = nil;
            [amtlSwapChain->view swapChainRecreatedInto: amtlSwapChain];
        }
        else
        {
            auto window = (__bridge NSWindow*)createInfo->window;
            amtlSwapChain->window = window;

            // Create the view
            amtlSwapChain->view = [[AGPUSwapChainView alloc] initWithSwapChain: amtlSwapChain];
            if(amtlSwapChain->view == nil)
                return agpu::swap_chain_ref();

            if(createInfo->flags & AGPU_SWAP_CHAIN_FLAG_OVERLAY_WINDOW)
            {
                [window.contentView addSubview: amtlSwapChain->view];
            }
            else
            {
                window.contentView = amtlSwapChain->view;        
            }        
        }

        auto drawable = [amtlSwapChain->metalLayer nextDrawable];
        if(!drawable)
            return agpu::swap_chain_ref();

        // Fetch the actual drawable size.
        CGSize drawableSize = amtlSwapChain->metalLayer.drawableSize;
        agpu_uint drawableWidth = drawableSize.width;
        agpu_uint drawableHeight = drawableSize.height;
        amtlSwapChain->swapChainInfo.width = drawableWidth;
        amtlSwapChain->swapChainInfo.height = drawableHeight;

        // Create the depth stencil buffer
        agpu::texture_ref depthStencilBuffer;
        agpu::texture_view_ref depthStencilBufferView;
        bool hasDepth = hasDepthComponent(createInfo->depth_stencil_format);
        bool hasStencil = hasStencilComponent(createInfo->depth_stencil_format);
        if(hasDepth || hasStencil)
        {
            // Depth stencil buffer descriptions.
            agpu_texture_description depthStencilDesc = {};
            depthStencilDesc.type = AGPU_TEXTURE_2D;
            depthStencilDesc.width = drawableWidth;
            depthStencilDesc.height = drawableHeight;
            depthStencilDesc.depth = 1;
            depthStencilDesc.layers = 1;
            depthStencilDesc.format = createInfo->depth_stencil_format;
            depthStencilDesc.miplevels = 1;
            depthStencilDesc.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
            if (hasDepth)
                depthStencilDesc.usage_modes = AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT;
            if (hasStencil)
                depthStencilDesc.usage_modes = agpu_texture_usage_mode_mask(depthStencilDesc.usage_modes | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT);
            depthStencilDesc.main_usage_mode = depthStencilDesc.usage_modes;

            depthStencilBuffer = AMtlTexture::create(device, &depthStencilDesc);
            if (!depthStencilBuffer)
                return agpu::swap_chain_ref();

            // Get the depth stencil buffer view description.
            depthStencilBufferView = agpu::texture_view_ref(depthStencilBuffer->getOrCreateFullView());
        }

        // Create the framebuffers
        amtlSwapChain->framebuffers.reserve(createInfo->buffer_count);
        for(agpu_uint i = 0; i < createInfo->buffer_count; ++i)
        {
            // Create the framebuffer
            auto framebuffer = AMtlFramebuffer::createForSwapChain(device, drawableWidth, drawableHeight, depthStencilBufferView);
            if(!framebuffer)
                return agpu::swap_chain_ref();

            amtlSwapChain->framebuffers.push_back(framebuffer);
            amtlSwapChain->presentCommands.push_back(nil);
        }

        // Set the drawable of the first framebuffer
        if(oldMtlSwapChain)
        {
            oldMtlSwapChain->framebuffers[oldMtlSwapChain->currentFramebufferIndex]
                .as<AMtlFramebuffer> ()->releaseDrawable();
        }
                
        amtlSwapChain->currentFramebufferIndex = 0;
        const auto &fb = amtlSwapChain->framebuffers[amtlSwapChain->currentFramebufferIndex];
        fb.as<AMtlFramebuffer> ()->setDrawable(drawable, drawable.texture);

        amtlSwapChain->presentQueue = presentQueue;
        return result;
    }
}

agpu_error AMtlSwapChain::swapBuffers()
{
    if(!view)
        return AGPU_INVALID_OPERATION;

    @autoreleasepool {
        auto currentFramebuffer = framebuffers[currentFramebufferIndex];
        auto &presentCommand = presentCommands[currentFramebufferIndex];
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
}


agpu::framebuffer_ptr AMtlSwapChain::getCurrentBackBuffer()
{
    return getCurrentBackBufferForLayer(0);
}

agpu::framebuffer_ptr AMtlSwapChain::getCurrentBackBufferForLayer(agpu_uint layer)
{
    if(layer != 0)
        return nullptr;
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

agpu_uint AMtlSwapChain::getWidth()
{
    return swapChainInfo.width;
}

agpu_uint AMtlSwapChain::getHeight()
{
    return swapChainInfo.height;
}

agpu_uint AMtlSwapChain::getLayerCount()
{
    return 1;
}

agpu_error AMtlSwapChain::setOverlayPosition(agpu_int x, agpu_int y)
{
    if(swapChainInfo.flags & AGPU_SWAP_CHAIN_FLAG_OVERLAY_WINDOW)
    {
        if(view)
            [view setFrameOrigin: (NSPoint){(CGFloat)x, (CGFloat)y}];
    }
    return AGPU_OK;
}

} // End of namespace AgpuMetal
