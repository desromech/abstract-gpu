#ifndef AGPU_COMMAND_ALLOCATOR_HPP
#define AGPU_COMMAND_ALLOCATOR_HPP

#include "device.hpp"

namespace AgpuVulkan
{

class AVkCommandAllocator : public agpu::command_allocator
{
public:
    AVkCommandAllocator(const agpu::device_ref &device);
    ~AVkCommandAllocator();

    static agpu::command_allocator_ref create(const agpu::device_ref &device, agpu_command_list_type type, const agpu::command_queue_ref &commandQueue);

    virtual agpu_error reset() override;

    agpu::device_ref device;
    agpu_command_list_type type;
    agpu_uint queueFamilyIndex;
    VkCommandPool commandPool;
};

} // End of namespace AgpuVulkan

#endif //AGPU_COMMAND_ALLOCATOR_HPP
