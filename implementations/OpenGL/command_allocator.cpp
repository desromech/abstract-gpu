#include "command_allocator.hpp"

namespace AgpuGL
{

GLCommandAllocator::GLCommandAllocator()
{

}

GLCommandAllocator::~GLCommandAllocator()
{

}

agpu::command_allocator_ref GLCommandAllocator::create(const agpu::device_ref &device, agpu_command_list_type type, const agpu::command_queue_ref &queue)
{
	auto result = agpu::makeObject<GLCommandAllocator> ();
	auto allocator = result.as<GLCommandAllocator> ();
	allocator->device = device;
    allocator->type = type;
	return result;
}

agpu_error GLCommandAllocator::reset()
{
	// No work needed.
	return AGPU_OK;
}

} // End of namespace AgpuGL
