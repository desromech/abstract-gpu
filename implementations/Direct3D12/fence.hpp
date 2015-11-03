#ifndef AGPU_D3D12_FENCE_HPP
#define AGPU_D3D12_FENCE_HPP

#include "device.hpp"

struct _agpu_fence : public Object<_agpu_fence>
{
public:
    _agpu_fence();

    void lostReferences();

    static _agpu_fence *create(agpu_device *device);
    
    agpu_error waitOnClient();

public:
    std::mutex fenceMutex;
    agpu_device *device;
    HANDLE event;
    ComPtr<ID3D12Fence> fence;
    UINT64 fenceValue;
};

#endif //AGPU_D3D12_FENCE_HPP
