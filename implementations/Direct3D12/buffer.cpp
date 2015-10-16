#include "buffer.hpp"
#include "common_commands.hpp"

DXGI_FORMAT mapIndexFormat(size_t indexSize)
{
    switch (indexSize)
    {
    case 1: return DXGI_FORMAT_R8_UINT;
    case 2: return DXGI_FORMAT_R16_UINT;
    case 4: return DXGI_FORMAT_R32_UINT;
    default:
        abort();
    }
}

_agpu_buffer::_agpu_buffer()
{

}

void _agpu_buffer::lostReferences()
{

}

agpu_buffer* _agpu_buffer::create(agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data)
{
    if (!device || !description)
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

    ComPtr<ID3D12Resource> uploadResource;
    ComPtr<ID3D12Resource> gpuResource;

    // The resource description.
    D3D12_RESOURCE_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment = 0;
    desc.Width = description->size;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12_RANGE nullRange;
    memset(&nullRange, 0, sizeof(nullRange));

    // Create the upload buffer.
    if (needsUploadBuffer)
    {
        D3D12_HEAP_PROPERTIES heapProperties;
        memset(&heapProperties, 0, sizeof(heapProperties));
        heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        auto initialState = D3D12_RESOURCE_STATE_GENERIC_READ;

        if (FAILED(device->d3dDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc, initialState, nullptr, IID_PPV_ARGS(&uploadResource))))
            return nullptr;

        // Upload the initial data.
        if (initial_data)
        {
            void *bufferBegin;
            if (FAILED(uploadResource->Map(0, &nullRange, &bufferBegin)))
                return nullptr;

            memcpy(bufferBegin, initial_data, description->size);
            uploadResource->Unmap(0, nullptr);
        }
    }

    if (needsDefaultBuffer)
    {
        D3D12_HEAP_PROPERTIES heapProperties;
        memset(&heapProperties, 0, sizeof(heapProperties));
        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        auto initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
        if (hasInitialData)
            initialState = D3D12_RESOURCE_STATE_COPY_DEST;

        if (FAILED(device->d3dDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&gpuResource))))
            return nullptr;

        // Copy the initial data.
        if (hasInitialData)
        {
            auto res = device->withTransferQueueAndCommandList([&](const ComPtr<ID3D12CommandQueue> &queue, const ComPtr<ID3D12GraphicsCommandList> &list) -> agpu_error {
                list->CopyResource(gpuResource.Get(), uploadResource.Get());
                auto barrier = resourceTransitionBarrier(gpuResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
                list->ResourceBarrier(1, &barrier);
                list->Close();

                ID3D12CommandList *ptr = list.Get();
                queue->ExecuteCommandLists(1, &ptr);
                return device->waitForMemoryTransfer();
            });

            if (res < 0)
                return nullptr;
        }
    }

    // Create the buffer object
    auto buffer = new agpu_buffer();
    buffer->device = device;
    buffer->description = *description;
    buffer->gpuResource = gpuResource;
    if(keepUploadBuffer)
        buffer->uploadResource = uploadResource;
    
    // Create the buffer view.
    if(buffer->createView() < 0)
    {
        buffer->release();
        return nullptr;
    }

    return buffer;
}

ID3D12Resource *_agpu_buffer::getActualGpuBuffer()
{
    if (gpuResource)
        return gpuResource.Get();
    return uploadResource.Get();
}

agpu_error _agpu_buffer::createView()
{
    auto gpuBuffer = getActualGpuBuffer();

    switch (description.binding)
    {
    case AGPU_ARRAY_BUFFER:
        view.vertexBuffer.BufferLocation = gpuBuffer->GetGPUVirtualAddress();
        view.vertexBuffer.SizeInBytes = description.size;
        view.vertexBuffer.StrideInBytes = description.stride;
        return AGPU_OK;
    case AGPU_ELEMENT_ARRAY_BUFFER:
        view.indexBuffer.BufferLocation = gpuBuffer->GetGPUVirtualAddress();
        view.indexBuffer.SizeInBytes = description.size;
        view.indexBuffer.Format = mapIndexFormat(description.stride);
        return AGPU_OK;
    case AGPU_UNIFORM_BUFFER:
        view.constantBuffer.BufferLocation = gpuBuffer->GetGPUVirtualAddress();
        view.constantBuffer.SizeInBytes = (description.size + 255) & (~255);
        return AGPU_OK;
    case AGPU_DRAW_INDIRECT_BUFFER:
    default:
        return AGPU_UNSUPPORTED;
    }
}

agpu_pointer _agpu_buffer::mapBuffer(agpu_mapping_access flags)
{
    bool canBeReaded = (description.mapping_flags & AGPU_MAP_READ_BIT) != 0;
    bool canBeWritten = (description.mapping_flags & AGPU_MAP_WRITE_BIT) != 0;
    if (!canBeReaded && !canBeWritten)
        return nullptr;

    // Check the flags more properly.
    D3D12_RANGE readRange;
    memset(&readRange, 0, sizeof(readRange));
    if (FAILED(uploadResource->Map(0, canBeReaded ? nullptr : &readRange, (void**)&mappedPointer)))
        return nullptr;

    return mappedPointer;
}

agpu_error _agpu_buffer::unmapBuffer()
{
    if (!mappedPointer)
        return AGPU_OK;

    uploadResource->Unmap(0, nullptr);
    mappedPointer = nullptr;

    // TODO: When the mapping is coherent, send the data to the gpu heap.
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
    D3D12_RANGE readRange;
    memset(&readRange, 0, sizeof(readRange));
    uint8_t *dstPtr;
    ERROR_IF_FAILED(uploadResource->Map(0, &readRange, reinterpret_cast<void**> (&dstPtr)));

    // Copy and unmap.
    memcpy(dstPtr + offset, data, size);

    D3D12_RANGE writtenRange;
    writtenRange.Begin = offset;
    writtenRange.End = offset + size;
    uploadResource->Unmap(0, &writtenRange);

    // Transfer the data to the gpu buffer.
    if (!gpuResource)
        return AGPU_OK;

    return device->withTransferQueueAndCommandList([&](const ComPtr<ID3D12CommandQueue> &queue, const ComPtr<ID3D12GraphicsCommandList> &list) -> agpu_error {
        auto barrier = resourceTransitionBarrier(gpuResource, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);
        list->ResourceBarrier(1, &barrier);
        list->CopyBufferRegion(gpuResource.Get(), offset, uploadResource.Get(), offset, size);
        barrier = resourceTransitionBarrier(gpuResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
        list->ResourceBarrier(1, &barrier);
        list->Close();

        ID3D12CommandList *ptr = list.Get();
        queue->ExecuteCommandLists(1, &ptr);
        return device->waitForMemoryTransfer();
    });
}

agpu_error _agpu_buffer::readBufferData(agpu_size offset, agpu_size size, agpu_pointer data)
{
    bool canBeSubUpdated = (description.mapping_flags & AGPU_MAP_DYNAMIC_STORAGE_BIT) != 0;
    if (!canBeSubUpdated)
        return AGPU_UNSUPPORTED;

    // Check the limits
    if (offset + size > description.size)
        return AGPU_ERROR;

    // Check the data.
    CHECK_POINTER(data);

    // Map the read buffer.
    uint8_t *srcPtr;
    ERROR_IF_FAILED(uploadResource->Map(0, nullptr, reinterpret_cast<void**> (&srcPtr)));

    // Copy and unmap.
    memcpy(data, srcPtr + offset, size);
    uploadResource->Unmap(0, nullptr);

    return AGPU_OK;
}

// Exported C interface
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
    return buffer->mapBuffer(flags);
}

AGPU_EXPORT agpu_error agpuUnmapBuffer(agpu_buffer* buffer)
{
    CHECK_POINTER(buffer);
    return buffer->unmapBuffer();
}

AGPU_EXPORT agpu_error agpuGetBufferDescription(agpu_buffer* buffer, agpu_buffer_description* description)
{
    CHECK_POINTER(buffer);
    *description = buffer->description;
    return AGPU_OK;
}

AGPU_EXPORT agpu_error agpuUploadBufferData(agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data)
{
    CHECK_POINTER(buffer);
    return buffer->uploadBufferData(offset, size, data);
}

AGPU_EXPORT agpu_error agpuReadBufferData(agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data)
{
    CHECK_POINTER(buffer);
    return buffer->readBufferData(offset, size, data);
}
