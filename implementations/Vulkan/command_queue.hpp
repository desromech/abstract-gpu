#ifndef AGPU_COMMAND_QUEUE_HPP
#define AGPU_COMMAND_QUEUE_HPP

#include "device.hpp"

struct _agpu_command_queue : public Object<_agpu_command_queue>
{
public:
    _agpu_command_queue(agpu_device *device);
    void lostReferences();

    static agpu_command_queue *create(agpu_device *device, agpu_uint queueFamilyIndex, agpu_uint queueIndex, VkQueue queue, agpu_command_queue_type type);

    bool supportsPresentingSurface(VkSurfaceKHR surface);

    agpu_device *device;
    agpu_uint queueFamilyIndex;
    agpu_uint queueIndex;
    VkQueue queue;
    agpu_command_queue_type type;
};

#endif //AGPU_COMMAND_QUEUE_HPP