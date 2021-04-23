#include "command_queue.hpp"
#include "command_list.hpp"
#include "fence.hpp"

namespace AgpuMetal
{
    
AMtlCommandQueue::AMtlCommandQueue(const agpu::device_ref &device)
    : weakDevice(device)
{
    handle = nil;
}

AMtlCommandQueue::~AMtlCommandQueue()
{
    if(handle)
        [handle release];
}

agpu::command_queue_ref AMtlCommandQueue::create(const agpu::device_ref &device, id<MTLCommandQueue> handle)
{
    auto finishFence = AMtlFence::create(device);
    if(!finishFence)
        return agpu::command_queue_ref();

    auto result = agpu::makeObject<AMtlCommandQueue> (device);
    auto queue = result.as<AMtlCommandQueue> ();
    queue->handle = handle;
    queue->finishFence = finishFence;
    return result;
}

agpu_error AMtlCommandQueue::addCommandList(const agpu::command_list_ref &command_list)
{
    CHECK_POINTER(command_list);
    auto amtlCommandList = command_list.as<AMtlCommandList> ();
    if(amtlCommandList->used)
    {
        printf("TODO: Recreate a command list for resubmitting\n");
        return AGPU_OK;
    }

    if(!amtlCommandList->buffer)
        return AGPU_INVALID_PARAMETER;

    [amtlCommandList->buffer commit];
    amtlCommandList->used = true;
    return AGPU_OK;
}

agpu_error AMtlCommandQueue::addCommandListsAndSignalFence(agpu_uint count, agpu::command_list_ref* command_lists, const agpu::fence_ref & fence)
{
    for(agpu_uint i = 0; i < count; ++i)
    {
        auto error = addCommandList(command_lists[i]);
        if(error)
            return error;
    }

    if(fence)
        return signalFence(fence);
    return AGPU_OK;
}

agpu_error AMtlCommandQueue::finishExecution (  )
{
    std::unique_lock<std::mutex> l(finishFenceMutex);
    signalFence(finishFence);
    return finishFence->waitOnClient();
}

agpu_error AMtlCommandQueue::signalFence(const agpu::fence_ref &fence)
{
    CHECK_POINTER(fence);
    return fence.as<AMtlFence> ()->signalOnQueue(handle);
}

agpu_error AMtlCommandQueue::waitFence(const agpu::fence_ref &fence)
{
    CHECK_POINTER(fence);
    return AGPU_UNIMPLEMENTED;
}

} // End of namespace AgpuMetal
