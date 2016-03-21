#ifndef AGPU_COMMAND_ALLOCATOR_HPP
#define AGPU_COMMAND_ALLOCATOR_HPP

#include "device.hpp"

struct _agpu_command_allocator : public Object<_agpu_command_allocator>
{
public:
    _agpu_command_allocator(agpu_device *device);
    void lostReferences();

    static agpu_command_allocator* create(agpu_device* device, agpu_command_list_type type, agpu_command_queue *commandQueue);

    agpu_error reset();

    agpu_device *device;
    agpu_command_list_type type;
    agpu_uint queueFamilyIndex;
    VkCommandPool commandPool;
};

#endif //AGPU_COMMAND_ALLOCATOR_HPP