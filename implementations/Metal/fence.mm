#include "fence.hpp"
#include "../Common/memory_profiler.hpp"

namespace AgpuMetal
{
    
AMtlFence::AMtlFence(const agpu::device_ref &device)
    : weakDevice(device)
{
    AgpuProfileConstructor(AMtlFence);
    isSignaled = false;
}

AMtlFence::~AMtlFence()
{
    AgpuProfileDestructor(AMtlFence);
    if(fenceCommand)
        [fenceCommand waitUntilCompleted];
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
            fenceCommand = nil;
        }
    }

    return AGPU_OK;
}

agpu_error AMtlFence::signalOnQueue(id<MTLCommandQueue> queue)
{
    @autoreleasepool {
        std::unique_lock<std::mutex> l(mutex);
        isSignaled = false;
        if(fenceCommand)
            [fenceCommand waitUntilCompleted];
        fenceCommand = [queue commandBuffer];
        [fenceCommand addCompletedHandler:^(id<MTLCommandBuffer> cb) {
            std::unique_lock<std::mutex> l(mutex);
            isSignaled = true;
            signaledCondition.notify_all();
        }];
        [fenceCommand commit];
        return AGPU_OK;
    }
}

} // End of namespace AgpuMetal
