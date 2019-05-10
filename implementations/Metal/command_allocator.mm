#include "command_allocator.hpp"
#include "command_queue.hpp"

namespace AgpuMetal
{
    
AMtlCommandAllocator::AMtlCommandAllocator(const agpu::device_ref &device)
    : device(device)
{
}

AMtlCommandAllocator::~AMtlCommandAllocator()
{
}

agpu::command_allocator_ref AMtlCommandAllocator::create ( const agpu::device_ref &device, agpu_command_list_type type, const agpu::command_queue_ref &queue )
{
    if(!queue)
        return agpu::command_allocator_ref();

    auto result = agpu::makeObject<AMtlCommandAllocator> (device); 
    auto allocator = result.as<AMtlCommandAllocator> ();
    allocator->type = type;
    allocator->queue = queue;
    return result;
}

agpu_error AMtlCommandAllocator::reset()
{
    return AGPU_OK;
}

} // End of namespace AgpuMetal
