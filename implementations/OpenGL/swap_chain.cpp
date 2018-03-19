#include <string.h>
#include "framebuffer.hpp"
#include "command_queue.hpp"
#include "swap_chain.hpp"
#include "texture.hpp"
#include "texture_formats.hpp"

_agpu_swap_chain::_agpu_swap_chain()
    : backBufferIndex(0)
{
}

void _agpu_swap_chain::lostReferences()
{
    for(auto fb : framebuffers)
    {
        if(fb)
			fb->release();
    }
    commandQueue->release();
}

agpu_swap_chain *_agpu_swap_chain::create(agpu_device *device, agpu_command_queue* commandQueue, agpu_swap_chain_create_info *create_info)
{
    // Create the framebuffer objects.
    std::vector<agpu_framebuffer*> framebuffers(create_info->buffer_count);
    bool failure = false;
    bool hasDepth = hasDepthComponent(create_info->depth_stencil_format);
    bool hasStencil = hasStencilComponent(create_info->depth_stencil_format);
    agpu_texture_view_description colorViewDesc;
    agpu_texture_view_description depthStencilViewDesc;
    auto depthStencilViewDescPointer = &depthStencilViewDesc;
    if (!hasDepth && !hasStencil)
        depthStencilViewDescPointer = nullptr;

    for(size_t i = 0; i < framebuffers.size(); ++i)
    {
        agpu_texture *colorBuffer = nullptr;
        agpu_texture *depthStencilBuffer = nullptr;

        // Create the color buffer.
        {
            agpu_texture_description desc;
            memset(&desc, 0, sizeof(desc));
            desc.type = AGPU_TEXTURE_2D;
            desc.width = create_info->width;
            desc.height = create_info->height;
            desc.depthOrArraySize = 1;
            desc.format = create_info->colorbuffer_format;
            desc.flags = agpu_texture_flags(AGPU_TEXTURE_FLAG_RENDER_TARGET | AGPU_TEXTURE_FLAG_RENDERBUFFER_ONLY);
            desc.miplevels = 1;
            colorBuffer = agpu_texture::create(device, &desc);
            if (!colorBuffer)
            {
                printError("Failed to create swap chain color buffer.\n");
                failure = true;
                break;
            }

            colorBuffer->getFullViewDescription(&colorViewDesc);
        }

        // Create the depth buffer.
        if (hasDepth || hasStencil)
        {
            agpu_texture_description desc;
            memset(&desc, 0, sizeof(desc));
            desc.type = AGPU_TEXTURE_2D;
            desc.width = create_info->width;
            desc.height = create_info->height;
            desc.depthOrArraySize = 1;
            desc.format = create_info->depth_stencil_format;
            desc.flags = agpu_texture_flags(AGPU_TEXTURE_FLAG_DEPTH | AGPU_TEXTURE_FLAG_RENDERBUFFER_ONLY);
            if (hasStencil)
                desc.flags = agpu_texture_flags(desc.flags | AGPU_TEXTURE_FLAG_STENCIL);
            desc.miplevels = 1;
            depthStencilBuffer = agpu_texture::create(device, &desc);
            if (!depthStencilBuffer)
            {
                printError("Failed to create swap chain depth stencil buffer buffer.\n");
                if (colorBuffer)
                    colorBuffer->release();
                failure = true;
                break;
            }

            depthStencilBuffer->getFullViewDescription(&depthStencilViewDesc);
        }
        auto fb = agpu_framebuffer::create(device, create_info->width, create_info->height, 1, &colorViewDesc, depthStencilViewDescPointer);
        if(!fb)
        {
            printError("Failed to create swap chain framebuffer.\n");
            if (colorBuffer)
                colorBuffer->release();
            if (depthStencilBuffer)
                depthStencilBuffer->release();
            failure = true;
            break;
        }

        // Store the framebuffer.
        framebuffers[i] = fb;

        if (colorBuffer)
            colorBuffer->release();
        if (depthStencilBuffer)
            depthStencilBuffer->release();

    }

    // Check the failure
    if(failure)
    {
        for(auto fb : framebuffers)
        {
            if(fb)
                fb->release();
        }

        return nullptr;
    }

    auto chain = new agpu_swap_chain();
    chain->device = device;
    chain->window = create_info->window;
    chain->width = create_info->width;
    chain->height = create_info->height;
    chain->commandQueue = commandQueue;
    commandQueue->retain();

    // Set the window pixel format.
    device->setWindowPixelFormat(chain->window);

    // Store the framebuffers.
	chain->framebuffers = framebuffers;
    return chain;
}

agpu_error _agpu_swap_chain::swapBuffers()
{
    device->onMainContextBlocking([this](){
        auto currentContext = OpenGLContext::getCurrent();
        auto res = currentContext->makeCurrentWithWindow(window);
        if(!res)
            return;

        // Blit the framebuffer.
        auto backBuffer = getCurrentBackBuffer();
        backBuffer->bind(GL_READ_FRAMEBUFFER);
        device->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		device->glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // Swap the window buffers.
        currentContext->swapBuffersOfWindow(window);
    });

    backBufferIndex = (backBufferIndex + 1) % framebuffers.size();
    return AGPU_OK;
}

agpu_framebuffer* _agpu_swap_chain::getCurrentBackBuffer ()
{
    auto fb = framebuffers[backBufferIndex];
    fb->retain();
    return fb;
}

agpu_size _agpu_swap_chain::getCurrentBackBufferIndex ()
{
    return backBufferIndex;
}

agpu_size _agpu_swap_chain::getFramebufferCount ()
{
    return framebuffers.size();
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

AGPU_EXPORT agpu_framebuffer* agpuGetCurrentBackBuffer ( agpu_swap_chain* swap_chain )
{
    if(!swap_chain)
        return nullptr;
    return swap_chain->getCurrentBackBuffer();
}

AGPU_EXPORT agpu_error agpuSwapBuffers ( agpu_swap_chain* swap_chain )
{
    CHECK_POINTER(swap_chain);
    return swap_chain->swapBuffers();
}


agpu_size agpuGetCurrentBackBufferIndex ( agpu_swap_chain* swap_chain )
{
    if(!swap_chain)
        return 0;
    return swap_chain->getCurrentBackBufferIndex();
}

agpu_size agpuGetFramebufferCount ( agpu_swap_chain* swap_chain )
{
    if(!swap_chain)
        return 0;
    return swap_chain->getFramebufferCount();
}
