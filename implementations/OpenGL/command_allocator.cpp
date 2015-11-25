#include "command_allocator.hpp"

_agpu_command_allocator::_agpu_command_allocator()
{
	
}

void _agpu_command_allocator::lostReferences()
{
	
}

_agpu_command_allocator *_agpu_command_allocator::create(agpu_device *device, agpu_command_list_type type)
{
	auto allocator = new _agpu_command_allocator();
	allocator->device = device;
    allocator->type = type;
	return allocator;
}

agpu_error _agpu_command_allocator::reset()
{
	// No work needed.
	return AGPU_OK;
}

// Exported C interface
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
