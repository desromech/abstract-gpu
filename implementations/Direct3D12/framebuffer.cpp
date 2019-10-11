#include "framebuffer.hpp"
#include "texture.hpp"
#include "texture_view.hpp"

namespace AgpuD3D12
{

ADXFramebuffer::ADXFramebuffer(const agpu::device_ref &cdevice)
    : device(cdevice)
{
    swapChainBuffer = false;
}

ADXFramebuffer::~ADXFramebuffer()
{
}

agpu::framebuffer_ref ADXFramebuffer::create(const agpu::device_ref &device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu::texture_view_ref* colorViews, const agpu::texture_view_ref &depthStencilView)
{
    int heapSize = colorCount;
    bool hasDepth = false;
    bool hasStencil = false;
    if (depthStencilView)
    {
        auto adxDepthStencilView = depthStencilView.as<ADXTextureView> ();
        hasDepth = (adxDepthStencilView->description.subresource_range.usage_mode & AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT) != 0;
        hasStencil = (adxDepthStencilView->description.subresource_range.usage_mode & AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT) != 0;
    }

    // Describe and create a render target view (RTV) descriptor heap.
    ComPtr<ID3D12DescriptorHeap> heap;
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = heapSize;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        if (FAILED(deviceForDX->d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap))))
            return agpu::framebuffer_ref();
    }

    // Describe and create a depth stencil view heap.
    ComPtr<ID3D12DescriptorHeap> depthStencilHeap;
    if (depthStencilView)
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = 1;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        if (FAILED(deviceForDX->d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&depthStencilHeap))))
            return agpu::framebuffer_ref();
    }

    // Create the framebuffer object.
    auto framebuffer = agpu::makeObject<ADXFramebuffer> (device);
    auto adxFramebuffer = framebuffer.as<ADXFramebuffer> ();
    adxFramebuffer->width = width;
    adxFramebuffer->height = height;
    adxFramebuffer->colorBufferViews.resize(colorCount);
    adxFramebuffer->colorBuffers.resize(colorCount);
    adxFramebuffer->hasDepth = hasDepth;
    adxFramebuffer->hasStencil = hasStencil;
    adxFramebuffer->heap = heap;
    adxFramebuffer->depthStencilHeap = depthStencilHeap;
    adxFramebuffer->descriptorSize = deviceForDX->d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    adxFramebuffer->colorBufferDescriptors.reserve(colorCount);
    for (agpu_uint i = 0; i < colorCount; ++i)
    {
        adxFramebuffer->colorBufferDescriptors.push_back(adxFramebuffer->getColorBufferCpuHandle(i));
    }

    // Attach the views.
    for (agpu_uint i = 0; i < colorCount; ++i)
    {
        adxFramebuffer->attachColorBuffer(i, colorViews[i]);
    }

    if (depthStencilView)
    {
        adxFramebuffer->attachDepthStencilBuffer(depthStencilView);
    }

    return framebuffer;
}

agpu_error ADXFramebuffer::attachColorBuffer(agpu_int index, const agpu::texture_view_ref &textureView)
{
    CHECK_POINTER(textureView);
    auto adxTextureView = textureView.as<ADXTextureView> ();
    auto texture = adxTextureView->texture.lock();
    CHECK_POINTER(texture);

    if ((size_t)index >= colorBuffers.size())
        return AGPU_OUT_OF_BOUNDS;

    // View description.
    D3D12_RENDER_TARGET_VIEW_DESC viewDescription = {};
    auto error = adxTextureView->getColorAttachmentViewDescription(&viewDescription);
    if(error) return error;

    // Store the new color buffer.
    colorBufferViews[index] = textureView;
    colorBuffers[index] = texture;

    // Perform the actual attachment.
    D3D12_CPU_DESCRIPTOR_HANDLE handle(heap->GetCPUDescriptorHandleForHeapStart());
    handle.ptr += index * descriptorSize;
    deviceForDX->d3dDevice->CreateRenderTargetView(texture.as<ADXTexture> ()->resource.Get(), &viewDescription, handle);
    return AGPU_OK;
}

agpu_error ADXFramebuffer::attachDepthStencilBuffer(const agpu::texture_view_ref &textureView)
{
    CHECK_POINTER(textureView);
    auto adxTextureView = textureView.as<ADXTextureView> ();
    auto texture = adxTextureView->texture.lock();
    CHECK_POINTER(texture);

    // View description.
    D3D12_DEPTH_STENCIL_VIEW_DESC viewDescription = {};
    auto error = adxTextureView->getDepthStencilViewDescription(&viewDescription);
    if(error) return error;

    // Store the new depth stencil buffer.
    depthStencilView = textureView;
    depthStencilBuffer = texture;

    // Perform the actual attachment.
    deviceForDX->d3dDevice->CreateDepthStencilView(texture.as<ADXTexture> ()->resource.Get(), &viewDescription, depthStencilHeap->GetCPUDescriptorHandleForHeapStart());
    return AGPU_OK;
}

size_t ADXFramebuffer::getColorBufferCount() const
{
    return colorBufferViews.size();
}

D3D12_CPU_DESCRIPTOR_HANDLE ADXFramebuffer::getColorBufferCpuHandle(size_t i)
{
    auto handle = heap->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += i * descriptorSize;
    return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE ADXFramebuffer::getDepthStencilCpuHandle()
{
    return depthStencilHeap->GetCPUDescriptorHandleForHeapStart();
}

} // End of namespace AgpuD3D12
