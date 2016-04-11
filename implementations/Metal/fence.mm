#include "fence.hpp"

_agpu_fence::_agpu_fence(agpu_device *device)
    : device(device)
{
    fenceCommand = nil;
}

void _agpu_fence::lostReferences()
{
    if(fenceCommand)
    {
        [fenceCommand release];
        fenceCommand = nil;
    }
}

agpu_fence *_agpu_fence::create(agpu_device *device)
{
    return new agpu_fence(device);
}

agpu_error _agpu_fence::waitOnClient()
{
    std::unique_lock<std::mutex> l(mutex);
    if(fenceCommand)
    {
        [fenceCommand waitUntilCompleted];
        [fenceCommand release];
        fenceCommand = nil;
    }

    return AGPU_OK;
}

agpu_error _agpu_fence::signalOnQueue(id<MTLCommandQueue> queue)
{
    std::unique_lock<std::mutex> l(mutex);
    if(fenceCommand)
        [fenceCommand release];

    fenceCommand = [queue commandBuffer];
    [fenceCommand commit];
    return AGPU_OK;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddFenceReference ( agpu_fence* fence )
{
    CHECK_POINTER(fence);
    return fence->retain();
}

AGPU_EXPORT agpu_error agpuReleaseFenceReference ( agpu_fence* fence )
{
    CHECK_POINTER(fence);
    return fence->release();
}

AGPU_EXPORT agpu_error agpuWaitOnClient ( agpu_fence* fence )
{
    CHECK_POINTER(fence);
    return fence->waitOnClient();
}
