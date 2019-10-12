#ifndef AGPU_D3D12_FRAMEBUFFER_HPP
#define AGPU_D3D12_FRAMEBUFFER_HPP

#include <vector>
#include "device.hpp"

namespace AgpuD3D12
{

class ADXFramebuffer : public agpu::framebuffer
{
public:
    ADXFramebuffer(const agpu::device_ref &cdevice);
    ~ADXFramebuffer();

    static agpu::framebuffer_ref create(const agpu::device_ref &device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu::texture_view_ref* colorViews, const agpu::texture_view_ref &depthStencilView);

    agpu_error attachColorBuffer(agpu_int index, const agpu::texture_view_ref &textureView);
    agpu_error attachDepthStencilBuffer(const agpu::texture_view_ref &textureView);

    size_t getColorBufferCount() const;
    D3D12_CPU_DESCRIPTOR_HANDLE getColorBufferCpuHandle(size_t i);
    D3D12_CPU_DESCRIPTOR_HANDLE getDepthStencilCpuHandle();


public:
    agpu::device_ref device;

    agpu_uint width;
    agpu_uint height;
    bool hasDepth;
    bool hasStencil;

    size_t descriptorSize;
    ComPtr<ID3D12DescriptorHeap> heap;
    ComPtr<ID3D12DescriptorHeap> depthStencilHeap;

    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> colorBufferDescriptors;

    std::vector<agpu::texture_view_ref> colorBufferViews;
    std::vector<agpu::texture_ref> colorBuffers; // For life cycle management
    agpu::texture_view_ref depthStencilView;
    agpu::texture_ref depthStencilBuffer;
    bool swapChainBuffer;
};

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_FRAMEBUFFER_HPP
