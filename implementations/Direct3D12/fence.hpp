#ifndef AGPU_D3D12_FENCE_HPP
#define AGPU_D3D12_FENCE_HPP

#include "device.hpp"

namespace AgpuD3D12
{

class ADXFence : public agpu::fence
{
public:
    ADXFence(const agpu::device_ref &cdevice);
    ~ADXFence();

    static agpu::fence_ref create(const agpu::device_ref &device);

    virtual agpu_error waitOnClient() override;

public:
    std::mutex fenceMutex;
    agpu::device_ref device;
    HANDLE event;
    ComPtr<ID3D12Fence> fence;
    UINT64 fenceValue;
};

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_FENCE_HPP
