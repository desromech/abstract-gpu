#include "fence.hpp"

namespace AgpuMetal
{
    
AMtlFence::AMtlFence(const agpu::device_ref &device)
    : weakDevice(device)
{
    fenceCommand = nil;
    isSignaled = false;
}

AMtlFence::~AMtlFence()
{
    if(fenceCommand)
    {
        [fenceCommand release];
        fenceCommand = nil;
    }
}

agpu::fence_ref AMtlFence::create(const agpu::device_ref &device)
{
    return agpu::makeObject<AMtlFence> (device);
}

agpu_error AMtlFence::waitOnClient()
{
    std::unique_lock<std::mutex> l(mutex);
    if(fenceCommand)
    {
        while(!isSignaled)
            signaledCondition.wait(l);
        if(fenceCommand)
        {
            [fenceCommand waitUntilCompleted];
            [fenceCommand release];
            fenceCommand = nil;            
        }
    }

    return AGPU_OK;
}

agpu_error AMtlFence::signalOnQueue(id<MTLCommandQueue> queue)
{
    std::unique_lock<std::mutex> l(mutex);
    if(fenceCommand)
        [fenceCommand release];

    isSignaled = false;
    fenceCommand = [queue commandBuffer];
    [fenceCommand addCompletedHandler:^(id<MTLCommandBuffer> cb) {
        std::unique_lock<std::mutex> l(mutex);
        isSignaled = true;
        signaledCondition.notify_all();
    }];
    [fenceCommand commit];
    return AGPU_OK;
}

} // End of namespace AgpuMetal
