#ifndef AGPU_COMMON_COMMANDS_HPP_
#define AGPU_COMMON_COMMANDS_HPP_

#include "device.hpp"

inline D3D12_RESOURCE_BARRIER resourceTransitionBarrier(const ComPtr<ID3D12Resource> &resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
    D3D12_RESOURCE_BARRIER barrier;
    memset(&barrier, 0, sizeof(barrier));
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = resource.Get();
    barrier.Transition.StateBefore = before;
    barrier.Transition.StateAfter = after;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    return barrier;
}


#endif //AGPU_COMMON_COMMANDS_HPP_
