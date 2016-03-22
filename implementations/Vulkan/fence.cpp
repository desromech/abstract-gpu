#include "fence.hpp"

_agpu_fence::_agpu_fence(agpu_device *device)
    : device(device)
{
}

void _agpu_fence::lostReferences()
{
    vkDestroyFence(device->device, fence, nullptr);
}

agpu_fence *_agpu_fence::create(agpu_device *device)
{
    VkFenceCreateInfo info;
    memset(&info, 0, sizeof(info));
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    VkFence fence;
    auto error = vkCreateFence(device->device, &info, nullptr, &fence);
    if (error)
        return nullptr;

    auto result = new agpu_fence(device);
    result->fence = fence;
    return result;
}

agpu_error _agpu_fence::waitOnClient()
{
    auto result = vkGetFenceStatus(device->device, fence);
    if (result == VK_SUCCESS)
    {
        // Do nothing
    }
    else if (result == VK_NOT_READY)
    {
        auto error = vkWaitForFences(device->device, 1, &fence, VK_TRUE, UINT64_MAX);
        CONVERT_VULKAN_ERROR(error);
    }
    else
    {
        CONVERT_VULKAN_ERROR(result);
        return AGPU_ERROR;
    }

    // Reset the fence.
    auto error = vkResetFences(device->device, 1, &fence);
    CONVERT_VULKAN_ERROR(error);
    return AGPU_OK;
    
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddFenceReference(agpu_fence* fence)
{
    CHECK_POINTER(fence);
    return fence->retain();
}

AGPU_EXPORT agpu_error agpuReleaseFenceReference(agpu_fence* fence)
{
    CHECK_POINTER(fence);
    return fence->release();
}

AGPU_EXPORT agpu_error agpuWaitOnClient(agpu_fence* fence)
{
    CHECK_POINTER(fence);
    return fence->waitOnClient();
}
