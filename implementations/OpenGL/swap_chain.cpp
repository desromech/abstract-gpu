#include <string.h>
#include "framebuffer.hpp"
#include "command_queue.hpp"
#include "swap_chain.hpp"
#include "texture.hpp"
#include "texture_formats.hpp"

namespace AgpuGL
{

GLSwapChain::GLSwapChain()
    : backBufferIndex(0)
{
}

GLSwapChain::~GLSwapChain()
{
}

agpu::swap_chain_ref GLSwapChain::create(const agpu::device_ref &device, const agpu::command_queue_ref &commandQueue, agpu_swap_chain_create_info *create_info)
{
    // Create the framebuffer objects.
    std::vector<agpu::framebuffer_ref> framebuffers(create_info->buffer_count);
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
        agpu::texture_ref colorBuffer;
        agpu::texture_ref depthStencilBuffer;

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
            colorBuffer = GLTexture::create(device, &desc);
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
            depthStencilBuffer = GLTexture::create(device, &desc);
            if (!depthStencilBuffer)
            {
                printError("Failed to create swap chain depth stencil buffer buffer.\n");
                failure = true;
                break;
            }

            depthStencilBuffer->getFullViewDescription(&depthStencilViewDesc);
        }

        auto fb = GLFramebuffer::create(device, create_info->width, create_info->height, 1, &colorViewDesc, depthStencilViewDescPointer);
        if(!fb)
        {
            printError("Failed to create swap chain framebuffer.\n");
            failure = true;
            break;
        }

        // Store the framebuffer.
        framebuffers[i] = fb;
    }

    // Check the failure
    if(failure)
        return agpu::swap_chain_ref();

    auto result = agpu::makeObject<GLSwapChain> ();
    auto chain = result.as<GLSwapChain> ();
    chain->device = device;
    chain->window = create_info->window;
    chain->width = create_info->width;
    chain->height = create_info->height;
    chain->commandQueue = commandQueue;

    // Set the window pixel format.
    deviceForGL->setWindowPixelFormat(chain->window);

    // Store the framebuffers.
	chain->framebuffers = framebuffers;
    return result;
}

agpu_error GLSwapChain::swapBuffers()
{
    auto glDevice = device.as<GLDevice> ();
    glDevice->onMainContextBlocking([&](){
        auto currentContext = OpenGLContext::getCurrent();
        auto res = currentContext->makeCurrentWithWindow(window);
        if(!res)
            return;

        // Blit the framebuffer.
        auto backBuffer = framebuffers[backBufferIndex].as<GLFramebuffer> ();
        backBuffer->bind(GL_READ_FRAMEBUFFER);
        glDevice->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDevice->glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // Swap the window buffers.
        currentContext->swapBuffersOfWindow(window);
    });

    backBufferIndex = (backBufferIndex + 1) % framebuffers.size();
    return AGPU_OK;
}

agpu::framebuffer_ptr GLSwapChain::getCurrentBackBuffer ()
{
    return framebuffers[backBufferIndex].disownedNewRef();
}

agpu_size GLSwapChain::getCurrentBackBufferIndex ()
{
    return backBufferIndex;
}

agpu_size GLSwapChain::getFramebufferCount ()
{
    return (agpu_size)framebuffers.size();
}

} // End of namespace AgpuGL
