#include "implicit_resource_command_list.hpp"
#include "device.hpp"
#include "command_queue.hpp"
#include "constants.hpp"

namespace AgpuVulkan
{

inline void addImageUsageSourceBarrierStages(agpu_texture_usage_mode_mask allowedUsages, agpu_texture_usage_mode_mask usage, VkPipelineStageFlags &stages)
{
    switch(int(usage))
    {
    case AGPU_TEXTURE_USAGE_COPY_DESTINATION:
        stages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
        break;
    case AGPU_TEXTURE_USAGE_COPY_SOURCE:
        stages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
        break;
    case AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT:
        stages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        break;
    case AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT:
    case AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT:
    case AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT:
        stages |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        break;
    case AGPU_TEXTURE_USAGE_SAMPLED:
        stages |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        break;
    case AGPU_TEXTURE_USAGE_PRESENT:
        break;
    case AGPU_TEXTURE_USAGE_STORAGE:
    default:
        stages |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        break;
    }
}

inline void addImageUsageDestinationBarrierStages(agpu_texture_usage_mode_mask allowedUsages, agpu_texture_usage_mode_mask usage, VkPipelineStageFlags &stages)
{
    switch(int(usage))
    {
    case AGPU_TEXTURE_USAGE_COPY_DESTINATION:
        stages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
        break;
    case AGPU_TEXTURE_USAGE_COPY_SOURCE:
        stages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
        break;
    case AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT:
        stages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        break;
    case AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT:
    case AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT:
    case AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT:
        stages |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        break;
    case AGPU_TEXTURE_USAGE_SAMPLED:
        stages |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        break;
    case AGPU_TEXTURE_USAGE_PRESENT:
        break;
    case AGPU_TEXTURE_USAGE_STORAGE:
    default:
        stages |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        break;
    }
}

VkImageMemoryBarrier barrierForImageUsageTransition(VkImage image, VkImageSubresourceRange range, agpu_texture_usage_mode_mask allowedUsages, agpu_texture_usage_mode_mask sourceUsage, agpu_texture_usage_mode_mask destUsage, VkPipelineStageFlags &srcStages, VkPipelineStageFlags &dstStages)
{
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcAccessMask = mapTextureUsageModeToAccessFlags(allowedUsages, sourceUsage);
    barrier.dstAccessMask = mapTextureUsageModeToAccessFlags(allowedUsages, destUsage);
    barrier.oldLayout = mapTextureUsageModeToLayout(allowedUsages, sourceUsage);
    barrier.newLayout = mapTextureUsageModeToLayout(allowedUsages, destUsage);
    barrier.subresourceRange = range;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    srcStages |= mapTextureUsageModeToPipelineSourceStages(allowedUsages, sourceUsage);
    dstStages |= mapTextureUsageModeToPipelineDestinationStages(allowedUsages, sourceUsage);
    return barrier;
}

// The generic setup command list.
AVkImplicitResourceSetupCommandList::AVkImplicitResourceSetupCommandList(AVkDevice &cdevice)
    : device(cdevice)
{
    commandPool = VK_NULL_HANDLE;
    commandBuffer = nullptr;
}

AVkImplicitResourceSetupCommandList::~AVkImplicitResourceSetupCommandList()
{
}

bool AVkImplicitResourceSetupCommandList::createCommandBuffer()
{
    VkCommandPoolCreateInfo poolCreate = {};
    poolCreate.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreate.queueFamilyIndex = commandQueue.as<AVkCommandQueue> ()->queueFamilyIndex;
    poolCreate.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

    auto error = vkCreateCommandPool(device.device, &poolCreate, nullptr, &commandPool);
    if (error)
        return false;

    VkCommandBufferAllocateInfo commandInfo = {};
    commandInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandInfo.commandPool = commandPool;
    commandInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandInfo.commandBufferCount = 1;

    error = vkAllocateCommandBuffers(device.device, &commandInfo, &commandBuffer);
    if (error)
    {
        vkDestroyCommandPool(device.device, commandPool, nullptr);
        commandPool = VK_NULL_HANDLE;
        return false;
    }

    VkCommandBufferInheritanceInfo inheritance = {};
    inheritance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pInheritanceInfo = &inheritance;

    error = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (error)
    {
        vkFreeCommandBuffers(device.device, commandPool, 1, &commandBuffer);
        vkDestroyCommandPool(device.device, commandPool, nullptr);
        commandPool = VK_NULL_HANDLE;
        commandBuffer = VK_NULL_HANDLE;
        return false;
    }

    return true;
}

bool AVkImplicitResourceSetupCommandList::setupCommandBuffer()
{
    if(!commandBuffer)
        return createCommandBuffer();

    auto error = vkResetCommandPool(device.device, commandPool, 0);
    if (error)
        abort();

    VkCommandBufferInheritanceInfo inheritance = {};
    inheritance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pInheritanceInfo = &inheritance;

    error = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (error)
    {
        vkFreeCommandBuffers(device.device, commandPool, 1, &commandBuffer);
        vkDestroyCommandPool(device.device, commandPool, nullptr);
        commandPool = VK_NULL_HANDLE;
        commandBuffer = VK_NULL_HANDLE;
        return false;
    }

    return true;
}

bool AVkImplicitResourceSetupCommandList::submitCommandBuffer()
{
    auto error = vkEndCommandBuffer(commandBuffer);
    if (error)
        abort();

    VkSubmitInfo submitInfo;
    memset(&submitInfo, 0, sizeof(submitInfo));
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    error = vkQueueSubmit(commandQueue.as<AVkCommandQueue> ()->queue, 1, &submitInfo, VK_NULL_HANDLE);
    if (error)
        abort();

    error = vkQueueWaitIdle(commandQueue.as<AVkCommandQueue> ()->queue);
    if (error)
        abort();

    return true;
}

bool AVkImplicitResourceSetupCommandList::clearImageWithColor(VkImage image, VkImageSubresourceRange range, agpu_texture_usage_mode_mask allAllowedUsages, agpu_texture_usage_mode_mask usageMode, VkClearColorValue *clearValue)
{
    // Clear the image
    vkCmdClearColorImage(commandBuffer, image, mapTextureUsageModeToLayout(allAllowedUsages, usageMode), clearValue, 1, &range);

    return true;
}

bool AVkImplicitResourceSetupCommandList::clearImageWithDepthStencil(VkImage image, VkImageSubresourceRange range, agpu_texture_usage_mode_mask allAllowedUsages, agpu_texture_usage_mode_mask usageMode, VkClearDepthStencilValue *clearValue)
{
    // Clear the image
    vkCmdClearDepthStencilImage(commandBuffer, image, mapTextureUsageModeToLayout(allAllowedUsages, usageMode), clearValue, 1, &range);

    return true;
}

bool AVkImplicitResourceSetupCommandList::transitionImageUsageMode(VkImage image, agpu_texture_usage_mode_mask allowedUsages, agpu_texture_usage_mode_mask sourceUsage, agpu_texture_usage_mode_mask destUsage, VkImageSubresourceRange range)
{
    // If the usage mode is the same, we do not need the barrier.
    if(sourceUsage == destUsage)
        return true;

    VkPipelineStageFlags srcStages = 0;
    VkPipelineStageFlags destStages = 0;
    auto barrier = barrierForImageUsageTransition(image, range, allowedUsages, sourceUsage, destUsage, srcStages, destStages);
    vkCmdPipelineBarrier(commandBuffer, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    return true;
}

VkResult AVkImplicitResourceSetupCommandList::destroyStagingBuffer(VkBuffer bufferHandle, VmaAllocation allocationHandle)
{
    vmaDestroyBuffer(device.memoryAllocator, bufferHandle, allocationHandle);
    return VK_SUCCESS;
}

VkResult AVkImplicitResourceSetupCommandList::createStagingBuffer(agpu_memory_heap_type heapType, size_t allocationSize, VkBuffer *bufferHandle, VmaAllocation *allocationHandle, void **mappedPointer)
{
    VkBufferCreateInfo bufferDescription = {};
    bufferDescription.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferDescription.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferDescription.size = allocationSize;
    bufferDescription.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    VmaAllocationCreateInfo allocationInfo = {};
    allocationInfo.usage = mapHeapType(heapType);
    allocationInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocationInfo.requiredFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    auto error = vmaCreateBuffer(device.memoryAllocator, &bufferDescription, &allocationInfo, bufferHandle, allocationHandle, nullptr);
    if(error) return error;

    return vmaMapMemory(device.memoryAllocator, *allocationHandle, mappedPointer);
}

} // End of namespace AgpuVulkan
