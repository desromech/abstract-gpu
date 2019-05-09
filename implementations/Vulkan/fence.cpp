#include "fence.hpp"

namespace AgpuVulkan
{

AVkFence::AVkFence(const agpu::device_ref &device)
    : device(device)
{
}

AVkFence::~AVkFence()
{
    vkDestroyFence(deviceForVk->device, fence, nullptr);
}

agpu::fence_ref AVkFence::create(const agpu::device_ref &device)
{
    VkFenceCreateInfo info;
    memset(&info, 0, sizeof(info));
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    VkFence fence;
    auto error = vkCreateFence(deviceForVk->device, &info, nullptr, &fence);
    if (error)
        return agpu::fence_ref();

    auto result = agpu::makeObject<AVkFence> (device);
    auto avkFence = result.as<AVkFence> ();
    avkFence->fence = fence;
    return result;
}

agpu_error AVkFence::waitOnClient()
{
    auto result = vkGetFenceStatus(deviceForVk->device, fence);
    if (result == VK_SUCCESS)
    {
        // Do nothing
    }
    else if (result == VK_NOT_READY)
    {
        auto error = vkWaitForFences(deviceForVk->device, 1, &fence, VK_TRUE, UINT64_MAX);
        CONVERT_VULKAN_ERROR(error);
    }
    else
    {
        CONVERT_VULKAN_ERROR(result);
        return AGPU_ERROR;
    }

    // Reset the fence.
    auto error = vkResetFences(deviceForVk->device, 1, &fence);
    CONVERT_VULKAN_ERROR(error);
    return AGPU_OK;

}

} // End of namespace AgpuVulkan
