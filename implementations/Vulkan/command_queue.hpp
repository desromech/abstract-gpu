#ifndef AGPU_COMMAND_QUEUE_HPP
#define AGPU_COMMAND_QUEUE_HPP

#include "device.hpp"

struct _agpu_command_queue : public Object<_agpu_command_queue>
{
public:
    _agpu_command_queue(agpu_device *device);
    void lostReferences();

    static agpu_command_queue *create(agpu_device *device, agpu_uint queueFamilyIndex, agpu_uint queueIndex, VkQueue queue, agpu_command_queue_type type);

    agpu_error addCommandList(agpu_command_list* command_list);
    agpu_error finishQueueExecution();
    agpu_error signalFence(agpu_fence* fence);
    agpu_error waitFence(agpu_fence* fence);

    bool supportsPresentingSurface(VkSurfaceKHR surface);

    agpu_device *device;
    agpu_uint queueFamilyIndex;
    agpu_uint queueIndex;
    VkQueue queue;
    agpu_command_queue_type type;
};

#endif //AGPU_COMMAND_QUEUE_HPP