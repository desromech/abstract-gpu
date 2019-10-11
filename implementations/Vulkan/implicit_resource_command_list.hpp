#ifndef AGPU_VULKAN_IMPLICIT_RESOURCE_COMMAND_LIST_HPP
#define AGPU_VULKAN_IMPLICIT_RESOURCE_COMMAND_LIST_HPP

#include "common.hpp"
#include "include_vulkan.h"
#include "vk_mem_alloc.h"
#include "../Common/utility.hpp"
#include <memory>
#include <mutex>
#include <algorithm>

namespace AgpuVulkan
{
using AgpuCommon::alignedTo;
using AgpuCommon::nextPowerOfTwo;
class AVkDevice;

/*VkImageMemoryBarrier barrierForImageLayoutTransition(VkImage image, VkImageSubresourceRange range, VkImageAspectFlags aspect, VkImageLayout sourceLayout, VkImageLayout destLayout, VkAccessFlags srcAccessMask, VkPipelineStageFlags &srcStages, VkPipelineStageFlags &dstStages);*/

VkImageMemoryBarrier barrierForImageUsageTransition(VkImage image, VkImageSubresourceRange range, agpu_texture_usage_mode_mask allowedUsages, agpu_texture_usage_mode_mask sourceUsage, agpu_texture_usage_mode_mask destUsage, VkPipelineStageFlags &srcStages, VkPipelineStageFlags &dstStages);

class AVkImplicitResourceSetupCommandList
{
public:
    AVkImplicitResourceSetupCommandList(AVkDevice &cdevice);
    ~AVkImplicitResourceSetupCommandList();

    bool clearImageWithColor(VkImage image, VkImageSubresourceRange range, agpu_texture_usage_mode_mask allAllowedUsages, agpu_texture_usage_mode_mask usageMode, VkClearColorValue *clearValue);
    bool clearImageWithDepthStencil(VkImage image, VkImageSubresourceRange range, agpu_texture_usage_mode_mask allAllowedUsages, agpu_texture_usage_mode_mask usageMode, VkClearDepthStencilValue *clearValue);

    bool setupCommandBuffer();
    bool submitCommandBuffer();
    bool transitionImageUsageMode(VkImage image, agpu_texture_usage_mode_mask allowedUsages, agpu_texture_usage_mode_mask sourceUsage, agpu_texture_usage_mode_mask destUsage, VkImageSubresourceRange range);

    AVkDevice &device;

    std::mutex mutex;
    agpu::command_queue_ref commandQueue;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

private:
    bool createCommandBuffer();

protected:
    VkResult destroyStagingBuffer(VkBuffer bufferHandle, VmaAllocation allocationHandle);
    VkResult createStagingBuffer(agpu_memory_heap_type heapType, size_t allocationSize, VkBuffer *bufferHandle, VmaAllocation *allocationHandle, void **mappedPointer);
};

template<agpu_memory_heap_type HT, size_t IC>
class AVkImplicitResourceStagingCommandList : public AVkImplicitResourceSetupCommandList
{
public:
    static constexpr agpu_memory_heap_type MemoryHeapType = HT;
    static constexpr size_t InitialCapacity = IC;

    AVkImplicitResourceStagingCommandList(AVkDevice &cdevice)
        : AVkImplicitResourceSetupCommandList(cdevice),
        currentStagingBufferSize(0),
        currentStagingBufferPointer(nullptr),
        stagingBufferCapacity(0),
        stagingBufferBasePointer(nullptr),
        bufferHandle(VK_NULL_HANDLE),
        allocationHandle(VK_NULL_HANDLE)
    {
    }

    ~AVkImplicitResourceStagingCommandList()
    {
    }

    void ensureValidCPUStagingBuffer(size_t requiredSize, size_t requiredAlignment)
    {
        if(stagingBufferCapacity < requiredSize)
        {
            stagingBufferCapacity = nextPowerOfTwo(requiredSize);
            stagingBufferCapacity = std::max(stagingBufferCapacity, size_t(InitialCapacity));
            if(bufferHandle)
            {
                destroyStagingBuffer(bufferHandle, allocationHandle);
                bufferHandle = VK_NULL_HANDLE;
                allocationHandle = VK_NULL_HANDLE;
                stagingBufferBasePointer = nullptr;
            }

            createStagingBuffer(MemoryHeapType, stagingBufferCapacity, &bufferHandle, &allocationHandle, reinterpret_cast<void**> (&stagingBufferBasePointer));
        }

        currentStagingBufferSize = requiredSize;
        currentStagingBufferPointer = stagingBufferBasePointer;
    }

    bool uploadBufferData(VkBuffer destBuffer, size_t offset, size_t size)
    {
        VkBufferCopy region;
        region.srcOffset = 0;
        region.dstOffset = offset;
        region.size = size;

        vkCmdCopyBuffer(commandBuffer, bufferHandle, destBuffer, 1, &region);
        return true;
    }

    bool readbackBufferData(VkBuffer sourceBuffer, size_t offset, size_t size)
    {
        VkBufferCopy region;
        region.srcOffset = offset;
        region.dstOffset = 0;
        region.size = size;

        vkCmdCopyBuffer(commandBuffer, sourceBuffer, bufferHandle, 1, &region);
        return true;
    }

    bool uploadBufferDataToImage(VkImage destImage, VkBufferImageCopy copyRegion)
    {
        copyRegion.bufferOffset = 0;
        vkCmdCopyBufferToImage(commandBuffer, bufferHandle, destImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
        return true;
    }

    bool readbackBufferDataToImage(VkImage sourceImage, VkBufferImageCopy copyRegion)
    {
        copyRegion.bufferOffset = 0;
        vkCmdCopyImageToBuffer(commandBuffer, sourceImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, bufferHandle, 1, &copyRegion);
        return true;
    }

    size_t currentStagingBufferSize;
    void *currentStagingBufferPointer;

protected:

    size_t stagingBufferCapacity;
    uint8_t *stagingBufferBasePointer;

    VkBuffer bufferHandle;
    VmaAllocation allocationHandle;
};

typedef AVkImplicitResourceStagingCommandList<AGPU_MEMORY_HEAP_TYPE_HOST_TO_DEVICE, 2048*2048*4> AVkImplicitResourceUploadCommandList;
typedef AVkImplicitResourceStagingCommandList<AGPU_MEMORY_HEAP_TYPE_DEVICE_TO_HOST, 2048*2048*4> AVkImplicitResourceReadbackCommandList;

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_RESOURCE_SETUP_QUEUE_HPP
