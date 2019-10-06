#include "buffer.hpp"

namespace AgpuVulkan
{

AVkBuffer::AVkBuffer(const agpu::device_ref &device)
    : weakDevice(device)
{
    uploadBuffer = VK_NULL_HANDLE;
    uploadBufferMemory = VK_NULL_HANDLE;
    gpuBuffer = VK_NULL_HANDLE;
    gpuBufferMemory = VK_NULL_HANDLE;
    mapCount = 0;
}

AVkBuffer::~AVkBuffer()
{
    auto device = weakDevice.lock();
    if(device)
    {
        if (uploadBuffer)
            vkDestroyBuffer(deviceForVk->device, uploadBuffer, nullptr);
        if (uploadBufferMemory)
            vkFreeMemory(deviceForVk->device, uploadBufferMemory, nullptr);
        if (gpuBuffer)
            vkDestroyBuffer(deviceForVk->device, gpuBuffer, nullptr);
        if (gpuBufferMemory)
            vkFreeMemory(deviceForVk->device, gpuBufferMemory, nullptr);
    }
}

agpu::buffer_ref AVkBuffer::create(const agpu::device_ref &device, agpu_buffer_description* description, agpu_pointer initial_data)
{
    if (!description)
        return agpu::buffer_ref();

    // Try to determine whan kind of buffer is needed.
    bool streaming = description->heap_type == AGPU_STREAM;
    bool canBeSubUpdated = (description->mapping_flags & AGPU_MAP_DYNAMIC_STORAGE_BIT) != 0;
    bool hasInitialData = initial_data != nullptr;
    bool canBeMapped = (description->mapping_flags & AGPU_MAP_READ_BIT) || (description->mapping_flags & AGPU_MAP_WRITE_BIT);
    bool canBeReaded = description->mapping_flags & AGPU_MAP_READ_BIT;
    bool hasCoherentMapping = (description->mapping_flags & AGPU_MAP_COHERENT_BIT) != 0;

    bool keepUploadBuffer = canBeMapped || canBeSubUpdated;
    bool needsUploadBuffer = keepUploadBuffer || hasInitialData;
    bool needsDefaultBuffer = !hasCoherentMapping && !streaming && !canBeReaded;

    VkFlags defaultMemoryTypeRequirements = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    VkFlags uploadMemoryTypeRequirements = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    if (hasCoherentMapping)
        uploadMemoryTypeRequirements |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;

    VkBufferCreateInfo bufferDescription;
    memset(&bufferDescription, 0, sizeof(bufferDescription));
    bufferDescription.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferDescription.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferDescription.size = description->size;
    bufferDescription.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    if(description->usage_modes & AGPU_ARRAY_BUFFER)
        bufferDescription.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if(description->usage_modes & AGPU_ELEMENT_ARRAY_BUFFER)
        bufferDescription.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if(description->usage_modes & AGPU_UNIFORM_TEXEL_BUFFER)
        bufferDescription.usage |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
    if(description->usage_modes & AGPU_STORAGE_TEXEL_BUFFER)
        bufferDescription.usage |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
    if(description->usage_modes & AGPU_DRAW_INDIRECT_BUFFER)
        bufferDescription.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    if(description->usage_modes & AGPU_COMPUTE_DISPATCH_INDIRECT_BUFFER)
        bufferDescription.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    if(description->usage_modes & AGPU_UNIFORM_BUFFER)
    {
        bufferDescription.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferDescription.size = alignedTo(size_t(bufferDescription.size), 256);
    }
    if(description->usage_modes & AGPU_STORAGE_BUFFER)
    {
        bufferDescription.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        bufferDescription.size = alignedTo(size_t(bufferDescription.size), 256);
    }

    VkBuffer mainBuffer = VK_NULL_HANDLE;
    auto error = vkCreateBuffer(deviceForVk->device, &bufferDescription, nullptr, &mainBuffer);
    if (error)
        return agpu::buffer_ref();

    VkBuffer uploadBuffer = VK_NULL_HANDLE;
    VkDeviceMemory uploadBufferMemory = VK_NULL_HANDLE;
    VkBuffer defaultBuffer = VK_NULL_HANDLE;
    VkDeviceMemory defaultBufferMemory = VK_NULL_HANDLE;

    if (needsUploadBuffer)
    {
        uploadBuffer = mainBuffer;
        mainBuffer = VK_NULL_HANDLE;

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(deviceForVk->device, uploadBuffer, &memoryRequirements);

        VkMemoryAllocateInfo allocateInfo;
        memset(&allocateInfo, 0, sizeof(allocateInfo));
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = memoryRequirements.size;
        if (!deviceForVk->findMemoryType(memoryRequirements.memoryTypeBits, uploadMemoryTypeRequirements, &allocateInfo.memoryTypeIndex))
            goto failure;

        error = vkAllocateMemory(deviceForVk->device, &allocateInfo, nullptr, &uploadBufferMemory);
        if (error)
            goto failure;

        error = vkBindBufferMemory(deviceForVk->device, uploadBuffer, uploadBufferMemory, 0);
        if (error)
            goto failure;

        // Upload the initial data.
        if (initial_data)
        {
            void *mappedBuffer;
            error = vkMapMemory(deviceForVk->device, uploadBufferMemory, 0, description->size, 0, &mappedBuffer);
            if (error)
                goto failure;
            memcpy(mappedBuffer, initial_data, description->size);
            vkUnmapMemory(deviceForVk->device, uploadBufferMemory);
        }
    }

    if (needsDefaultBuffer)
    {
        // Allocate the default buffer if needed.
        if (mainBuffer)
        {
            printf("Needs default buffer, had main\n");
            defaultBuffer = mainBuffer;
            mainBuffer = VK_NULL_HANDLE;
        }
        else
        {
            auto error = vkCreateBuffer(deviceForVk->device, &bufferDescription, nullptr, &defaultBuffer);
            if (error)
                goto failure;
        }

        // Default buffer requirements
        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(deviceForVk->device, defaultBuffer, &memoryRequirements);

        // Allocate the memory for the default buffer.
        VkMemoryAllocateInfo allocateInfo;
        memset(&allocateInfo, 0, sizeof(allocateInfo));
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = memoryRequirements.size;
        if (!deviceForVk->findMemoryType(memoryRequirements.memoryTypeBits, defaultMemoryTypeRequirements, &allocateInfo.memoryTypeIndex))
            goto failure;

        error = vkAllocateMemory(deviceForVk->device, &allocateInfo, nullptr, &defaultBufferMemory);
        if (error)
            goto failure;

        error = vkBindBufferMemory(deviceForVk->device, defaultBuffer, defaultBufferMemory, 0);
        if (error)
            goto failure;
    }

    // Copy the initial data.
    if (initial_data && needsUploadBuffer && needsDefaultBuffer)
    {
        VkBufferCopy copyRegion;
        copyRegion.size = description->size;
        copyRegion.dstOffset = copyRegion.srcOffset = 0;
        if (!deviceForVk->copyBuffer(uploadBuffer, defaultBuffer, 1, &copyRegion))
            goto failure;
    }

    // Destroy the upload buffer.
    if (!keepUploadBuffer && uploadBuffer != VK_NULL_HANDLE)
    {
        if (uploadBuffer)
            vkDestroyBuffer(deviceForVk->device, uploadBuffer, nullptr);
        if (uploadBufferMemory)
            vkFreeMemory(deviceForVk->device, uploadBufferMemory, nullptr);
        uploadBuffer = VK_NULL_HANDLE;
        uploadBufferMemory = VK_NULL_HANDLE;
    }

    {
        auto result = agpu::makeObject<AVkBuffer> (device);
        auto buffer = result.as<AVkBuffer> ();
        buffer->description = *description;
        buffer->gpuBuffer = defaultBuffer;
        buffer->gpuBufferMemory = defaultBufferMemory;
        buffer->uploadBuffer = uploadBuffer;
        buffer->uploadBufferMemory = uploadBufferMemory;
        return result;
    }

failure:
    if(mainBuffer)
        vkDestroyBuffer(deviceForVk->device, mainBuffer, nullptr);
    if(uploadBuffer)
        vkDestroyBuffer(deviceForVk->device, uploadBuffer, nullptr);
    if(uploadBufferMemory)
        vkFreeMemory(deviceForVk->device, uploadBufferMemory, nullptr);
    if (defaultBuffer)
        vkDestroyBuffer(deviceForVk->device, defaultBuffer, nullptr);
    if (defaultBufferMemory)
        vkFreeMemory(deviceForVk->device, defaultBufferMemory, nullptr);
    return agpu::buffer_ref();
}

agpu_pointer AVkBuffer::mapBuffer(agpu_mapping_access flags)
{
    // The upload buffer must be available.
    if (!uploadBufferMemory)
        return nullptr;

    auto device = weakDevice.lock();
    if(!device)
        return nullptr;

    std::unique_lock<std::mutex> l(mapMutex);

    if (mapCount == 0)
    {
        auto error = vkMapMemory(deviceForVk->device, uploadBufferMemory, 0, VK_WHOLE_SIZE, 0, &mappedPointer);
        if (error)
        {
            return nullptr;
        }
    }
    ++mapCount;
    return mappedPointer;
}

agpu_error AVkBuffer::unmapBuffer()
{
    // The upload buffer must be available.
    if (!uploadBufferMemory)
        return AGPU_INVALID_OPERATION;

    auto device = weakDevice.lock();
    if(!device)
        return AGPU_INVALID_OPERATION;

    std::unique_lock<std::mutex> l(mapMutex);
    --mapCount;
    if (mapCount == 0)
    {
        vkUnmapMemory(deviceForVk->device, uploadBufferMemory);
        mappedPointer = nullptr;
    }

    return AGPU_OK;
}

agpu_error AVkBuffer::getDescription(agpu_buffer_description* description)
{
    CHECK_POINTER(description);
    *description = this->description;
    return AGPU_OK;
}

agpu_error AVkBuffer::uploadBufferData(agpu_size offset, agpu_size size, agpu_pointer data)
{
    bool canBeSubUpdated = (description.mapping_flags & AGPU_MAP_DYNAMIC_STORAGE_BIT) != 0;
    if (!canBeSubUpdated)
        return AGPU_UNSUPPORTED;

    // Check the limits
    if (offset + size > description.size)
        return AGPU_ERROR;

    auto device = weakDevice.lock();
    if(!device)
        return AGPU_INVALID_OPERATION;

    // Check the data.
    CHECK_POINTER(data);

    // Map the upload buffer.
    void *mappedBuffer;
    auto error = vkMapMemory(deviceForVk->device, uploadBufferMemory, offset, size, 0, &mappedBuffer);
    CONVERT_VULKAN_ERROR(error);

    // Copy and unmap
    memcpy(mappedBuffer, data, size);
    vkUnmapMemory(deviceForVk->device, uploadBufferMemory);

    // Transfer the data to the gpu buffer.
    if (!gpuBuffer)
        return AGPU_OK;

    VkBufferCopy region;
    region.srcOffset = region.dstOffset = offset;
    region.size = size;

    if (!deviceForVk->copyBuffer(uploadBuffer, gpuBuffer, 1, &region))
        return AGPU_ERROR;

    return AGPU_OK;
}

agpu_error AVkBuffer::readBufferData(agpu_size offset, agpu_size size, agpu_pointer data)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AVkBuffer::flushWholeBuffer()
{
    if(!uploadBufferMemory)
        return AGPU_OK;

    auto device = weakDevice.lock();
    if(!device)
        return AGPU_INVALID_OPERATION;

    VkMappedMemoryRange range;
    memset(&range, 0, sizeof(range));
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = uploadBufferMemory;
    range.size = VK_WHOLE_SIZE;
    auto error = vkFlushMappedMemoryRanges(deviceForVk->device, 1, &range);
    CONVERT_VULKAN_ERROR(error);
    return AGPU_OK;
}

agpu_error AVkBuffer::invalidateWholeBuffer()
{
    if(!uploadBufferMemory)
        return AGPU_OK;

    auto device = weakDevice.lock();
    if(!device)
        return AGPU_INVALID_OPERATION;

    VkMappedMemoryRange range;
    memset(&range, 0, sizeof(range));
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = uploadBufferMemory;
    range.size = VK_WHOLE_SIZE;
    auto error = vkInvalidateMappedMemoryRanges(deviceForVk->device, 1, &range);
    CONVERT_VULKAN_ERROR(error);
    return AGPU_OK;
}

} // End of namespace AgpuVulkan
