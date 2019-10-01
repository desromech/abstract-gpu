#include "texture.hpp"
#include "texture_view.hpp"
#include "texture_formats.hpp"
#include "common_commands.hpp"

namespace AgpuD3D12
{

inline D3D12_RESOURCE_DIMENSION mapTextureDimension(agpu_texture_type type)
{
    switch (type)
    {
    case AGPU_TEXTURE_BUFFER: return D3D12_RESOURCE_DIMENSION_BUFFER;
    case AGPU_TEXTURE_1D: return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
    case AGPU_TEXTURE_2D: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    case AGPU_TEXTURE_CUBE: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    case AGPU_TEXTURE_3D: return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
    case AGPU_TEXTURE_UNKNOWN:
    default: abort();

    }
}

ADXTexture::ADXTexture(const agpu::device_ref &cdevice)
    : device(cdevice), mapCount(0), mappedPointer(nullptr)
{
}

ADXTexture::~ADXTexture()
{
}

agpu::texture_ref ADXTexture::create(const agpu::device_ref &device, agpu_texture_description* description)
{
    if (!description)
        return agpu::texture_ref();

    // The resource description.
    D3D12_RESOURCE_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.Dimension = mapTextureDimension(description->type);
    desc.Alignment = 0;
    desc.Width = description->width;
    desc.Height = description->height;
    desc.DepthOrArraySize = description->type == AGPU_TEXTURE_3D ? description->depth : description->layers;
    desc.MipLevels = description->miplevels;
    desc.Format = (DXGI_FORMAT)description->format;
    desc.SampleDesc.Count = description->sample_count;
    desc.SampleDesc.Quality = description->sample_quality;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    auto usageModes = description->usage_modes;
    auto mainUsageMode = description->main_usage_mode;
    if (usageModes & AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT)
        desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    if (usageModes & (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT))
        desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    if (usageModes & AGPU_TEXTURE_USAGE_STORAGE)
        desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    auto readedBack = (usageModes & AGPU_TEXTURE_USAGE_READED_BACK) != 0;
    auto uploaded = (usageModes & AGPU_TEXTURE_USAGE_UPLOADED) != 0;

    // Get the transfer footprint
    UINT transferBufferNumRows;
    UINT64 transferBufferPitch;
    UINT64 transferBufferLength;
    deviceForDX->d3dDevice->GetCopyableFootprints(&desc, 0, 1, 0, nullptr, &transferBufferNumRows, &transferBufferPitch, &transferBufferLength);

    // Create the transfer resource description
    D3D12_RESOURCE_DESC transferDesc;
    memset(&transferDesc, 0, sizeof(transferDesc));
    transferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    transferDesc.Width = transferBufferLength;
    transferDesc.Height = 1;
    transferDesc.DepthOrArraySize = 1;
    transferDesc.MipLevels = 1;
    transferDesc.Format = DXGI_FORMAT_UNKNOWN;
    transferDesc.SampleDesc.Count = 1;
    transferDesc.SampleDesc.Quality = 0;
    transferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    transferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    ComPtr<ID3D12Resource> gpuResource;
    ComPtr<ID3D12Resource> uploadResource;
    ComPtr<ID3D12Resource> readbackResource;

    auto texture = agpu::makeObject<ADXTexture> (device);
    auto adxTexture = texture.as<ADXTexture> ();
    adxTexture->description = *description;

    // The clear value
    D3D12_CLEAR_VALUE clearValueData;
    memset(&clearValueData, 0, sizeof(clearValueData));
    clearValueData.Format = desc.Format;
    auto clearValuePtr = &clearValueData;
    if (usageModes & (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT | AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT))
    {
        clearValueData.DepthStencil.Depth = 1;
        clearValueData.DepthStencil.Stencil = 0;
    }
    else if((usageModes & AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT) == 0)
    {
        clearValuePtr = nullptr;
    }

    if (uploaded)
    {
        D3D12_HEAP_PROPERTIES heapProperties;
        memset(&heapProperties, 0, sizeof(heapProperties));
        heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        auto initialState = D3D12_RESOURCE_STATE_GENERIC_READ;

        if (FAILED(deviceForDX->d3dDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &transferDesc, initialState, nullptr, IID_PPV_ARGS(&uploadResource))))
            return agpu::texture_ref();
    }

    {
        D3D12_HEAP_PROPERTIES heapProperties;
        memset(&heapProperties, 0, sizeof(heapProperties));
        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        auto initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
        if ((mainUsageMode & AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT) != 0)
            initialState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

        if (FAILED(deviceForDX->d3dDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc, initialState, clearValuePtr, IID_PPV_ARGS(&gpuResource))))
            return agpu::texture_ref();
    }

    if (readedBack)
    {
        D3D12_HEAP_PROPERTIES heapProperties;
        memset(&heapProperties, 0, sizeof(heapProperties));
        heapProperties.Type = D3D12_HEAP_TYPE_READBACK;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        auto initialState = D3D12_RESOURCE_STATE_COPY_DEST;

        if (FAILED(deviceForDX->d3dDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &transferDesc, initialState, nullptr, IID_PPV_ARGS(&readbackResource))))
            return agpu::texture_ref();
    }

    adxTexture->resourceDescription = desc;
    adxTexture->transferResourceDescription = transferDesc;
    adxTexture->transferBufferPitch = transferBufferPitch;
    adxTexture->transferBufferNumRows = transferBufferNumRows;
    adxTexture->gpuResource = gpuResource;
    adxTexture->uploadResource = uploadResource;
    adxTexture->readbackResource = readbackResource;
    return texture;
}

agpu::texture_ref ADXTexture::createFromResource(const agpu::device_ref &device, agpu_texture_description* description, const ComPtr<ID3D12Resource> &resource)
{
    if (!description)
        return agpu::texture_ref();

    auto texture = agpu::makeObject<ADXTexture> (device);
    auto adxTexture = texture.as<ADXTexture> ();
    adxTexture->description = *description;
    adxTexture->gpuResource = resource;

    return texture;
}

UINT ADXTexture::subresourceIndexFor(agpu_uint level, agpu_uint arrayIndex)
{
    return level + description.miplevels*arrayIndex;
}

agpu_error ADXTexture::getDescription(agpu_texture_description* description)
{
    CHECK_POINTER(description);
    *description = this->description;
    return AGPU_OK;
}

agpu_pointer ADXTexture::mapLevel(agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags, agpu_region3d* region)
{
    if (mappedPointer)
    {
        if (mappedLevel == level && mappedArrayIndex == arrayIndex)
        {
            ++mapCount;
            return mappedPointer;
        }
    }

    this->mappingFlags = flags;
    this->mappedLevel = level;
    this->mappedArrayIndex = arrayIndex;

    bool isRead = (flags & AGPU_READ_ONLY) != 0;
    bool isWrite = (flags & AGPU_WRITE_ONLY) != 0;

    UINT subresource = subresourceIndexFor(level, arrayIndex);
    if ((isRead && !isWrite) && readbackResource)
    {
        auto err = deviceForDX->withTransferQueueAndCommandList([&](const ComPtr<ID3D12CommandQueue> &queue, const ComPtr<ID3D12GraphicsCommandList> &list) -> agpu_error {
            D3D12_TEXTURE_COPY_LOCATION dst;
            dst.pResource = readbackResource.Get();
            dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            deviceForDX->d3dDevice->GetCopyableFootprints(&resourceDescription, subresource, 1, 0, &dst.PlacedFootprint, nullptr, nullptr, nullptr);

            D3D12_TEXTURE_COPY_LOCATION src;
            src.pResource = gpuResource.Get();
            src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            src.SubresourceIndex = subresource;

            list->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
            list->Close();

            ID3D12CommandList *ptr = list.Get();
            queue->ExecuteCommandLists(1, &ptr);
            return deviceForDX->waitForMemoryTransfer();
        });
        if (err < 0)
            return nullptr;

        if (FAILED(readbackResource->Map(subresource, nullptr, &mappedPointer)))
            return nullptr;

        ++mapCount;
        return mappedPointer;
    }
    else if (isWrite && uploadResource)
    {
        D3D12_RANGE readRange;
        memset(&readRange, 0, sizeof(readRange));

        if (FAILED(uploadResource->Map(subresource, isRead ? &readRange : nullptr, &mappedPointer)))
            return nullptr;

        ++mapCount;
        return mappedPointer;
    }
    else
    {
        return nullptr;
    }

}

agpu_error ADXTexture::unmapLevel()
{
    if (!mappedPointer)
        return AGPU_INVALID_OPERATION;

    if (--mapCount != 0)
        return AGPU_OK;

    bool isRead = (mappingFlags & AGPU_READ_ONLY) != 0;
    bool isWrite = (mappingFlags & AGPU_WRITE_ONLY) != 0;
    UINT subresource = subresourceIndexFor(mappedLevel, mappedArrayIndex);
    if ((isRead && !isWrite) && readbackResource)
    {
        readbackResource->Unmap(subresource, nullptr);
    }
    else if (isWrite && uploadResource)
    {
        uploadResource->Unmap(subresource, nullptr);
        mappedPointer = nullptr;
        return deviceForDX->withTransferQueueAndCommandList([&](const ComPtr<ID3D12CommandQueue> &queue, const ComPtr<ID3D12GraphicsCommandList> &list) -> agpu_error {
            D3D12_TEXTURE_COPY_LOCATION dst;
            dst.pResource = gpuResource.Get();
            dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            dst.SubresourceIndex = subresource;

            D3D12_TEXTURE_COPY_LOCATION src;
            src.pResource = uploadResource.Get();
            src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            deviceForDX->d3dDevice->GetCopyableFootprints(&resourceDescription, subresource, 1, 0, &src.PlacedFootprint, nullptr, nullptr, nullptr);

            auto barrier = resourceTransitionBarrier(gpuResource, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);
            list->ResourceBarrier(1, &barrier);
            list->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
            barrier = resourceTransitionBarrier(gpuResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
            list->ResourceBarrier(1, &barrier);
            list->Close();

            ID3D12CommandList *ptr = list.Get();
            queue->ExecuteCommandLists(1, &ptr);
            return deviceForDX->waitForMemoryTransfer();
        });
    }
    else
    {
        // Do nothing
    }

    mappedPointer = nullptr;
    return AGPU_OK;
}

agpu_error ADXTexture::readTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer)
{
    auto mappedPointer = mapLevel(level, arrayIndex, AGPU_READ_ONLY, nullptr);
    if (!mappedPointer)
        return AGPU_ERROR;

    UINT subresource = subresourceIndexFor(mappedLevel, mappedArrayIndex);

    D3D12_TEXTURE_COPY_LOCATION dst;
    dst.pResource = readbackResource.Get();
    dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    deviceForDX->d3dDevice->GetCopyableFootprints(&resourceDescription, subresource, 1, 0, &dst.PlacedFootprint, nullptr, nullptr, nullptr);

    UINT srcNumRows = dst.PlacedFootprint.Footprint.Height;
    UINT64 srcRowPitch = dst.PlacedFootprint.Footprint.RowPitch;
    UINT64 srcSlicePitch = srcNumRows * srcRowPitch;
    if (srcRowPitch == pitch && srcSlicePitch == slicePitch)
    {
        memcpy(buffer, mappedPointer, srcSlicePitch);
    }
    else
    {
        auto srcRow = reinterpret_cast<uint8_t*> (mappedPointer);
        auto dstRow = reinterpret_cast<uint8_t*> (buffer);
        for (int y = 0; y < srcNumRows; ++y)
        {
            memcpy(dstRow, srcRow, pitch);
            srcRow += srcRowPitch;
            dstRow += pitch;
        }
    }

    unmapLevel();

    return AGPU_OK;
}

agpu_error ADXTexture::uploadTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data)
{
    auto mappedPointer = mapLevel(level, arrayIndex, AGPU_WRITE_ONLY, nullptr);
    if(!mappedPointer)
        return AGPU_ERROR;

    UINT subresource = subresourceIndexFor(mappedLevel, mappedArrayIndex);

    UINT dstNumRows;
    UINT64 dstRowPitch;
    UINT64 dstTotalBytes;
    deviceForDX->d3dDevice->GetCopyableFootprints(&resourceDescription, subresource, 1, 0, nullptr, &dstNumRows, &dstRowPitch, &dstTotalBytes);

    UINT64 dstSlicePitch = dstNumRows * dstRowPitch;
    if (dstRowPitch == pitch && dstSlicePitch == slicePitch)
    {
        memcpy(mappedPointer, data, dstTotalBytes);
    }
    else
    {
        auto copyHeight = slicePitch / pitch;
        auto srcRow = reinterpret_cast<uint8_t*> (data);
        auto dstRow = reinterpret_cast<uint8_t*> (mappedPointer);
        for (int y = 0; y < copyHeight; ++y)
        {
            memcpy(dstRow, srcRow, pitch);
            srcRow += pitch;
            dstRow += dstRowPitch;
        }
    }


    unmapLevel();

    return AGPU_OK;
}

agpu_error ADXTexture::uploadTextureSubData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_size3d* sourceSize, agpu_region3d* destRegion, agpu_pointer data)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error ADXTexture::discardUploadBuffer()
{
    if (!gpuResource)
        return AGPU_INVALID_OPERATION;

    if (uploadResource)
    {
        uploadResource.Reset();
        description.usage_modes = agpu_texture_usage_mode_mask(description.usage_modes & ~AGPU_TEXTURE_USAGE_UPLOADED);
    }

    return AGPU_OK;
}

agpu_error ADXTexture::discardReadbackBuffer()
{
    if (!gpuResource)
        return AGPU_INVALID_OPERATION;

    if (readbackResource)
    {
        readbackResource.Reset();
        description.usage_modes = agpu_texture_usage_mode_mask(description.usage_modes & ~AGPU_TEXTURE_USAGE_READED_BACK);
    }

    return AGPU_OK;
}

bool ADXTexture::isArray()
{
    return description.type != AGPU_TEXTURE_3D && description.type != AGPU_TEXTURE_BUFFER && description.layers > 1;
}

agpu_error ADXTexture::getFullViewDescription(agpu_texture_view_description *viewDescription)
{
    CHECK_POINTER(viewDescription);
    memset(viewDescription, 0, sizeof(*viewDescription));
    viewDescription->type = description.type;
    viewDescription->format = description.format;
    viewDescription->sample_count = description.sample_count;
    viewDescription->components.r = AGPU_COMPONENT_SWIZZLE_R;
    viewDescription->components.g = AGPU_COMPONENT_SWIZZLE_G;
    viewDescription->components.b = AGPU_COMPONENT_SWIZZLE_B;
    viewDescription->components.a = AGPU_COMPONENT_SWIZZLE_A;
    viewDescription->subresource_range.usage_mode = description.usage_modes;
    viewDescription->subresource_range.base_miplevel = 0;
    viewDescription->subresource_range.level_count = description.miplevels;
    viewDescription->subresource_range.base_arraylayer = 0;
    viewDescription->subresource_range.layer_count = description.layers;
    if(viewDescription->subresource_range.layer_count == 1)
        viewDescription->subresource_range.layer_count = 0;
    return AGPU_OK;
}


agpu::texture_view_ptr ADXTexture::createView(agpu_texture_view_description* viewDescription)
{
	if (!viewDescription)
		return nullptr;

	return ADXTextureView::create(device, refFromThis<agpu::texture> (), *viewDescription).disown();
}

agpu::texture_view_ptr ADXTexture::getOrCreateFullView()
{
	if (!fullTextureView)
	{
		agpu_texture_view_description fullTextureViewDescription = {};
		getFullViewDescription(&fullTextureViewDescription);
		fullTextureView = agpu::texture_view_ref(createView(&fullTextureViewDescription));
	}

	return fullTextureView.disownedNewRef();
}

} // End of namespace AgpuD3D12
