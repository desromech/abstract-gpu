#include "buffer.hpp"
#include "common_commands.hpp"
#include "constants.hpp"
#include <assert.h>

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
    : weakDevice(device)
{

}

ADXBuffer::~ADXBuffer()
{
	resource.Reset();
	allocation.Reset();
}

agpu::buffer_ref ADXBuffer::create(const agpu::device_ref &device, agpu_buffer_description* originalDescription, agpu_pointer initial_data)
{
    if (!device || !originalDescription)
        return agpu::buffer_ref();

    auto description = *originalDescription;

	if (description.mapping_flags & AGPU_MAP_DYNAMIC_STORAGE_BIT)
	{
		if (description.heap_type == AGPU_MEMORY_HEAP_TYPE_HOST_TO_DEVICE)
			description.mapping_flags |= AGPU_MAP_WRITE_BIT;
		else if (description.heap_type == AGPU_MEMORY_HEAP_TYPE_HOST)
			description.mapping_flags |= AGPU_MAP_READ_BIT | AGPU_MAP_WRITE_BIT;
		else if (description.heap_type == AGPU_MEMORY_HEAP_TYPE_DEVICE_TO_HOST)
			description.mapping_flags |= AGPU_MAP_READ_BIT;
	}


    // The resource description.
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment = 0;
    desc.Width = description.size;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// Uniform buffers require an alignment of 256 bytes.
	if ((description.usage_modes & AGPU_UNIFORM_BUFFER) != 0)
	{
        desc.Width = alignedTo(size_t(desc.Width), 256);
	}

	if ((description.usage_modes & AGPU_STORAGE_BUFFER) != 0)
	{
		desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

    D3D12MA::ALLOCATION_DESC allocationDesc = {};
    allocationDesc.HeapType = mapHeapType(description.heap_type);

    ComPtr<ID3D12Resource> resource;
    ComPtr<D3D12MA::Allocation> allocation;
    auto buffer = agpu::makeObject<ADXBuffer> (device);
    auto adxBuffer = buffer.as<ADXBuffer> ();
    adxBuffer->description = description;

    auto initialState = mapBufferUsageToResourceState(description.heap_type, description.main_usage_mode);
    if(FAILED(deviceForDX->memoryAllocator->CreateResource(&allocationDesc, &desc, initialState, NULL, &adxBuffer->allocation, IID_PPV_ARGS(&adxBuffer->resource))))
        return agpu::buffer_ref();

    // Create the buffer object
    adxBuffer->gpuVirtualAddress = adxBuffer->resource->GetGPUVirtualAddress();

    // Copy the initial data.
    if (initial_data != nullptr)
    {
        // Make sure the AGPU_MAP_DYNAMIC_STORAGE_BIT flag is setted while uploading the buffer data.
        adxBuffer->description.mapping_flags |= AGPU_MAP_DYNAMIC_STORAGE_BIT;
        auto error = adxBuffer->uploadBufferData(0, description.size, initial_data);
        if(error)
            return agpu::buffer_ref();

        if((description.mapping_flags & AGPU_MAP_DYNAMIC_STORAGE_BIT) == 0)
            adxBuffer->description.mapping_flags &= ~AGPU_MAP_DYNAMIC_STORAGE_BIT;
    }

    return buffer;
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
    outView->Buffer.FirstElement = offset/4;
    outView->Buffer.NumElements = size/4;
    outView->Buffer.StructureByteStride = 0;
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
    D3D12_RANGE readRange = {};
    if (flags & AGPU_READ_ONLY)
        readRange.End = description.size;

    agpu_pointer* mappedPointer;
    if (FAILED(resource->Map(0, &readRange, (void**)&mappedPointer)))
    {
        return nullptr;
    }

    return mappedPointer;
}

agpu_error ADXBuffer::unmapBuffer()
{
    resource->Unmap(0, nullptr);
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
    deviceForDX->withUploadCommandListDo(size, 1, [&](ADXImplicitResourceUploadCommandList &uploadList) {
        // Do we need to stream the buffer upload?.
        if(size <= uploadList.currentStagingBufferSize)
        {
            memcpy(uploadList.currentStagingBufferPointer, data, size);
            uploadResult = uploadList.setupCommandBuffer()
                && uploadList.transitionBufferUsageMode(resource, description.heap_type, description.main_usage_mode, AGPU_COPY_DESTINATION_BUFFER)
                && uploadList.uploadBufferData(resource, offset, size)
                && uploadList.transitionBufferUsageMode(resource, description.heap_type, AGPU_COPY_DESTINATION_BUFFER, description.main_usage_mode)
                && uploadList.submitCommandBufferAndWait();
        }
        else
        {
            abort();
        }
    });

    assert(uploadResult);
    return uploadResult ? AGPU_OK : AGPU_ERROR;
}

agpu_error ADXBuffer::readBufferData(agpu_size offset, agpu_size size, agpu_pointer data)
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
    deviceForDX->withReadbackCommandListDo(size, 1, [&](ADXImplicitResourceReadbackCommandList &readbackList) {
        // Do we need to stream the buffer upload?.
        if(size <= readbackList.currentStagingBufferSize)
        {
            readbackResult = readbackList.setupCommandBuffer()
                && readbackList.transitionBufferUsageMode(resource, description.heap_type, description.main_usage_mode, AGPU_COPY_SOURCE_BUFFER)
                && readbackList.readbackBufferData(resource, offset, size)
                && readbackList.transitionBufferUsageMode(resource, description.heap_type, AGPU_COPY_SOURCE_BUFFER, description.main_usage_mode)
                && readbackList.submitCommandBufferAndWait();
            memcpy(data, readbackList.currentStagingBufferPointer, size);
        }
        else
        {
            abort();
        }
    });

    return readbackResult ? AGPU_OK : AGPU_ERROR;
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
