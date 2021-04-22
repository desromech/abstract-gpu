#ifndef AGPU_COMMON_COMMANDS_HPP_
#define AGPU_COMMON_COMMANDS_HPP_

#include "device.hpp"

namespace AgpuD3D12
{

inline D3D12_RESOURCE_BARRIER resourceTransitionBarrier(ID3D12Resource *resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
{
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = resource;
    barrier.Transition.StateBefore = before;
    barrier.Transition.StateAfter = after;
    barrier.Transition.Subresource = subresource;
    return barrier;
}

} // End of namespace AgpuD3D12

#endif //AGPU_COMMON_COMMANDS_HPP_
