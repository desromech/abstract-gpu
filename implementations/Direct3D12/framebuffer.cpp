#include "framebuffer.hpp"
#include "texture.hpp"

_agpu_framebuffer::_agpu_framebuffer()
{
    depthStencil = nullptr;
}

void _agpu_framebuffer::lostReferences()
{
    for (size_t i = 0; i < colorBuffers.size(); ++i)
    {
        auto colorBuffer = colorBuffers[i];
        if (colorBuffer)
            colorBuffer->release();
    }

    if (depthStencil)
        depthStencil->release();
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
    framebuffer->colorBuffers.resize(renderTargetCount, nullptr);
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

agpu_error _agpu_framebuffer::attachColorBuffer(agpu_int index, agpu_texture* buffer)
{
    CHECK_POINTER(buffer);
    if (index >= colorBuffers.size())
        return AGPU_OUT_OF_BOUNDS;

    // Store the new color buffer.
    buffer->retain();
    if (colorBuffers[index])
        colorBuffers[index]->release();
    colorBuffers[index] = buffer;

    // Perform the actual attachment.
    D3D12_CPU_DESCRIPTOR_HANDLE handle(heap->GetCPUDescriptorHandleForHeapStart());
    handle.ptr += index * descriptorSize;
    device->d3dDevice->CreateRenderTargetView(buffer->gpuResource.Get(), nullptr, handle);
    return AGPU_OK;
}

agpu_error _agpu_framebuffer::attachDepthStencilBuffer(agpu_texture* buffer)
{
    CHECK_POINTER(buffer);

    // Store the new depth stencil buffer.
    buffer->retain();
    if (depthStencil)
        depthStencil->release();
    depthStencil = buffer;

    // Perform the actual attachment.
    device->d3dDevice->CreateDepthStencilView(depthStencil->gpuResource.Get(), nullptr, depthStencilHeap->GetCPUDescriptorHandleForHeapStart());
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

AGPU_EXPORT agpu_error agpuAttachColorBuffer(agpu_framebuffer* framebuffer, agpu_int index, agpu_texture* buffer)
{
    CHECK_POINTER(framebuffer);
    return framebuffer->attachColorBuffer(index, buffer);
}

AGPU_EXPORT agpu_error agpuAttachDepthStencilBuffer(agpu_framebuffer* framebuffer, agpu_texture* buffer)
{
    CHECK_POINTER(framebuffer);
    return framebuffer->attachDepthStencilBuffer(buffer);
}
