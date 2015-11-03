#include "fence.hpp"

_agpu_fence::_agpu_fence()
{
    fenceValue = 1;
}

void _agpu_fence::lostReferences()
{
    CloseHandle(event);
}

_agpu_fence *_agpu_fence::create(agpu_device *device)
{
    std::unique_ptr<agpu_fence> fence(new agpu_fence());
    fence->device = device;

    // Create transfer synchronization fence.
    if (FAILED(device->d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence->fence))))
        return false;

    // Create an event handle to use for frame synchronization.
    fence->event = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
    if (fence->event == nullptr)
        return false;

    return fence.release();
}

agpu_error _agpu_fence::waitOnClient()
{
    UINT64 waitValue = 0;

    {
        std::unique_lock<std::mutex> (fenceMutex);
        waitValue = fenceValue - 1;
    }

    if (fence->GetCompletedValue() < waitValue)
    {
        ERROR_IF_FAILED(fence->SetEventOnCompletion(waitValue, event));
        WaitForSingleObject(event, INFINITE);
    }

    return AGPU_OK;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddFenceReference(agpu_fence* fence)
{
    CHECK_POINTER(fence);
    return fence->retain();
}

AGPU_EXPORT agpu_error agpuReleaseFenceReference(agpu_fence* fence)
{
    CHECK_POINTER(fence);
    return fence->release();
}

AGPU_EXPORT agpu_error agpuWaitOnClient(agpu_fence* fence)
{
    CHECK_POINTER(fence);
    return fence->waitOnClient();
}