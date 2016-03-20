#include "command_queue.hpp"

_agpu_command_queue::_agpu_command_queue(agpu_device *device)
    : device(device)
{
    queue = nullptr;
}

void _agpu_command_queue::lostReferences()
{
}

agpu_command_queue *_agpu_command_queue::create(agpu_device *device, agpu_uint queueFamilyIndex, agpu_uint queueIndex, VkQueue queue, agpu_command_queue_type type)
{
    std::unique_ptr<agpu_command_queue> commandQueue(new agpu_command_queue(device));
    commandQueue->queueFamilyIndex = queueFamilyIndex;
    commandQueue->queueIndex = queueIndex;
    commandQueue->queue = queue;
    commandQueue->type = type;
    return commandQueue.release();
}

bool _agpu_command_queue::supportsPresentingSurface(VkSurfaceKHR surface)
{
    VkBool32 presentSupported = 0;
    device->fpGetPhysicalDeviceSurfaceSupportKHR(device->physicalDevice, queueFamilyIndex, surface, &presentSupported);
    return presentSupported != 0;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddCommandQueueReference(agpu_command_queue* command_queue)
{
    CHECK_POINTER(command_queue);
    return command_queue->retain();
}

AGPU_EXPORT agpu_error agpuReleaseCommandQueue(agpu_command_queue* command_queue)
{
    CHECK_POINTER(command_queue);
    return command_queue->release();
}

AGPU_EXPORT agpu_error agpuAddCommandList(agpu_command_queue* command_queue, agpu_command_list* command_list)
{
    CHECK_POINTER(command_queue);
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuFinishQueueExecution(agpu_command_queue* command_queue)
{
    CHECK_POINTER(command_queue);
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSignalFence(agpu_command_queue* command_queue, agpu_fence* fence)
{
    CHECK_POINTER(command_queue);
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuWaitFence(agpu_command_queue* command_queue, agpu_fence* fence)
{
    CHECK_POINTER(command_queue);
    return AGPU_UNIMPLEMENTED;
}
