#include "texture.hpp"
#include "texture_formats.hpp"
#include "common_commands.hpp"

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
_agpu_texture::_agpu_texture()
    : mapCount(0), mappedPointer(nullptr)
{
}

void _agpu_texture::lostReferences()
{
}

agpu_texture* _agpu_texture::create(agpu_device* device, agpu_texture_description* description)
{
    if (!description)
        return nullptr;

    // The resource description.
    D3D12_RESOURCE_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.Dimension = mapTextureDimension(description->type);
    desc.Alignment = 0;
    desc.Width = description->width;
    desc.Height = description->height;
    desc.DepthOrArraySize = description->depthOrArraySize;
    desc.MipLevels = description->miplevels;
    desc.Format = (DXGI_FORMAT)description->format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    auto flags = description->flags;
    if (flags & AGPU_TEXTURE_FLAG_RENDER_TARGET)
        desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    if (flags & AGPU_TEXTURE_FLAG_DEPTH_STENCIL)
        desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    if (flags & AGPU_TEXTURE_FLAG_UNORDERED_ACCESS)
        desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    auto readedBack = (flags & AGPU_TEXTURE_FLAG_READED_BACK) != 0;
    auto uploaded = (flags & AGPU_TEXTURE_FLAG_UPLOADED) != 0;

    // Get the transfer footprint
    UINT transferBufferNumRows;
    UINT64 transferBufferPitch;
    UINT64 transferBufferLength;
    device->d3dDevice->GetCopyableFootprints(&desc, 0, 1, 0, nullptr, &transferBufferNumRows, &transferBufferPitch, &transferBufferLength);

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

    std::unique_ptr<agpu_texture> texture(new agpu_texture());
    texture->device = device;
    texture->description = *description;

    // The clear value
    D3D12_CLEAR_VALUE clearValueData;
    memset(&clearValueData, 0, sizeof(clearValueData));
    clearValueData.Format = desc.Format;
    auto clearValuePtr = &clearValueData;
    if (flags & AGPU_TEXTURE_FLAG_DEPTH_STENCIL)
    {
        clearValueData.DepthStencil.Depth = 1;
        clearValueData.DepthStencil.Stencil = 0;
    }
    else if((flags & AGPU_TEXTURE_FLAG_RENDER_TARGET) == 0)
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

        if (FAILED(device->d3dDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &transferDesc, initialState, nullptr, IID_PPV_ARGS(&uploadResource))))
            return nullptr;
    }

    {
        D3D12_HEAP_PROPERTIES heapProperties;
        memset(&heapProperties, 0, sizeof(heapProperties));
        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        auto initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
        if (flags & AGPU_TEXTURE_FLAG_DEPTH_STENCIL)
            initialState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

        if (FAILED(device->d3dDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc, initialState, clearValuePtr, IID_PPV_ARGS(&gpuResource))))
            return nullptr;
    }

    if (readedBack)
    {
        D3D12_HEAP_PROPERTIES heapProperties;
        memset(&heapProperties, 0, sizeof(heapProperties));
        heapProperties.Type = D3D12_HEAP_TYPE_READBACK;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        auto initialState = D3D12_RESOURCE_STATE_COPY_DEST;

        if (FAILED(device->d3dDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &transferDesc, initialState, nullptr, IID_PPV_ARGS(&readbackResource))))
            return nullptr;
    }

    texture->resourceDescription = desc;
    texture->transferResourceDescription = transferDesc;
    texture->transferBufferPitch = transferBufferPitch;
    texture->transferBufferNumRows = transferBufferNumRows;
    texture->gpuResource = gpuResource;
    texture->uploadResource = uploadResource;
    texture->readbackResource = readbackResource;
    return texture.release();
}

agpu_texture* _agpu_texture::createFromResource(agpu_device* device, agpu_texture_description* description, const ComPtr<ID3D12Resource> &resource)
{
    if (!description)
        return nullptr;

    std::unique_ptr<agpu_texture> texture(new agpu_texture());
    texture->device = device;
    texture->description = *description;
    texture->gpuResource = resource;

    return texture.release();
}

UINT _agpu_texture::subresourceIndexFor(agpu_uint level, agpu_uint arrayIndex)
{
    return level + description.miplevels*arrayIndex;
}

agpu_pointer _agpu_texture::mapLevel(agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags)
{
    std::unique_lock<std::mutex> l(mapMutex);
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
        auto err = device->withTransferQueueAndCommandList([&](const ComPtr<ID3D12CommandQueue> &queue, const ComPtr<ID3D12GraphicsCommandList> &list) -> agpu_error {
            D3D12_TEXTURE_COPY_LOCATION dst;
            dst.pResource = readbackResource.Get();
            dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            device->d3dDevice->GetCopyableFootprints(&resourceDescription, subresource, 1, 0, &dst.PlacedFootprint, nullptr, nullptr, nullptr);

            D3D12_TEXTURE_COPY_LOCATION src;
            src.pResource = gpuResource.Get();
            src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            src.SubresourceIndex = subresource;

            list->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
            list->Close();

            ID3D12CommandList *ptr = list.Get();
            queue->ExecuteCommandLists(1, &ptr);
            return device->waitForMemoryTransfer();
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

agpu_error _agpu_texture::unmapLevel()
{
    std::unique_lock<std::mutex> l(mapMutex);
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
        return device->withTransferQueueAndCommandList([&](const ComPtr<ID3D12CommandQueue> &queue, const ComPtr<ID3D12GraphicsCommandList> &list) -> agpu_error {
            D3D12_TEXTURE_COPY_LOCATION dst;
            dst.pResource = gpuResource.Get();
            dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            dst.SubresourceIndex = subresource;

            D3D12_TEXTURE_COPY_LOCATION src;
            src.pResource = uploadResource.Get();
            src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            device->d3dDevice->GetCopyableFootprints(&resourceDescription, subresource, 1, 0, &src.PlacedFootprint, nullptr, nullptr, nullptr);

            auto barrier = resourceTransitionBarrier(gpuResource, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);
            list->ResourceBarrier(1, &barrier);
            list->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
            barrier = resourceTransitionBarrier(gpuResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
            list->ResourceBarrier(1, &barrier);
            list->Close();

            ID3D12CommandList *ptr = list.Get();
            queue->ExecuteCommandLists(1, &ptr);
            return device->waitForMemoryTransfer();
        });
    }
    else
    {
        // Do nothing
    }

    mappedPointer = nullptr;
    return AGPU_OK;
}

agpu_error _agpu_texture::readTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data)
{
    auto mappedPointer = mapLevel(level, arrayIndex, AGPU_READ_ONLY);
    if (!mappedPointer)
        return AGPU_ERROR;

    UINT subresource = subresourceIndexFor(mappedLevel, mappedArrayIndex);

    UINT srcNumRows;
    UINT64 srcRowPitch;
    UINT64 srcTotalBytes;
    device->d3dDevice->GetCopyableFootprints(&resourceDescription, subresource, 1, 0, nullptr, &srcNumRows, &srcRowPitch, &srcTotalBytes);

    UINT64 srcSlicePitch = srcNumRows * srcRowPitch;
    if (srcRowPitch == pitch && srcSlicePitch == slicePitch)
    {
        memcpy(data, mappedPointer, srcTotalBytes);
    }
    else
    {
        // TODO: Perform a slow copy.
        printError("Unimplemented slow texture data copy\n");
    }

    unmapLevel();

    return AGPU_OK;
}

agpu_error _agpu_texture::uploadTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data)
{
    auto mappedPointer = mapLevel(level, arrayIndex, AGPU_WRITE_ONLY);
    if(!mappedPointer)
        return AGPU_ERROR;

    UINT subresource = subresourceIndexFor(mappedLevel, mappedArrayIndex);
    
    UINT dstNumRows;
    UINT64 dstRowPitch;
    UINT64 dstTotalBytes;
    device->d3dDevice->GetCopyableFootprints(&resourceDescription, subresource, 1, 0, nullptr, &dstNumRows, &dstRowPitch, &dstTotalBytes);

    UINT64 dstSlicePitch = dstNumRows * dstRowPitch;
    if (dstRowPitch == pitch && dstSlicePitch == slicePitch)
    {
        memcpy(mappedPointer, data, dstTotalBytes);
    }
    else
    {
        // TODO: Perform a slow copy.
        printError("Unimplemented slow texture data copy\n");
    }
    

    unmapLevel();

    return AGPU_OK;
}

agpu_error _agpu_texture::discardUploadBuffer()
{
    if (!gpuResource)
        return AGPU_INVALID_OPERATION;

    if (uploadResource)
    {
        uploadResource.Reset();
        description.flags = agpu_texture_flags(description.flags & ~AGPU_TEXTURE_FLAG_UPLOADED);
    }

    return AGPU_OK;
}

agpu_error _agpu_texture::discardReadbackBuffer()
{
    if (!gpuResource)
        return AGPU_INVALID_OPERATION;

    if (readbackResource)
    {
        readbackResource.Reset();
        description.flags = agpu_texture_flags(description.flags & ~AGPU_TEXTURE_FLAG_READED_BACK);
    }

    return AGPU_OK;
}

bool _agpu_texture::isArray()
{
    return description.type != AGPU_TEXTURE_3D && description.type != AGPU_TEXTURE_BUFFER && description.depthOrArraySize > 1;
}

// Exported C interface
AGPU_EXPORT agpu_error agpuAddTextureReference(agpu_texture* texture)
{
    CHECK_POINTER(texture);
    return texture->retain();
}

AGPU_EXPORT agpu_error agpuReleaseTexture(agpu_texture* texture)
{
    CHECK_POINTER(texture);
    return texture->release();
}

AGPU_EXPORT agpu_error agpuGetTextureDescription(agpu_texture* texture, agpu_texture_description* description)
{
    CHECK_POINTER(texture);
    *description = texture->description;
    return AGPU_OK;
}

AGPU_EXPORT agpu_error agpuReadTextureData(agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer)
{
    CHECK_POINTER(texture);
    return texture->readTextureData(level, arrayIndex, pitch, slicePitch, buffer);
}

AGPU_EXPORT agpu_error agpuUploadTextureData(agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data)
{
    CHECK_POINTER(texture);
    return texture->uploadTextureData(level, arrayIndex, pitch, slicePitch, data);
}

AGPU_EXPORT agpu_error agpuDiscardTextureUploadBuffer(agpu_texture* texture)
{
    CHECK_POINTER(texture);
    return texture->discardUploadBuffer();
}

AGPU_EXPORT agpu_error agpuDiscardTextureReadbackBuffer(agpu_texture* texture)
{
    CHECK_POINTER(texture);
    return texture->discardReadbackBuffer();
}

AGPU_EXPORT agpu_pointer agpuMapTextureLevel(agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags)
{
    if (!texture)
        return nullptr;
    return texture->mapLevel(level, arrayIndex, flags);
}

AGPU_EXPORT agpu_error agpuUnmapTextureLevel(agpu_texture* texture)
{
    CHECK_POINTER(texture);
    return texture->unmapLevel();
}
