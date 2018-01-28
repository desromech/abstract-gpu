#include <string.h>
#include "framebuffer.hpp"
#include "command_queue.hpp"
#include "swap_chain.hpp"
#include "texture.hpp"
#include "texture_formats.hpp"

_agpu_swap_chain::_agpu_swap_chain()
    : doublebuffer(false), backBufferIndex(0)
{
    framebuffers[0] = framebuffers[1] = nullptr;
}

void _agpu_swap_chain::lostReferences()
{
    for(int i = 0; i < 2; ++i)
    {
        auto framebuffer = framebuffers[i];
        if(framebuffer)
            framebuffer->release();
    }
    commandQueue->release();
}

agpu_swap_chain *_agpu_swap_chain::create(agpu_device *device, agpu_command_queue* commandQueue, agpu_swap_chain_create_info *create_info)
{
    // Create the framebuffer objects.
    agpu_framebuffer *framebuffers[2] = {nullptr, nullptr};

    int fbCount = create_info->buffer_count > 1 ? 2 : 1;
    bool failure = false;
    bool hasDepth = hasDepthComponent(create_info->depth_stencil_format);
    bool hasStencil = hasStencilComponent(create_info->depth_stencil_format);
    agpu_texture_view_description colorViewDesc;
    agpu_texture_view_description depthStencilViewDesc;
    auto depthStencilViewDescPointer = &depthStencilViewDesc;
    if (!hasDepth && !hasStencil)
        depthStencilViewDescPointer = nullptr;

    for(int i = 0; i < fbCount; ++i)
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
        for(int i = 0; i < fbCount; ++i)
        {
            if(framebuffers[i])
                framebuffers[i]->release();
        }

        return nullptr;
    }

    auto chain = new agpu_swap_chain();
    chain->device = device;
    chain->window = create_info->window;
    chain->doublebuffer = create_info->buffer_count > 1;
    chain->width = create_info->width;
    chain->height = create_info->height;
    chain->commandQueue = commandQueue;
    commandQueue->retain();

    // Set the window pixel format.
    device->setWindowPixelFormat(chain->window);

    // Store the framebuffers.
    for(int i = 0; i < fbCount; ++i)
        chain->framebuffers[i] = framebuffers[i];

    return chain;
}

agpu_error _agpu_swap_chain::swapBuffers()
{
    auto presentBuffer = getCurrentBackBuffer();
    device->onMainContextBlocking([this, presentBuffer](){
        auto currentContext = OpenGLContext::getCurrent();
        auto res = currentContext->makeCurrentWithWindow(window);
        if(!res)
            return;

        // Blit the framebuffer.
        auto backBuffer = getCurrentBackBuffer();
        //printf("%p\n", backBuffer);
        backBuffer->bind(GL_READ_FRAMEBUFFER);
        device->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        device->glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // Swap the window buffers.
        currentContext->swapBuffersOfWindow(window);
    });

    if(doublebuffer)
        backBufferIndex = (backBufferIndex + 1) % 2;
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
    return 2;
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
