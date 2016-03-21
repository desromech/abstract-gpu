#include "command_allocator.hpp"

_agpu_command_allocator::_agpu_command_allocator()
{
	
}

void _agpu_command_allocator::lostReferences()
{
}

_agpu_command_allocator *_agpu_command_allocator::create(agpu_device *device, agpu_command_list_type type, agpu_command_queue *queue)
{
    // Create the command allocator.
    ComPtr<ID3D12CommandAllocator> allocator;
    if (FAILED(device->d3dDevice->CreateCommandAllocator(mapCommandListType(type), IID_PPV_ARGS(&allocator))))
        return nullptr;

    auto res = new agpu_command_allocator();
    res->device = device;
    res->allocator = allocator;
    return res;
}

agpu_error _agpu_command_allocator::reset()
{
    ERROR_IF_FAILED(allocator->Reset());
    return AGPU_OK;
}

// Exported C interface
AGPU_EXPORT agpu_error agpuAddCommandAllocatorReference(agpu_command_allocator* command_allocator)
{
    CHECK_POINTER(command_allocator);
    return command_allocator->retain();
}

AGPU_EXPORT agpu_error agpuReleaseCommandAllocator(agpu_command_allocator* command_allocator)
{
    CHECK_POINTER(command_allocator);
    return command_allocator->release();
}

AGPU_EXPORT agpu_error agpuResetCommandAllocator(agpu_command_allocator* command_allocator)
{
    CHECK_POINTER(command_allocator);
    return command_allocator->reset();
}
