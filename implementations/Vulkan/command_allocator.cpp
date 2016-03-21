#include "command_allocator.hpp"
#include "command_queue.hpp"

_agpu_command_allocator::_agpu_command_allocator(agpu_device *device)
    : device(device)
{
    commandPool = nullptr;
}

void _agpu_command_allocator::lostReferences()
{
    vkDestroyCommandPool(device->device, commandPool, nullptr);
}

agpu_command_allocator* _agpu_command_allocator::create(agpu_device* device, agpu_command_list_type type, agpu_command_queue *commandQueue)
{
    if (!commandQueue)
        return nullptr;

    VkCommandPoolCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(createInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = commandQueue->queueFamilyIndex;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkCommandPool pool;
    auto error = vkCreateCommandPool(device->device, &createInfo, nullptr, &pool);
    if (error)
        return nullptr;

    std::unique_ptr<agpu_command_allocator> result(new agpu_command_allocator(device));
    result->type = type;
    result->queueFamilyIndex = commandQueue->queueFamilyIndex;
    result->commandPool = pool;
    return result.release();
}

agpu_error _agpu_command_allocator::reset()
{
    auto error = vkResetCommandPool(device->device, commandPool, 0);
    CONVERT_VULKAN_ERROR(error);
    return AGPU_OK;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddCommandAllocatorReference(agpu_command_allocator* command_allocator)
{
    CHECK_POINTER(command_allocator);
    return command_allocator->retain();
}

AGPU_EXPORT agpu_error agpuReleaseCommandAllocator(agpu_command_allocator* command_allocator)
{
    CHECK_POINTER(command_allocator);
    return command_allocator->release();
}

AGPU_EXPORT agpu_error agpuResetCommandAllocator(agpu_command_allocator* command_allocator)
{
    CHECK_POINTER(command_allocator);
    return command_allocator->reset();
}