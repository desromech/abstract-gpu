#ifndef AGPU_METAL_COMMAND_ALLOCATOR_HPP
#define AGPU_METAL_COMMAND_ALLOCATOR_HPP

#include "device.hpp"

namespace AgpuMetal
{
    
class AMtlCommandAllocator : public agpu::command_allocator
{
public:
    AMtlCommandAllocator(const agpu::device_ref &device);
    ~AMtlCommandAllocator();

    static agpu::command_allocator_ref create ( const agpu::device_ref &device, agpu_command_list_type type, const agpu::command_queue_ref &queue );

    virtual agpu_error reset() override;

    agpu::device_ref device;
    agpu_command_list_type type;
    agpu::command_queue_ref queue;
};

} // End of namespace AgpuMetal

#endif //AGPU_METAL_COMMAND_ALLOCATOR_HPP
