#include "command_allocator.hpp"
#include "command_queue.hpp"

_agpu_command_allocator::_agpu_command_allocator(agpu_device *device)
    : device(device)
{
    queue = nullptr;
}

void _agpu_command_allocator::lostReferences()
{
    if(queue)
        queue->release();
}

agpu_command_allocator* _agpu_command_allocator::create ( agpu_device* device, agpu_command_list_type type, agpu_command_queue* queue )
{
    if(!queue)
        return nullptr;

    std::unique_ptr<agpu_command_allocator> result(new agpu_command_allocator(device));
    result->type = type;
    result->queue = queue;
    queue->retain();
    return result.release();
}

agpu_error _agpu_command_allocator::reset()
{
    return AGPU_OK;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddCommandAllocatorReference ( agpu_command_allocator* command_allocator )
{
    CHECK_POINTER(command_allocator);
    return command_allocator->retain();
}

AGPU_EXPORT agpu_error agpuReleaseCommandAllocator ( agpu_command_allocator* command_allocator )
{
    CHECK_POINTER(command_allocator);
    return command_allocator->release();
}

AGPU_EXPORT agpu_error agpuResetCommandAllocator ( agpu_command_allocator* command_allocator )
{
    CHECK_POINTER(command_allocator);
    return command_allocator->reset();
}
