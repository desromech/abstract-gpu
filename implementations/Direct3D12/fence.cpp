#include "fence.hpp"

namespace AgpuD3D12
{

ADXFence::ADXFence(const agpu::device_ref &cdevice)
    : device(cdevice)
{
    fenceValue = 1;
    event = NULL;
}

ADXFence::~ADXFence()
{
    if(event)
        CloseHandle(event);
}

agpu::fence_ref ADXFence::create(const agpu::device_ref &device)
{
    auto result = agpu::makeObject<ADXFence> (device);
    auto dxFence = result.as<ADXFence> ();

    // Create transfer synchronization fence.
    if (FAILED(deviceForDX->d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&dxFence->fence))))
        return agpu::fence_ref();

    // Create an event handle to use for frame synchronization.
    dxFence->event = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
    if (dxFence->event == nullptr)
        return agpu::fence_ref();

    return result;
}

agpu_error ADXFence::waitOnClient()
{
    UINT64 waitValue = 0;

    // FIXME: Replace this mutex with atomic operations.
    {
        std::unique_lock<std::mutex> l(fenceMutex);
        waitValue = fenceValue - 1;
    }

    if (fence->GetCompletedValue() < waitValue)
    {
        ERROR_IF_FAILED(fence->SetEventOnCompletion(waitValue, event));
        WaitForSingleObject(event, INFINITE);
    }

    return AGPU_OK;
}

} // End of namespace AgpuD3D12
