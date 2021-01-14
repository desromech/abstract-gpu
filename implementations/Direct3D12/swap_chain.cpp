#include "swap_chain.hpp"
#include "framebuffer.hpp"
#include "texture.hpp"
#include "texture_formats.hpp"
#include "command_queue.hpp"

namespace AgpuD3D12
{

inline DXGI_FORMAT sanitizeSwapChainFormat(DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM;
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8A8_UNORM;
    default: return format;
    }
}
ADXSwapChain::ADXSwapChain(const agpu::device_ref &cdevice)
    : device(cdevice), window(NULL), frameIndex(0), frameCount(0), windowWidth(0), windowHeight(0)
{
}

ADXSwapChain::~ADXSwapChain()
{
}

agpu::swap_chain_ref ADXSwapChain::create(const agpu::device_ref& device, const agpu::command_queue_ref& queue, agpu_swap_chain_create_info* createInfo)
{
    if (createInfo->old_swap_chain)
        return createResizedSwapChain(device, queue, createInfo);

    return createNewSwapChain(device, queue, createInfo);
}

agpu::swap_chain_ref ADXSwapChain::createNewSwapChain(const agpu::device_ref &device, const agpu::command_queue_ref &queue, agpu_swap_chain_create_info *createInfo)
{
    auto swapChain = agpu::makeObject<ADXSwapChain> (device);
    auto adxSwapChain = swapChain.as<ADXSwapChain> ();

	if (createInfo->flags & AGPU_SWAP_CHAIN_FLAG_OVERLAY_WINDOW)
		adxSwapChain->overlayWindow = AgpuCommon::createOverlaySwapChainWindow(createInfo);

    adxSwapChain->windowWidth = createInfo->width;
    adxSwapChain->windowHeight = createInfo->height;

#if WINAPI_PARTITION_DESKTOP
    adxSwapChain->window = (HWND)createInfo->window;
	if (adxSwapChain->overlayWindow)
		adxSwapChain->window = (HWND)adxSwapChain->overlayWindow->getWindowHandle();
#else
    adxSwapChain->window = (IUnknown*)createInfo->window;
#endif
    adxSwapChain->frameCount = createInfo->buffer_count;
    // Create the swap chain
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = (UINT)adxSwapChain->frameCount;
    swapChainDesc.Width = adxSwapChain->windowWidth;
    swapChainDesc.Height = adxSwapChain->windowHeight;
    swapChainDesc.Format = sanitizeSwapChainFormat((DXGI_FORMAT)createInfo->colorbuffer_format);
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    ComPtr<IDXGIFactory4> factory;
    
    if (FAILED(CreateDXGIFactory2(device.as<ADXDevice>()->isDebugEnabled ? DXGI_CREATE_FACTORY_DEBUG : 0, IID_PPV_ARGS(&factory))))
        return agpu::swap_chain_ref();

#if WINAPI_PARTITION_DESKTOP
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {};
    fullscreenDesc.RefreshRate.Numerator = 60;
    fullscreenDesc.RefreshRate.Denominator = 1;
    fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    fullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    fullscreenDesc.Windowed = TRUE;

    ComPtr<IDXGISwapChain1> oldSwapChainInterface;
    if (FAILED(factory->CreateSwapChainForHwnd(queue.as<ADXCommandQueue>()->queue.Get(), adxSwapChain->window, &swapChainDesc, &fullscreenDesc, nullptr, &oldSwapChainInterface)))
        return agpu::swap_chain_ref();
#else
    swapChainDesc.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;
    ComPtr<IDXGISwapChain1> oldSwapChainInterface;
    if (FAILED(factory->CreateSwapChainForCoreWindow(queue.as<ADXCommandQueue>()->queue.Get(), adxSwapChain->window, &swapChainDesc, nullptr, &oldSwapChainInterface)))
        return agpu::swap_chain_ref();
#endif
    if (FAILED(oldSwapChainInterface.As(&adxSwapChain->swapChain)))
        return agpu::swap_chain_ref();

    if(!adxSwapChain->createFrameBuffers(createInfo))
        return agpu::swap_chain_ref();

    return swapChain;
}

agpu::swap_chain_ref ADXSwapChain::createResizedSwapChain(const agpu::device_ref& device, const agpu::command_queue_ref& queue, agpu_swap_chain_create_info* createInfo)
{
    auto oldSwapChain = agpu::swap_chain_ref::import(createInfo->old_swap_chain);
    auto oldAdxSwapChain = oldSwapChain.as<ADXSwapChain>();
    if (!oldAdxSwapChain->swapChain)
        return agpu::swap_chain_ref();

    auto swapChain = agpu::makeObject<ADXSwapChain>(device);
    auto adxSwapChain = swapChain.as<ADXSwapChain>();

    adxSwapChain->frameCount = createInfo->buffer_count;
    adxSwapChain->overlayWindow = oldAdxSwapChain->overlayWindow;
    adxSwapChain->windowWidth = createInfo->width;
    adxSwapChain->windowHeight = createInfo->height;
    adxSwapChain->swapChain = oldAdxSwapChain->swapChain;

    oldAdxSwapChain->disconnect();
    if (adxSwapChain->overlayWindow)
    {
        adxSwapChain->overlayWindow->setSize(adxSwapChain->windowWidth, adxSwapChain->windowHeight);
    }

    DXGI_FORMAT newFormat = sanitizeSwapChainFormat((DXGI_FORMAT)createInfo->colorbuffer_format);
    UINT swapChainFlags = 0;
    if (FAILED(adxSwapChain->swapChain->ResizeBuffers(
        UINT(adxSwapChain->frameCount), adxSwapChain->windowWidth, adxSwapChain->windowHeight, newFormat, swapChainFlags)))
        return agpu::swap_chain_ref();

    if (!adxSwapChain->createFrameBuffers(createInfo))
        return agpu::swap_chain_ref();

    return swapChain;
}

void ADXSwapChain::disconnect()
{
    for (auto& colorBuffer : colorBuffers)
    {
        colorBuffer.as<ADXTexture>()->releaseTextureHandle();
    }

    colorBuffers.clear();
    framebuffers.clear();
    swapChain.Reset();
}

bool ADXSwapChain::createFrameBuffers(agpu_swap_chain_create_info* createInfo)
{
    this->frameIndex = swapChain->GetCurrentBackBufferIndex();

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
    depthStencilDesc.clear_value.depth_stencil.depth = 1.0f;
    depthStencilDesc.clear_value.depth_stencil.stencil = 0;
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
            return false;

        // Get the depth stencil buffer view description.
        depthStencilView = agpu::texture_view_ref(depthStencilBuffer->getOrCreateFullView());
    }

    // Create the main frame buffers.
    this->framebuffers.reserve(frameCount);
    this->colorBuffers.reserve(frameCount);
    for (int i = 0; i < this->frameCount; ++i)
    {
        ComPtr<ID3D12Resource> colorBufferResource;
        if (FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&colorBufferResource))))
            return false;

        // Create the color buffer.
        auto colorBuffer = ADXTexture::createFromResource(device, &colorDesc, colorBufferResource);
        if (!colorBuffer)
            return false;

        // Get the color buffer view description.
        auto colorBufferView = agpu::texture_view_ref(colorBuffer->getOrCreateFullView());

        auto framebuffer = ADXFramebuffer::create(device, this->windowWidth, this->windowHeight, 1, &colorBufferView, depthStencilView);
        framebuffer.as<ADXFramebuffer>()->swapChainBuffer = true;
        this->framebuffers.push_back(framebuffer);
        this->colorBuffers.push_back(colorBuffer);
    }

    return true;
}

agpu_error ADXSwapChain::swapBuffers()
{
    ERROR_IF_FAILED(swapChain->Present(1, 0));
    frameIndex = (frameIndex + 1) % frameCount;
    return AGPU_OK;
}

agpu::framebuffer_ptr ADXSwapChain::getCurrentBackBuffer()
{
	return getCurrentBackBufferForLayer(0);
}

agpu::framebuffer_ptr ADXSwapChain::getCurrentBackBufferForLayer(agpu_uint layer)
{
    if (layer != 0)
        return nullptr;
    return framebuffers[frameIndex].disownedNewRef();
}

agpu_size ADXSwapChain::getCurrentBackBufferIndex()
{
    return (agpu_size)frameIndex;
}

agpu_size ADXSwapChain::getFramebufferCount()
{
    return (agpu_size)framebuffers.size();
}

agpu_uint ADXSwapChain::getWidth()
{
    return windowWidth;
}

agpu_uint ADXSwapChain::getHeight()
{
    return windowHeight;
}

agpu_uint ADXSwapChain::getLayerCount()
{
    return 1;
}

agpu_error ADXSwapChain::setOverlayPosition(agpu_int x, agpu_int y)
{
	if (!overlayWindow)
		return AGPU_OK;

	return overlayWindow->setPosition(x, y);
}

} // End of namespace AgpuD3D12
