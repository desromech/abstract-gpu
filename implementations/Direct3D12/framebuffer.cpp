#include "framebuffer.hpp"

inline DXGI_FORMAT findDepthStencilFormat(agpu_uint depthSize, agpu_uint stencilSize)
{
    if (depthSize >= 32)
    {
        if (stencilSize)
            return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        return DXGI_FORMAT_D32_FLOAT;
    }
    else if(depthSize >= 24)
    {
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
    }
    else if (depthSize >= 16)
    {
        if(stencilSize)
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        return DXGI_FORMAT_D16_UNORM;
    }
    else
    {
        if(stencilSize)
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        return DXGI_FORMAT_D16_UNORM;
    }
}

_agpu_framebuffer::_agpu_framebuffer()
{
    isMainFrameBuffer = false;
}

void _agpu_framebuffer::lostReferences()
{

}

agpu_framebuffer* _agpu_framebuffer::create(agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint renderTargetCount, agpu_bool hasDepth, agpu_bool hasStencil)
{
    int heapSize = renderTargetCount;
    if (hasDepth || hasStencil)
        ++heapSize;

    // Describe and create a render target view (RTV) descriptor heap.
    ComPtr<ID3D12DescriptorHeap> heap;
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = heapSize;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        if (FAILED(device->d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap))))
            return nullptr;
    }

    // Describe and create a depth stencil view heap.
    ComPtr<ID3D12DescriptorHeap> depthStencilHeap;
    if (hasDepth || hasStencil)
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = heapSize;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        if (FAILED(device->d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&depthStencilHeap))))
            return nullptr;
    }

    // Create the framebuffer object.
    auto framebuffer = new agpu_framebuffer();
    framebuffer->width = width;
    framebuffer->height = height;
    framebuffer->device = device;
    framebuffer->colorBuffers.resize(renderTargetCount);
    framebuffer->hasDepth = hasDepth;
    framebuffer->hasStencil = hasStencil;
    framebuffer->heap = heap;
    framebuffer->depthStencilHeap = depthStencilHeap;
    framebuffer->descriptorSize = device->d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    framebuffer->colorBufferDescriptors.reserve(renderTargetCount);
    for (agpu_uint i = 0; i < renderTargetCount; ++i)
        framebuffer->colorBufferDescriptors.push_back(framebuffer->getColorBufferCpuHandle(i));
    
    return framebuffer;
}

agpu_error _agpu_framebuffer::attachRawColorBuffer(agpu_uint index, const ComPtr<ID3D12Resource> &colorBuffer)
{
    if (index >= colorBuffers.size())
        return AGPU_OUT_OF_BOUNDS;

    colorBuffers[index] = colorBuffer;

    // Perform the actual attachment.
    D3D12_CPU_DESCRIPTOR_HANDLE handle(heap->GetCPUDescriptorHandleForHeapStart());
    handle.ptr += index * descriptorSize;
    device->d3dDevice->CreateRenderTargetView(colorBuffer.Get(), nullptr, handle);
    return AGPU_OK;
}

agpu_error _agpu_framebuffer::createImplicitDepthStencil(agpu_uint depthSize, agpu_uint stencilSize)
{
    if (!depthSize && !stencilSize)
        return AGPU_OK;

    DXGI_FORMAT format = findDepthStencilFormat(depthSize, stencilSize);
    if (format == DXGI_FORMAT_UNKNOWN)
        return AGPU_ERROR;

    D3D12_HEAP_PROPERTIES heapProperties;
    memset(&heapProperties, 0, sizeof(heapProperties));
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Alignment = 0;
    desc.Width = width;
    desc.Height = height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;


    // Try to create already cleared.
    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = format;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    ERROR_IF_FAILED(device->d3dDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&depthStencil)));
    device->d3dDevice->CreateDepthStencilView(depthStencil.Get(), nullptr, depthStencilHeap->GetCPUDescriptorHandleForHeapStart());
    return AGPU_OK;
}

size_t _agpu_framebuffer::getColorBufferCount() const
{
    return colorBuffers.size();
}

D3D12_CPU_DESCRIPTOR_HANDLE _agpu_framebuffer::getColorBufferCpuHandle(size_t i)
{
    auto handle = heap->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += i * descriptorSize;
    return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE _agpu_framebuffer::getDepthStencilCpuHandle()
{
    return depthStencilHeap->GetCPUDescriptorHandleForHeapStart();
}

// Exported C interface
AGPU_EXPORT agpu_error agpuAddFramebufferReference(agpu_framebuffer* framebuffer)
{
    CHECK_POINTER(framebuffer);
    return framebuffer->retain();
}

AGPU_EXPORT agpu_error agpuReleaseFramebuffer(agpu_framebuffer* framebuffer)
{
    CHECK_POINTER(framebuffer);
    return framebuffer->release();
}

AGPU_EXPORT agpu_bool agpuisMainFrameBuffer(agpu_framebuffer* framebuffer)
{
    if (!framebuffer)
        return false;
    return framebuffer->isMainFrameBuffer;
}
