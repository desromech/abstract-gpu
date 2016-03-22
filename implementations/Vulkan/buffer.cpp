#include "buffer.hpp"

_agpu_buffer::_agpu_buffer(agpu_device *device)
    : device(device)
{
    uploadBuffer = VK_NULL_HANDLE;
    uploadBufferMemory = VK_NULL_HANDLE;
    gpuBuffer = VK_NULL_HANDLE;
    gpuBufferMemory = VK_NULL_HANDLE;
}

void _agpu_buffer::lostReferences()
{
    if (uploadBuffer)
        vkDestroyBuffer(device->device, uploadBuffer, nullptr);
    if (uploadBufferMemory)
        vkFreeMemory(device->device, uploadBufferMemory, nullptr);
    if (gpuBuffer)
        vkDestroyBuffer(device->device, gpuBuffer, nullptr);
    if (gpuBufferMemory)
        vkFreeMemory(device->device, gpuBufferMemory, nullptr);
}

agpu_buffer* _agpu_buffer::create(agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data)
{
    if (!description)
        return nullptr;

    // Try to determine whan kind of buffer is needed.
    bool streaming = description->usage == AGPU_STREAM;
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
        uploadMemoryTypeRequirements |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    
    VkBufferCreateInfo bufferDescription;
    memset(&bufferDescription, 0, sizeof(bufferDescription));
    bufferDescription.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferDescription.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferDescription.size = description->size;
    bufferDescription.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    switch (description->binding)
    {
    case AGPU_GENERIC_DATA_BUFFER:
        // For copying data
        break;
    case AGPU_ARRAY_BUFFER:
        bufferDescription.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        break;
    case AGPU_ELEMENT_ARRAY_BUFFER:
        bufferDescription.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        break;
    case AGPU_UNIFORM_BUFFER:
        bufferDescription.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        break;
    case AGPU_DRAW_INDIRECT_BUFFER:
        bufferDescription.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        break;
    }

    VkBuffer mainBuffer = VK_NULL_HANDLE;
    auto error = vkCreateBuffer(device->device, &bufferDescription, nullptr, &mainBuffer);
    if (error)
        return nullptr;

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device->device, mainBuffer, &memoryRequirements);

    VkBuffer uploadBuffer = VK_NULL_HANDLE;
    VkDeviceMemory uploadBufferMemory = VK_NULL_HANDLE;
    VkBuffer defaultBuffer = VK_NULL_HANDLE;
    VkDeviceMemory defaultBufferMemory = VK_NULL_HANDLE;

    if (needsUploadBuffer)
    {
        uploadBuffer = mainBuffer;
        mainBuffer = VK_NULL_HANDLE;

        VkMemoryAllocateInfo allocateInfo;
        memset(&allocateInfo, 0, sizeof(allocateInfo));
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = memoryRequirements.size;
        if (!device->findMemoryType(memoryRequirements.memoryTypeBits, uploadMemoryTypeRequirements, &allocateInfo.memoryTypeIndex))
            goto failure;

        error = vkAllocateMemory(device->device, &allocateInfo, nullptr, &uploadBufferMemory);
        if (error)
            goto failure;

        error = vkBindBufferMemory(device->device, uploadBuffer, uploadBufferMemory, 0);
        if (error)
            goto failure;

        // Upload the initial data.
        if (initial_data)
        {
            void *mappedBuffer;
            error = vkMapMemory(device->device, uploadBufferMemory, 0, description->size, 0, &mappedBuffer);
            if (error)
                goto failure;
            memcpy(mappedBuffer, initial_data, description->size);
            vkUnmapMemory(device->device, uploadBufferMemory);
        }
    }

    if (needsDefaultBuffer)
    {
        // Allocate the default buffer if needed.
        if (mainBuffer)
        {
            uploadBuffer = mainBuffer;
            mainBuffer = VK_NULL_HANDLE;
        }
        else
        {
            auto error = vkCreateBuffer(device->device, &bufferDescription, nullptr, &defaultBuffer);
            if (error)
                goto failure;
        }

        // Allocate the memory for the default buffer.
        VkMemoryAllocateInfo allocateInfo;
        memset(&allocateInfo, 0, sizeof(allocateInfo));
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = memoryRequirements.size;
        if (!device->findMemoryType(memoryRequirements.memoryTypeBits, defaultMemoryTypeRequirements, &allocateInfo.memoryTypeIndex))
            goto failure;

        error = vkAllocateMemory(device->device, &allocateInfo, nullptr, &defaultBufferMemory);
        if (error)
            goto failure;

        error = vkBindBufferMemory(device->device, defaultBuffer, defaultBufferMemory, 0);
        if (error)
            goto failure;
    }

    // Copy the initial data.
    if (initial_data && needsUploadBuffer && needsDefaultBuffer)
    {
        VkBufferCopy copyRegion;
        copyRegion.size = description->size;
        copyRegion.dstOffset = copyRegion.srcOffset = 0;
        if (!device->copyBuffer(uploadBuffer, defaultBuffer, 1, &copyRegion))
            goto failure;
    }

    // Destroy the upload buffer.
    if (!keepUploadBuffer && uploadBuffer != VK_NULL_HANDLE)
    {
        if (uploadBuffer)
            vkDestroyBuffer(device->device, uploadBuffer, nullptr);
        if (uploadBufferMemory)
            vkFreeMemory(device->device, uploadBufferMemory, nullptr);
        uploadBuffer = VK_NULL_HANDLE;
        uploadBufferMemory = VK_NULL_HANDLE;
    }

    auto result = new agpu_buffer(device);
    result->description = *description;
    result->gpuBuffer = defaultBuffer;
    result->gpuBufferMemory = defaultBufferMemory;
    result->uploadBuffer = uploadBuffer;
    result->uploadBufferMemory = uploadBufferMemory;
    result->memoryRequirements = memoryRequirements;
    return result;

failure:
    if(mainBuffer)
        vkDestroyBuffer(device->device, mainBuffer, nullptr);
    if(uploadBuffer)
        vkDestroyBuffer(device->device, uploadBuffer, nullptr);
    if(uploadBufferMemory)
        vkFreeMemory(device->device, uploadBufferMemory, nullptr);
    if (defaultBuffer)
        vkDestroyBuffer(device->device, defaultBuffer, nullptr);
    if (defaultBufferMemory)
        vkFreeMemory(device->device, defaultBufferMemory, nullptr);
    return nullptr;
}

agpu_pointer _agpu_buffer::map(agpu_mapping_access flags)
{
    return nullptr;
}

agpu_error _agpu_buffer::unmap()
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_buffer::getBufferDescription(agpu_buffer_description* description)
{
    CHECK_POINTER(description);
    *description = this->description;
    return AGPU_OK;
}

agpu_error _agpu_buffer::uploadBufferData(agpu_size offset, agpu_size size, agpu_pointer data)
{
    bool canBeSubUpdated = (description.mapping_flags & AGPU_MAP_DYNAMIC_STORAGE_BIT) != 0;
    if (!canBeSubUpdated)
        return AGPU_UNSUPPORTED;

    // Check the limits
    if (offset + size > description.size)
        return AGPU_ERROR;

    // Check the data.
    CHECK_POINTER(data);

    // Map the upload buffer.
    void *mappedBuffer;
    auto error = vkMapMemory(device->device, uploadBufferMemory, offset, size, 0, &mappedBuffer);
    CONVERT_VULKAN_ERROR(error);

    // Copy and unmap
    memcpy(mappedBuffer, data, size);
    vkUnmapMemory(device->device, uploadBufferMemory);

    // Transfer the data to the gpu buffer.
    if (!gpuBuffer)
        return AGPU_OK;

    VkBufferCopy region;
    region.srcOffset = region.dstOffset = offset;
    region.size = size;

    if (!device->copyBuffer(uploadBuffer, gpuBuffer, 1, &region))
        return AGPU_ERROR;

    return AGPU_OK;
}

agpu_error _agpu_buffer::readBufferData(agpu_size offset, agpu_size size, agpu_pointer data)
{
    return AGPU_UNIMPLEMENTED;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddBufferReference(agpu_buffer* buffer)
{
    CHECK_POINTER(buffer);
    return buffer->retain();
}

AGPU_EXPORT agpu_error agpuReleaseBuffer(agpu_buffer* buffer)
{
    CHECK_POINTER(buffer);
    return buffer->release();
}

AGPU_EXPORT agpu_pointer agpuMapBuffer(agpu_buffer* buffer, agpu_mapping_access flags)
{
    if (!buffer)
        return nullptr;
    return buffer->map(flags);
}

AGPU_EXPORT agpu_error agpuUnmapBuffer(agpu_buffer* buffer)
{
    CHECK_POINTER(buffer);
    return buffer->unmap();
}

AGPU_EXPORT agpu_error agpuGetBufferDescription(agpu_buffer* buffer, agpu_buffer_description* description)
{
    CHECK_POINTER(buffer);
    return buffer->getBufferDescription(description);
}

AGPU_EXPORT agpu_error agpuUploadBufferData(agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data)
{
    CHECK_POINTER(buffer);
    return buffer->uploadBufferData(offset, size,data);
}

AGPU_EXPORT agpu_error agpuReadBufferData(agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data)
{
    CHECK_POINTER(buffer);
    return buffer->readBufferData(offset, size, data);
}
