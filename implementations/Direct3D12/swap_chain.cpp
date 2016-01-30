#include "swap_chain.hpp"
#include "framebuffer.hpp"
#include "texture.hpp"
#include "texture_formats.hpp"
#include "command_queue.hpp"

_agpu_swap_chain::_agpu_swap_chain()
{
    for (int i = 0; i < MaxFrameCount; ++i)
        framebuffer[i] = nullptr;

}

void _agpu_swap_chain::lostReferences()
{
    for (int i = 0; i < MaxFrameCount; ++i)
    {
        auto fb = framebuffer[i];
        if (fb)
            fb->release();
    }
}

_agpu_swap_chain *_agpu_swap_chain::create(agpu_device *device, agpu_command_queue *queue, agpu_swap_chain_create_info *createInfo)
{
    std::unique_ptr<agpu_swap_chain> swapChain(new agpu_swap_chain());

    swapChain->windowWidth = createInfo->width;
    swapChain->windowHeight = createInfo->height;

    swapChain->window = (HWND)createInfo->window;
    swapChain->frameCount = createInfo->doublebuffer ? 2 : 1;

    // Create the swap chain
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = swapChain->frameCount;
    swapChainDesc.BufferDesc.Width = swapChain->windowWidth;
    swapChainDesc.BufferDesc.Height = swapChain->windowHeight;
    swapChainDesc.BufferDesc.Format = (DXGI_FORMAT)createInfo->colorbuffer_format; // TODO: Pick a proper format
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.OutputWindow = swapChain->window;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;

    ComPtr<IDXGIFactory4> factory;
    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))))
        return nullptr;

    ComPtr<IDXGISwapChain> oldSwapChain;
    if (FAILED(factory->CreateSwapChain(queue->queue.Get(), &swapChainDesc, &oldSwapChain)))
        return nullptr;

    if (FAILED(oldSwapChain.As(&swapChain->swapChain)))
        return nullptr;

    auto &dxSwapChain = swapChain->swapChain;
    swapChain->frameIndex = dxSwapChain->GetCurrentBackBufferIndex();

    bool hasDepth = hasDepthComponent(createInfo->depth_stencil_format);
    bool hasStencil = hasStencilComponent(createInfo->depth_stencil_format);

    // Create the main frame buffer.
    {
        bool hasDepthStencil = createInfo->depth_stencil_format != AGPU_TEXTURE_FORMAT_UNKNOWN;
        bool failure = false;
        for (int i = 0; i < swapChain->frameCount; ++i)
        {
            auto framebuffer = agpu_framebuffer::create(device, swapChain->windowWidth, swapChain->windowHeight, 1, hasDepth, hasStencil);
            framebuffer->swapChainBuffer = true;
            swapChain->framebuffer[i] = framebuffer;

            ComPtr<ID3D12Resource> colorBufferResource;
            if (FAILED(dxSwapChain->GetBuffer(i, IID_PPV_ARGS(&colorBufferResource))))
                return false;

            // Create the color buffer.
            {
                agpu_texture_description desc;
                memset(&desc, 0, sizeof(desc));
                desc.type = AGPU_TEXTURE_2D;
                desc.width = createInfo->width;
                desc.height = createInfo->height;
                desc.depthOrArraySize = 1;
                desc.format = createInfo->colorbuffer_format;
                desc.flags = agpu_texture_flags(AGPU_TEXTURE_FLAG_RENDER_TARGET | AGPU_TEXTURE_FLAG_RENDERBUFFER_ONLY);
                desc.miplevels = 1;
                desc.sample_count = 1;
                auto colorBuffer = agpu_texture::createFromResource(device, &desc, colorBufferResource);
                if (!colorBuffer)
                {
                    failure = true;
                    break;
                }

                // Attach the color buffer.
                framebuffer->attachColorBuffer(0, colorBuffer);
                colorBuffer->release();
            }

            // Create the depth buffer.
            if (hasDepth || hasStencil)
            {
                agpu_texture_description desc;
                memset(&desc, 0, sizeof(desc));
                desc.type = AGPU_TEXTURE_2D;
                desc.width = createInfo->width;
                desc.height = createInfo->height;
                desc.depthOrArraySize = 1;
                desc.format = createInfo->depth_stencil_format;
                desc.flags = agpu_texture_flags(AGPU_TEXTURE_FLAG_DEPTH_STENCIL | AGPU_TEXTURE_FLAG_RENDERBUFFER_ONLY);
                desc.miplevels = 1;
                desc.sample_count = 1;
                auto depthStencilBuffer = agpu_texture::create(device, &desc);
                if (!depthStencilBuffer)
                {
                    failure = true;
                    break;
                }

                // Attach the depth stencil buffer.
                framebuffer->attachDepthStencilBuffer(depthStencilBuffer);
                depthStencilBuffer->release();
            }
        }

        // Release the already created framebuffers.
        if (failure)
        {
            for (int i = 0; i < swapChain->frameCount; ++i)
            {
                auto fb = swapChain->framebuffer[i];
                if (fb)
                    fb->release();
            }

            return nullptr;
        }
    }


    swapChain->device = device;

    return swapChain.release();
}

agpu_framebuffer* _agpu_swap_chain::getCurrentBackBuffer()
{
    auto fb = framebuffer[frameIndex];
    fb->retain();
    return fb;
}

agpu_error _agpu_swap_chain::swapBuffers()
{
    ERROR_IF_FAILED(swapChain->Present(1, 0));
    frameIndex = (frameIndex + 1) % frameCount;
    return AGPU_OK;
}

// The exported C interface.
AGPU_EXPORT agpu_error agpuAddSwapChainReference(agpu_swap_chain* swap_chain)
{
    CHECK_POINTER(swap_chain);
    return swap_chain->retain();
}

AGPU_EXPORT agpu_error agpuReleaseSwapChain(agpu_swap_chain* swap_chain)
{
    CHECK_POINTER(swap_chain);
    return swap_chain->release();
}

AGPU_EXPORT agpu_error agpuSwapBuffers(agpu_swap_chain* swap_chain)
{
    CHECK_POINTER(swap_chain);
    return swap_chain->swapBuffers();
}

AGPU_EXPORT agpu_framebuffer* agpuGetCurrentBackBuffer(agpu_swap_chain* swap_chain)
{
    if (!swap_chain)
        return nullptr;
    return swap_chain->getCurrentBackBuffer();
}
