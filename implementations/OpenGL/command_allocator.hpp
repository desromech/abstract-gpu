#ifndef AGPU_GL_COMMAND_ALLOCATOR_HPP_
#define AGPU_GL_COMMAND_ALLOCATOR_HPP_

#include "device.hpp"

struct _agpu_command_allocator : public Object<_agpu_command_allocator>
{
public:
    _agpu_command_allocator();

    void lostReferences();

    static _agpu_command_allocator *create(agpu_device *device, agpu_command_list_type type, agpu_command_queue *queue);

    agpu_error reset();

public:

    agpu_device *device;
    agpu_command_list_type type;
};

#endif //AGPU_GL_COMMAND_ALLOCATOR_HPP_
