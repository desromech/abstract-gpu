#include "command_queue.hpp"
#include "command_list.hpp"
#include "fence.hpp"

namespace AgpuVulkan
{

AVkCommandQueue::AVkCommandQueue(const agpu::device_ref &device)
    : weakDevice(device)
{
    queue = nullptr;
}

AVkCommandQueue::~AVkCommandQueue()
{
}

agpu::command_queue_ref AVkCommandQueue::create(const agpu::device_ref &device, agpu_uint queueFamilyIndex, agpu_uint queueIndex, VkQueue queue, agpu_command_queue_type type)
{
    auto result = agpu::makeObject<AVkCommandQueue> (device);
    auto commandQueue = result.as<AVkCommandQueue> ();
    commandQueue->queueFamilyIndex = queueFamilyIndex;
    commandQueue->queueIndex = queueIndex;
    commandQueue->queue = queue;
    commandQueue->type = type;
    return result;
}

bool AVkCommandQueue::supportsPresentingSurface(VkSurfaceKHR surface)
{
    VkBool32 presentSupported = 0;
    auto device = weakDevice.lock();
    deviceForVk->fpGetPhysicalDeviceSurfaceSupportKHR(deviceForVk->physicalDevice, queueFamilyIndex, surface, &presentSupported);
    return presentSupported != 0;
}

agpu_error AVkCommandQueue::addCommandList(const agpu::command_list_ref &command_list)
{
    CHECK_POINTER(command_list);
    auto avkCommandList = command_list.as<AVkCommandList> ();
    if (avkCommandList->queueFamilyIndex != queueFamilyIndex)
        return AGPU_INVALID_PARAMETER;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = avkCommandList->waitSemaphores.size();
    submitInfo.pWaitSemaphores = avkCommandList->waitSemaphores.data();
    submitInfo.pWaitDstStageMask = avkCommandList->waitSemaphoresDstStageMask.data();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &avkCommandList->commandBuffer;
    submitInfo.signalSemaphoreCount = avkCommandList->signalSemaphores.size();
    submitInfo.pSignalSemaphores = avkCommandList->signalSemaphores.data();

    auto error = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    CONVERT_VULKAN_ERROR(error);
    return AGPU_OK;
}

agpu_error AVkCommandQueue::finishExecution()
{
    auto error = vkQueueWaitIdle(queue);
    CONVERT_VULKAN_ERROR(error);
    return AGPU_OK;
}

agpu_error AVkCommandQueue::signalFence(const agpu::fence_ref &fence)
{
    CHECK_POINTER(fence);

    auto error = vkQueueSubmit(queue, 0, nullptr, fence.as<AVkFence> ()->fence);
    CONVERT_VULKAN_ERROR(error);
    return AGPU_OK;
}

agpu_error AVkCommandQueue::waitFence(const agpu::fence_ref &fence)
{
    CHECK_POINTER(fence);
    return AGPU_UNSUPPORTED;
}

} // End of namespace AgpuVulkan
