#include "command_queue.hpp"


_agpu_command_queue::_agpu_command_queue(agpu_device *device)
    : device(device)
{
    handle = nil;
}

void _agpu_command_queue::lostReferences()
{
}

_agpu_command_queue *_agpu_command_queue::create(agpu_device *device, id<MTLCommandQueue> handle)
{
    std::unique_ptr<agpu_command_queue> result(new agpu_command_queue(device));
    result->handle = handle;
    return result.release();
}

agpu_error _agpu_command_queue::addCommandList ( agpu_command_list* command_list )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_queue::finishExecution (  )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_queue::signalFence ( agpu_fence* fence )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_queue::waitFence ( agpu_fence* fence )
{
    return AGPU_UNIMPLEMENTED;
}

// The exported C interface.
AGPU_EXPORT agpu_error agpuAddCommandQueueReference ( agpu_command_queue* command_queue )
{
    CHECK_POINTER(command_queue);
    return command_queue->retain();
}

AGPU_EXPORT agpu_error agpuReleaseCommandQueue ( agpu_command_queue* command_queue )
{
    CHECK_POINTER(command_queue);
    return command_queue->release();
}

AGPU_EXPORT agpu_error agpuAddCommandList ( agpu_command_queue* command_queue, agpu_command_list* command_list )
{
    CHECK_POINTER(command_queue);
    return command_queue->addCommandList(command_list);
}

AGPU_EXPORT agpu_error agpuFinishQueueExecution ( agpu_command_queue* command_queue )
{
    CHECK_POINTER(command_queue);
    return command_queue->finishExecution();
}

AGPU_EXPORT agpu_error agpuSignalFence ( agpu_command_queue* command_queue, agpu_fence* fence )
{
    CHECK_POINTER(command_queue);
    return command_queue->signalFence(fence);
}

AGPU_EXPORT agpu_error agpuWaitFence ( agpu_command_queue* command_queue, agpu_fence* fence )
{
    CHECK_POINTER(command_queue);
    return command_queue->waitFence(fence);
}
