#ifndef AGPU_D3D12_FRAMEBUFFER_HPP
#define AGPU_D3D12_FRAMEBUFFER_HPP

#include <vector>
#include "device.hpp"

struct _agpu_framebuffer : public Object<_agpu_framebuffer>
{
public:
    _agpu_framebuffer();

    void lostReferences();

    static agpu_framebuffer* create(agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint renderTargetCount, agpu_bool hasDepth, agpu_bool hasStencil);
    
    agpu_error attachColorBuffer(agpu_int index, agpu_texture* buffer);
    agpu_error attachDepthStencilBuffer(agpu_texture* buffer);

    size_t getColorBufferCount() const;
    D3D12_CPU_DESCRIPTOR_HANDLE getColorBufferCpuHandle(size_t i);
    D3D12_CPU_DESCRIPTOR_HANDLE getDepthStencilCpuHandle();

    
public:
    agpu_device *device;

    agpu_uint width;
    agpu_uint height;
    bool hasDepth;
    bool hasStencil;

    size_t descriptorSize;
    ComPtr<ID3D12DescriptorHeap> heap;
    ComPtr<ID3D12DescriptorHeap> depthStencilHeap;

    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> colorBufferDescriptors;

    std::vector<agpu_texture*> colorBuffers;
    agpu_texture* depthStencil;
    bool swapChainBuffer;
};

#endif //AGPU_D3D12_FRAMEBUFFER_HPP
