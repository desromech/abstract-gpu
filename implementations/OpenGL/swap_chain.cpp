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
    bool hasDepth = hasDepthComponent(create_info->depth_stencil_format);
    bool hasStencil = hasStencilComponent(create_info->depth_stencil_format);

    agpu_texture_description colorAttachmentDescription = {};
    colorAttachmentDescription.type = AGPU_TEXTURE_2D;
    colorAttachmentDescription.width = create_info->width;
    colorAttachmentDescription.height = create_info->height;
    colorAttachmentDescription.depth = 1;
    colorAttachmentDescription.layers = 1;
    colorAttachmentDescription.format = create_info->colorbuffer_format;
    colorAttachmentDescription.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
    colorAttachmentDescription.usage_modes = AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT;
    colorAttachmentDescription.main_usage_mode = colorAttachmentDescription.usage_modes;
    colorAttachmentDescription.miplevels = 1;

    // Create the depth buffer.
    agpu::texture_ref depthStencilBuffer;
    agpu::texture_view_ref depthStencilView;
    if (hasDepth || hasStencil)
    {
        agpu_texture_description depthStencilAttachmentDescription = {};
        depthStencilAttachmentDescription.type = AGPU_TEXTURE_2D;
        depthStencilAttachmentDescription.width = create_info->width;
        depthStencilAttachmentDescription.height = create_info->height;
        depthStencilAttachmentDescription.depth = 1;
        depthStencilAttachmentDescription.layers = 1;
        depthStencilAttachmentDescription.format = create_info->depth_stencil_format;
        depthStencilAttachmentDescription.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
        depthStencilAttachmentDescription.usage_modes = agpu_texture_usage_mode_mask(AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT);
        if (hasStencil)
            depthStencilAttachmentDescription.usage_modes = agpu_texture_usage_mode_mask(depthStencilAttachmentDescription.usage_modes | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT);
        depthStencilAttachmentDescription.main_usage_mode = depthStencilAttachmentDescription.usage_modes;
        depthStencilAttachmentDescription.miplevels = 1;

        depthStencilBuffer = GLTexture::create(device, &depthStencilAttachmentDescription);
        if (!depthStencilBuffer)
        {
            printError("Failed to create swap chain depth stencil buffer buffer.\n");
            return agpu::swap_chain_ref();
        }

        depthStencilView = agpu::texture_view_ref(depthStencilBuffer->getOrCreateFullView());
    }

    for(size_t i = 0; i < framebuffers.size(); ++i)
    {
        agpu::texture_ref colorBuffer;
        agpu::texture_view_ref colorBufferView;

        // Create the color buffer.
        {
            colorBuffer = GLTexture::create(device, &colorAttachmentDescription);
            if (!colorBuffer)
            {
                printError("Failed to create swap chain color buffer.\n");
                return agpu::swap_chain_ref();
            }

            colorBufferView = agpu::texture_view_ref(colorBuffer->getOrCreateFullView());
        }

        auto fb = GLFramebuffer::create(device, create_info->width, create_info->height, 1, &colorBufferView, depthStencilView);
        if(!fb)
        {
            printError("Failed to create swap chain framebuffer.\n");
            return agpu::swap_chain_ref();
        }

        // Store the framebuffer.
        framebuffers[i] = fb;
    }

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
