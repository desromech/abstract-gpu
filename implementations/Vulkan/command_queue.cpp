#include "command_queue.hpp"
#include "command_list.hpp"
#include "fence.hpp"

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

agpu_error _agpu_command_queue::addCommandList(agpu_command_list* command_list)
{
    CHECK_POINTER(command_list);
    if (command_list->queueFamilyIndex != queueFamilyIndex)
        return AGPU_INVALID_PARAMETER;

    VkSubmitInfo submitInfo;
    memset(&submitInfo, 0, sizeof(VkSubmitInfo));
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &command_list->commandBuffer;

    auto error = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    CONVERT_VULKAN_ERROR(error);
    return AGPU_OK;
}

agpu_error _agpu_command_queue::finishQueueExecution()
{
    auto error = vkQueueWaitIdle(queue);
    CONVERT_VULKAN_ERROR(error);
    return AGPU_OK;
}

agpu_error _agpu_command_queue::signalFence(agpu_fence* fence)
{
    CHECK_POINTER(fence);

    auto error = vkQueueSubmit(queue, 0, nullptr, fence->fence);
    CONVERT_VULKAN_ERROR(error);
    return AGPU_OK;
}

agpu_error _agpu_command_queue::waitFence(agpu_fence* fence)
{
    CHECK_POINTER(fence);
    return AGPU_UNSUPPORTED;
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
    return command_queue->addCommandList(command_list);
}

AGPU_EXPORT agpu_error agpuFinishQueueExecution(agpu_command_queue* command_queue)
{
    CHECK_POINTER(command_queue);
    return command_queue->finishQueueExecution();
}

AGPU_EXPORT agpu_error agpuSignalFence(agpu_command_queue* command_queue, agpu_fence* fence)
{
    CHECK_POINTER(command_queue);
    return command_queue->signalFence(fence);
}

AGPU_EXPORT agpu_error agpuWaitFence(agpu_command_queue* command_queue, agpu_fence* fence)
{
    CHECK_POINTER(command_queue);
    return command_queue->waitFence(fence);
}
