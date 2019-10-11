#include "buffer.hpp"

namespace AgpuVulkan
{

AVkBuffer::AVkBuffer(const agpu::device_ref &device)
    : weakDevice(device)
{
    handle = VK_NULL_HANDLE;
    allocation = VK_NULL_HANDLE;
    mapCount = 0;
}

AVkBuffer::~AVkBuffer()
{
    auto device = weakDevice.lock();
    if(device)
    {
        if (handle)
            vmaDestroyBuffer(deviceForVk->memoryAllocator, handle, allocation);
    }
}

agpu::buffer_ref AVkBuffer::create(const agpu::device_ref &device, agpu_buffer_description* originalDescription, agpu_pointer initial_data)
{
    if (!originalDescription)
        return agpu::buffer_ref();

    auto description = *originalDescription;

    VkBufferCreateInfo bufferDescription = {};
    bufferDescription.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferDescription.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferDescription.size = description.size;
    bufferDescription.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    if(description.usage_modes & AGPU_ARRAY_BUFFER)
        bufferDescription.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if(description.usage_modes & AGPU_ELEMENT_ARRAY_BUFFER)
        bufferDescription.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if(description.usage_modes & AGPU_UNIFORM_TEXEL_BUFFER)
        bufferDescription.usage |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
    if(description.usage_modes & AGPU_STORAGE_TEXEL_BUFFER)
        bufferDescription.usage |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
    if(description.usage_modes & AGPU_DRAW_INDIRECT_BUFFER)
        bufferDescription.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    if(description.usage_modes & AGPU_COMPUTE_DISPATCH_INDIRECT_BUFFER)
        bufferDescription.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    if(description.usage_modes & AGPU_UNIFORM_BUFFER)
    {
        bufferDescription.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferDescription.size = alignedTo(size_t(bufferDescription.size), 256);
    }
    if(description.usage_modes & AGPU_STORAGE_BUFFER)
    {
        bufferDescription.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        bufferDescription.size = alignedTo(size_t(bufferDescription.size), 256);
    }

    VmaAllocationCreateInfo allocationInfo = {};
    allocationInfo.usage = mapHeapType(description.heap_type);
    if(description.mapping_flags & AGPU_MAP_DYNAMIC_STORAGE_BIT)
    {
        if(description.heap_type == AGPU_MEMORY_HEAP_TYPE_HOST_TO_DEVICE)
            description.mapping_flags |= AGPU_MAP_WRITE_BIT;
		else if (description.heap_type == AGPU_MEMORY_HEAP_TYPE_HOST)
			description.mapping_flags |= AGPU_MAP_READ_BIT | AGPU_MAP_WRITE_BIT;
        else if(description.heap_type == AGPU_MEMORY_HEAP_TYPE_DEVICE_TO_HOST)
            description.mapping_flags |= AGPU_MAP_READ_BIT;
    }

    if((description.mapping_flags & AGPU_MAP_READ_BIT) || (description.mapping_flags & AGPU_MAP_WRITE_BIT))
        allocationInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    // Is coherent mapping required?
    if((description.mapping_flags & AGPU_MAP_COHERENT_BIT) != 0)
        allocationInfo.requiredFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkBuffer bufferHandle;
    VmaAllocation allocationHandle;
    auto error = vmaCreateBuffer(deviceForVk->memoryAllocator, &bufferDescription, &allocationInfo, &bufferHandle, &allocationHandle, nullptr);
    if(error) return agpu::buffer_ref();

    // Create the buffer object.
    auto result = agpu::makeObject<AVkBuffer> (device);
    auto avkBuffer = result.as<AVkBuffer> ();
    avkBuffer->handle = bufferHandle;
    avkBuffer->allocation = allocationHandle;
    avkBuffer->description = description;

    // Upload the buffer initial data.
    if(initial_data)
    {
        // We need the at least temporarily the AGPU_MAP_DYNAMIC_STORAGE_BIT mapping here.
        avkBuffer->description.mapping_flags |= AGPU_MAP_DYNAMIC_STORAGE_BIT;
        result->uploadBufferData(0, description.size, initial_data);

        if((description.mapping_flags & AGPU_MAP_DYNAMIC_STORAGE_BIT) == 0)
            avkBuffer->description.mapping_flags &= ~AGPU_MAP_DYNAMIC_STORAGE_BIT;
    }

    return result;
}

agpu_pointer AVkBuffer::mapBuffer(agpu_mapping_access flags)
{
    if (!allocation)
        return nullptr;

    auto device = weakDevice.lock();
    if(!device)
        return nullptr;

    std::unique_lock<Spinlock> l(mappingLock);
    if (mapCount == 0)
    {
        auto error = vmaMapMemory(deviceForVk->memoryAllocator, allocation, &mappedPointer);
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
    if (!allocation)
        return AGPU_INVALID_OPERATION;

    auto device = weakDevice.lock();
    if(!device)
        return AGPU_INVALID_OPERATION;

    std::unique_lock<Spinlock> l(mappingLock);
    --mapCount;
    if (mapCount == 0)
    {
        vmaUnmapMemory(deviceForVk->memoryAllocator, allocation);
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
    bool canBeSubUpdated = (description.mapping_flags & (AGPU_MAP_DYNAMIC_STORAGE_BIT | AGPU_MAP_WRITE_BIT)) != 0;
    if (!canBeSubUpdated)
        return AGPU_UNSUPPORTED;

    // Check the limits
    if (offset + size > description.size)
        return AGPU_ERROR;

    // Do we have data to upload?
    if(size == 0)
        return AGPU_OK;

    // Check the data.
    CHECK_POINTER(data);

    // If we can map the buffer, then just perform a memcpy onto it.
    if(description.mapping_flags & AGPU_MAP_WRITE_BIT)
    {
        auto uploadPointer = reinterpret_cast<uint8_t*> (mapBuffer(AGPU_WRITE_ONLY));
        if(!uploadPointer)
            return AGPU_ERROR;

		uploadPointer += offset;
        memcpy(uploadPointer, data, size);
        return unmapBuffer();
    }

    auto device = weakDevice.lock();
    if(!device)
        return AGPU_INVALID_OPERATION;

    bool uploadResult = false;
    deviceForVk->withUploadCommandListDo(size, 1, [&](AVkImplicitResourceUploadCommandList &uploadList) {
        // Do we need to stream the buffer upload?.
        if(size <= uploadList.currentStagingBufferSize)
        {
            memcpy(uploadList.currentStagingBufferPointer, data, size);
            uploadResult = uploadList.setupCommandBuffer()
                && uploadList.uploadBufferData(handle, offset, size)
                && uploadList.submitCommandBuffer();
        }
        else
        {
            abort();
        }
    });

    return uploadResult ? AGPU_OK : AGPU_ERROR;
}

agpu_error AVkBuffer::readBufferData(agpu_size offset, agpu_size size, agpu_pointer data)
{
    bool canBeSubReaded = (description.mapping_flags & (AGPU_MAP_DYNAMIC_STORAGE_BIT | AGPU_MAP_READ_BIT)) != 0;
    if (!canBeSubReaded)
        return AGPU_UNSUPPORTED;

    // Check the limits
    if (offset + size > description.size)
        return AGPU_ERROR;

    // Do we have data to upload?
    if(size == 0)
        return AGPU_OK;

    // If we can map the buffer, then just perform a memcpy from it.
    if(description.mapping_flags & AGPU_MAP_READ_BIT)
    {
        auto readbackPointer = reinterpret_cast<uint8_t*> (mapBuffer(AGPU_READ_ONLY)) + offset;
        if(!readbackPointer)
            return AGPU_ERROR;

        memcpy(readbackPointer, data, size);
        return unmapBuffer();
    }

    auto device = weakDevice.lock();
    if(!device)
        return AGPU_INVALID_OPERATION;

    bool readbackResult = false;
    deviceForVk->withReadbackCommandListDo(size, 1, [&](AVkImplicitResourceReadbackCommandList &readbackList) {
        // Do we need to stream the buffer upload?.
        if(size <= readbackList.currentStagingBufferSize)
        {
            readbackResult = readbackList.setupCommandBuffer()
                && readbackList.readbackBufferData(handle, offset, size)
                && readbackList.submitCommandBuffer();
            memcpy(data, readbackList.currentStagingBufferPointer, size);
        }
        else
        {
            abort();
        }
    });

    return readbackResult ? AGPU_OK : AGPU_ERROR;
}

agpu_error AVkBuffer::flushWholeBuffer()
{
    if(!allocation)
        return AGPU_OK;

    auto device = weakDevice.lock();
    if(!device)
        return AGPU_INVALID_OPERATION;

    vmaFlushAllocation(deviceForVk->memoryAllocator, allocation, 0, description.size);
    return AGPU_OK;
}

agpu_error AVkBuffer::invalidateWholeBuffer()
{
    if(!allocation)
        return AGPU_OK;

    auto device = weakDevice.lock();
    if(!device)
        return AGPU_INVALID_OPERATION;

    vmaInvalidateAllocation(deviceForVk->memoryAllocator, allocation, 0, description.size);
    return AGPU_OK;
}

} // End of namespace AgpuVulkan
