#ifndef AGPU_GL_COMMAND_ALLOCATOR_HPP_
#define AGPU_GL_COMMAND_ALLOCATOR_HPP_

#include "device.hpp"

namespace AgpuGL
{

struct GLCommandAllocator : public agpu::command_allocator
{
public:
    GLCommandAllocator();
    ~GLCommandAllocator();

    static agpu::command_allocator_ref create(const agpu::device_ref &device, agpu_command_list_type type, const agpu::command_queue_ref &queue);

    virtual agpu_error reset() override;

public:
    agpu::device_ref device;
    agpu_command_list_type type;
};

} // End of namespace AgpuGL

#endif //AGPU_GL_COMMAND_ALLOCATOR_HPP_
