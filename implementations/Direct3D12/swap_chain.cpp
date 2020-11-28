#include "swap_chain.hpp"
#include "framebuffer.hpp"
#include "texture.hpp"
#include "texture_formats.hpp"
#include "command_queue.hpp"

namespace AgpuD3D12
{

ADXSwapChain::ADXSwapChain(const agpu::device_ref &cdevice)
    : device(cdevice), window(NULL), frameIndex(0), frameCount(0), windowWidth(0), windowHeight(0)
{
}

ADXSwapChain::~ADXSwapChain()
{
}

agpu::swap_chain_ref ADXSwapChain::create(const agpu::device_ref &device, const agpu::command_queue_ref &queue, agpu_swap_chain_create_info *createInfo)
{
    auto swapChain = agpu::makeObject<ADXSwapChain> (device);
    auto adxSwapChain = swapChain.as<ADXSwapChain> ();

	if (createInfo->flags & AGPU_SWAP_CHAIN_FLAG_OVERLAY_WINDOW)
		adxSwapChain->overlayWindow = AgpuCommon::createOverlaySwapChainWindow(createInfo);

    adxSwapChain->windowWidth = createInfo->width;
    adxSwapChain->windowHeight = createInfo->height;

    adxSwapChain->window = (HWND)createInfo->window;
	if (adxSwapChain->overlayWindow)
		adxSwapChain->window = (HWND)adxSwapChain->overlayWindow->getWindowHandle();
    adxSwapChain->frameCount = createInfo->buffer_count;

    // Create the swap chain
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = adxSwapChain->frameCount;
    swapChainDesc.BufferDesc.Width = adxSwapChain->windowWidth;
    swapChainDesc.BufferDesc.Height = adxSwapChain->windowHeight;
    swapChainDesc.BufferDesc.Format = (DXGI_FORMAT)createInfo->colorbuffer_format;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.OutputWindow = adxSwapChain->window;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;

    // The swap chain creation is failing with a SRGB format, so use the non-srgb variant and compensate on the texture view.
    if (swapChainDesc.BufferDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    if (swapChainDesc.BufferDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB)
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

    ComPtr<IDXGIFactory4> factory;
    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))))
        return agpu::swap_chain_ref();

    ComPtr<IDXGISwapChain> oldSwapChain;
    if (FAILED(factory->CreateSwapChain(queue.as<ADXCommandQueue> ()->queue.Get(), &swapChainDesc, &oldSwapChain)))
        return agpu::swap_chain_ref();

    if (FAILED(oldSwapChain.As(&adxSwapChain->swapChain)))
        return agpu::swap_chain_ref();

    auto &dxSwapChain = adxSwapChain->swapChain;
    adxSwapChain->frameIndex = dxSwapChain->GetCurrentBackBufferIndex();

    // Color buffer description.
    agpu_texture_description colorDesc = {};
    colorDesc.type = AGPU_TEXTURE_2D;
    colorDesc.width = createInfo->width;
    colorDesc.height = createInfo->height;
    colorDesc.depth = 1;
    colorDesc.layers = 1;
    colorDesc.format = createInfo->colorbuffer_format;
    colorDesc.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
    colorDesc.usage_modes = AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT;
    colorDesc.main_usage_mode = AGPU_TEXTURE_USAGE_PRESENT;
    colorDesc.miplevels = 1;
    colorDesc.sample_count = 1;

    // Depth stencil buffer descriptions.
    agpu_texture_description depthStencilDesc = {};
    depthStencilDesc.type = AGPU_TEXTURE_2D;
    depthStencilDesc.width = createInfo->width;
    depthStencilDesc.height = createInfo->height;
    depthStencilDesc.depth = 1;
    depthStencilDesc.layers = 1;
    depthStencilDesc.format = createInfo->depth_stencil_format;
    depthStencilDesc.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
    depthStencilDesc.miplevels = 1;
    depthStencilDesc.sample_count = 1;
    bool hasDepth = hasDepthComponent(createInfo->depth_stencil_format);
    bool hasStencil = hasStencilComponent(createInfo->depth_stencil_format);
    if (hasDepth)
        depthStencilDesc.usage_modes = agpu_texture_usage_mode_mask(depthStencilDesc.usage_modes | AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT);
    if (hasStencil)
        depthStencilDesc.usage_modes = agpu_texture_usage_mode_mask(depthStencilDesc.usage_modes | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT);
    depthStencilDesc.main_usage_mode = depthStencilDesc.usage_modes;

    // Create the shared depth stencil buffer.
    agpu::texture_ref depthStencilBuffer;
    agpu::texture_view_ref depthStencilView;
    if (hasDepth || hasStencil)
    {
        depthStencilBuffer = ADXTexture::create(device, &depthStencilDesc);
        if (!depthStencilBuffer)
            return agpu::swap_chain_ref();

        // Get the depth stencil buffer view description.
        depthStencilView = agpu::texture_view_ref(depthStencilBuffer->getOrCreateFullView());
    }

    // Create the main frame buffers.
    adxSwapChain->framebuffers.reserve(adxSwapChain->frameCount);
    for (int i = 0; i < adxSwapChain->frameCount; ++i)
    {
        ComPtr<ID3D12Resource> colorBufferResource;
        if (FAILED(dxSwapChain->GetBuffer(i, IID_PPV_ARGS(&colorBufferResource))))
            return agpu::swap_chain_ref();

        // Create the color buffer.
        auto colorBuffer = ADXTexture::createFromResource(device, &colorDesc, colorBufferResource);
        if (!colorBuffer)
            return agpu::swap_chain_ref();

        // Get the color buffer view description.
        auto colorBufferView = agpu::texture_view_ref(colorBuffer->getOrCreateFullView());

        auto framebuffer = ADXFramebuffer::create(device, adxSwapChain->windowWidth, adxSwapChain->windowHeight, 1, &colorBufferView, depthStencilView);
        framebuffer.as<ADXFramebuffer> ()->swapChainBuffer = true;
        adxSwapChain->framebuffers.push_back(framebuffer);
    }

    return swapChain;
}

agpu_error ADXSwapChain::swapBuffers()
{
    ERROR_IF_FAILED(swapChain->Present(1, 0));
    frameIndex = (frameIndex + 1) % frameCount;
    return AGPU_OK;
}

agpu::framebuffer_ptr ADXSwapChain::getCurrentBackBuffer()
{
	return framebuffers[frameIndex].disownedNewRef();
}

agpu_size ADXSwapChain::getCurrentBackBufferIndex()
{
    return frameIndex;
}

agpu_size ADXSwapChain::getFramebufferCount()
{
    return framebuffers.size();
}

agpu_error ADXSwapChain::setOverlayPosition(agpu_int x, agpu_int y)
{
	if (!overlayWindow)
		return AGPU_OK;

	return overlayWindow->setPosition(x, y);
}

} // End of namespace AgpuD3D12
