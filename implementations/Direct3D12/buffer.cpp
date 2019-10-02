#include "buffer.hpp"
#include "common_commands.hpp"

namespace AgpuD3D12
{

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

ADXBuffer::ADXBuffer(const agpu::device_ref &device)
    : device(device)
{

}

ADXBuffer::~ADXBuffer()
{

}

agpu::buffer_ref ADXBuffer::create(const agpu::device_ref &device, agpu_buffer_description* description, agpu_pointer initial_data)
{
    if (!device || !description)
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

	// Uniform buffers require an alignment of 256 bytes.
	if ((description->binding & AGPU_UNIFORM_BUFFER) != 0)
	{
		desc.Width = (desc.Width + 255) & (-256);
	}

	D3D12_RANGE nullRange = {};

    // Create the upload buffer.
    if (needsUploadBuffer)
    {
        D3D12_HEAP_PROPERTIES heapProperties;
        memset(&heapProperties, 0, sizeof(heapProperties));
        heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        auto initialState = D3D12_RESOURCE_STATE_GENERIC_READ;

        if (FAILED(deviceForDX->d3dDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc, initialState, nullptr, IID_PPV_ARGS(&uploadResource))))
            return agpu::buffer_ref();

        // Upload the initial data.
        if (initial_data)
        {
            void *bufferBegin;
            if (FAILED(uploadResource->Map(0, &nullRange, &bufferBegin)))
                return agpu::buffer_ref();

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

        if (FAILED(deviceForDX->d3dDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc, initialState, nullptr, IID_PPV_ARGS(&gpuResource))))
            return agpu::buffer_ref();

        // Copy the initial data.
        if (hasInitialData)
        {
            auto res = deviceForDX->withTransferQueueAndCommandList([&](const ComPtr<ID3D12CommandQueue> &queue, const ComPtr<ID3D12GraphicsCommandList> &list) -> agpu_error {
                list->CopyResource(gpuResource.Get(), uploadResource.Get());
                auto barrier = resourceTransitionBarrier(gpuResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
                list->ResourceBarrier(1, &barrier);
                list->Close();

                ID3D12CommandList *ptr = list.Get();
                queue->ExecuteCommandLists(1, &ptr);
                return deviceForDX->waitForMemoryTransfer();
            });

            if (res < 0)
                return agpu::buffer_ref();
        }
    }

    // Create the buffer object
    auto buffer = agpu::makeObject<ADXBuffer> (device);
    auto dxBuffer = buffer.as<ADXBuffer> ();
    dxBuffer->description = *description;
    dxBuffer->gpuResource = gpuResource;
    if(keepUploadBuffer)
        dxBuffer->uploadResource = uploadResource;

    // Create the buffer view.
    // FIXME: Remove these views and move them to the respective bindings.
    auto gpuBuffer = dxBuffer->getActualGpuBuffer();
    dxBuffer->gpuVirtualAddress = gpuBuffer->GetGPUVirtualAddress();;

    return buffer;
}

ID3D12Resource *ADXBuffer::getActualGpuBuffer()
{
    if (gpuResource)
        return gpuResource.Get();
    return uploadResource.Get();
}

agpu_error ADXBuffer::getDescription(agpu_buffer_description* description)
{
	CHECK_POINTER(description);
	*description = this->description;
	return AGPU_OK;
}

agpu_error ADXBuffer::createVertexBufferView(D3D12_VERTEX_BUFFER_VIEW *outView, agpu_size offset, agpu_size stride)
{
    if(offset > description.size)
        return AGPU_OUT_OF_BOUNDS;

    outView->BufferLocation = gpuVirtualAddress + offset;
    outView->SizeInBytes = description.size - offset;
    outView->StrideInBytes = stride;
    return AGPU_OK;
}

agpu_error ADXBuffer::createIndexBufferView(D3D12_INDEX_BUFFER_VIEW *outView, agpu_size offset, agpu_size index_size)
{
    if(offset > description.size)
        return AGPU_OUT_OF_BOUNDS;

    outView->BufferLocation = gpuVirtualAddress + offset;
    outView->SizeInBytes = description.size - offset;
    outView->Format = mapIndexFormat(index_size);
    return AGPU_OK;
}


agpu_error ADXBuffer::createConstantBufferViewDescription(D3D12_CONSTANT_BUFFER_VIEW_DESC *outView, agpu_size offset, agpu_size size)
{
    if(offset + size > description.size)
        return AGPU_OUT_OF_BOUNDS;

    outView->BufferLocation = gpuVirtualAddress + offset;
    outView->SizeInBytes = (size + 255) & (-256);
    return AGPU_OK;
}

agpu_error ADXBuffer::createUAVDescription(D3D12_UNORDERED_ACCESS_VIEW_DESC *outView, agpu_size offset, agpu_size size)
{
    if(offset + size > description.size)
        return AGPU_OUT_OF_BOUNDS;

    memset(outView, 0, sizeof(D3D12_UNORDERED_ACCESS_VIEW_DESC));
    outView->Format = DXGI_FORMAT_R32_TYPELESS;
    outView->ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    outView->Buffer.FirstElement = offset;
    outView->Buffer.NumElements = size;
    outView->Buffer.StructureByteStride = 1;
    outView->Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
    return AGPU_OK;
}

agpu_pointer ADXBuffer::mapBuffer(agpu_mapping_access flags)
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

agpu_error ADXBuffer::unmapBuffer()
{
    if (!mappedPointer)
        return AGPU_OK;

    uploadResource->Unmap(0, nullptr);
    mappedPointer = nullptr;

    // TODO: When the mapping is coherent, send the data to the gpu heap.
    return AGPU_OK;
}

agpu_error ADXBuffer::uploadBufferData(agpu_size offset, agpu_size size, agpu_pointer data)
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

    return deviceForDX->withTransferQueueAndCommandList([&](const ComPtr<ID3D12CommandQueue> &queue, const ComPtr<ID3D12GraphicsCommandList> &list) -> agpu_error {
        auto barrier = resourceTransitionBarrier(gpuResource, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);
        list->ResourceBarrier(1, &barrier);
        list->CopyBufferRegion(gpuResource.Get(), offset, uploadResource.Get(), offset, size);
        barrier = resourceTransitionBarrier(gpuResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
        list->ResourceBarrier(1, &barrier);
        list->Close();

        ID3D12CommandList *ptr = list.Get();
        queue->ExecuteCommandLists(1, &ptr);
        return deviceForDX->waitForMemoryTransfer();
    });
}

agpu_error ADXBuffer::readBufferData(agpu_size offset, agpu_size size, agpu_pointer data)
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

agpu_error ADXBuffer::flushWholeBuffer()
{
	return AGPU_UNIMPLEMENTED;
}

agpu_error ADXBuffer::invalidateWholeBuffer()
{
	return AGPU_UNIMPLEMENTED;
}
} // End of namespace AgpuD3D12
