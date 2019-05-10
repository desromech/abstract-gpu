#include "command_allocator.hpp"
#include "command_queue.hpp"

namespace AgpuVulkan
{

AVkCommandAllocator::AVkCommandAllocator(const agpu::device_ref &device)
    : device(device)
{
    commandPool = VK_NULL_HANDLE;
}

AVkCommandAllocator::~AVkCommandAllocator()
{
    vkDestroyCommandPool(deviceForVk->device, commandPool, nullptr);
}

agpu::command_allocator_ref AVkCommandAllocator::create(const agpu::device_ref &device, agpu_command_list_type type, const agpu::command_queue_ref &commandQueue)
{
    if (!commandQueue)
        return agpu::command_allocator_ref();

    auto queueFamilyIndex = commandQueue.as<AVkCommandQueue> ()->queueFamilyIndex;
    VkCommandPoolCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(createInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = queueFamilyIndex;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkCommandPool pool;
    auto error = vkCreateCommandPool(deviceForVk->device, &createInfo, nullptr, &pool);
    if (error)
        return agpu::command_allocator_ref();

    auto result = agpu::makeObject<AVkCommandAllocator> (device);
    auto allocator = result.as<AVkCommandAllocator> ();
    allocator->type = type;
    allocator->queueFamilyIndex = queueFamilyIndex;
    allocator->commandPool = pool;
    return result;
}

agpu_error AVkCommandAllocator::reset()
{
    auto error = vkResetCommandPool(deviceForVk->device, commandPool, 0);
    CONVERT_VULKAN_ERROR(error);
    return AGPU_OK;
}

} // End of namespace AgpuVulkan
