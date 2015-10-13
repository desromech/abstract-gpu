#include <string.h>
#include "framebuffer.hpp"
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
}

agpu_swap_chain *_agpu_swap_chain::create(agpu_device *device, agpu_swap_chain_create_info *create_info)
{
    // Create the framebuffer objects.
    agpu_framebuffer *framebuffers[2] = {nullptr, nullptr};

    int fbCount = create_info->doublebuffer ? 2 : 1;
    bool failure = false;
    bool hasDepth = hasDepthComponent(create_info->depth_stencil_format);
    bool hasStencil = hasStencilComponent(create_info->depth_stencil_format);
    for(int i = 0; i < fbCount; ++i)
    {
        auto fb = agpu_framebuffer::create(device, create_info->width, create_info->height, 1, hasDepth, hasStencil);
        if(!fb)
        {
            failure = true;
            break;
        }

        // Store the framebuffer.
        framebuffers[i] = fb;

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
            auto colorBuffer = agpu_texture::create(device, &desc, nullptr);
            if(!colorBuffer)
            {
                failure = true;
                break;
            }

            // Attach the color buffer.
            fb->attachColorBuffer(0, colorBuffer);
        }

        // Create the depth buffer.
        {
            agpu_texture_description desc;
            memset(&desc, 0, sizeof(desc));
            desc.type = AGPU_TEXTURE_2D;
            desc.width = create_info->width;
            desc.height = create_info->height;
            desc.depthOrArraySize = 1;
            desc.format = create_info->depth_stencil_format;
            desc.flags = agpu_texture_flags(AGPU_TEXTURE_FLAG_DEPTH_STENCIL | AGPU_TEXTURE_FLAG_RENDERBUFFER_ONLY);
            desc.miplevels = 1;
            auto depthStencilBuffer = agpu_texture::create(device, &desc, nullptr);
            if(!depthStencilBuffer)
            {
                failure = true;
                break;
            }

            // Attach the color buffer.
            fb->attachDepthStencilBuffer(depthStencilBuffer);
        }
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
    chain->doublebuffer = create_info->doublebuffer;
    chain->width = create_info->width;
    chain->height = create_info->height;

    // Store the framebuffers.
    for(int i = 0; i < fbCount; ++i)
        chain->framebuffers[i] = framebuffers[i];

    return chain;
}

agpu_error _agpu_swap_chain::swapBuffers()
{
    agpu_error error = AGPU_OK;
    device->onMainContextBlocking([&](){
        // Use the window.
        auto res = device->mainContext->makeCurrentWithWindow(window);
        if(!res)
        {
            error = AGPU_ERROR;
            return;
        }

        // Blit the framebuffer.
        auto backBuffer = getCurrentBackBuffer();
        //printf("%p\n", backBuffer);
        backBuffer->bind(GL_READ_FRAMEBUFFER);
        device->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        device->glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        device->mainContext->finish();

        // Swap the window buffers.
        device->mainContext->swapBuffersOfWindow(window);

        // Restore the main window.
        device->mainContext->makeCurrent();
    });

    if(error != AGPU_OK)
        return error;

    if(doublebuffer)
        backBufferIndex = (backBufferIndex + 1) % 2;
    return error;
}

agpu_framebuffer* _agpu_swap_chain::getCurrentBackBuffer ()
{
    return framebuffers[backBufferIndex];
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
