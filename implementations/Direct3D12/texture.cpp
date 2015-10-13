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
{
}

void _agpu_texture::lostReferences()
{
}

agpu_texture* _agpu_texture::create(agpu_device* device, agpu_texture_description* description, agpu_pointer initialData)
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
    auto readedBack = (desc.Flags & AGPU_TEXTURE_FLAG_READED_BACK) != 0;
    auto uploadedOnce = (desc.Flags & AGPU_TEXTURE_FLAG_UPLOADED_ONCE) != 0;
    auto uploaded = (desc.Flags & AGPU_TEXTURE_FLAG_UPLOADED) != 0 || uploadedOnce;

    ComPtr<ID3D12Resource> gpuResource;
    ComPtr<ID3D12Resource> uploadResource;
    ComPtr<ID3D12Resource> readbackResource;

    std::unique_ptr<agpu_texture> texture(new agpu_texture());
    texture->device = device;
    texture->description = *description;

    if (uploaded)
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
        if (initialData)
        {
            void *bufferBegin;
            if (FAILED(uploadResource->Map(0, nullptr, &bufferBegin)))
                return nullptr;

            memcpy(bufferBegin, initialData, texture->sizeOfLevel(0));
            uploadResource->Unmap(0, nullptr);
        }
    }

    {
        D3D12_HEAP_PROPERTIES heapProperties;
        memset(&heapProperties, 0, sizeof(heapProperties));
        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        auto initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
        if (initialData)
            initialState = D3D12_RESOURCE_STATE_COPY_DEST;
        if (flags & AGPU_TEXTURE_FLAG_DEPTH_STENCIL)
            initialState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

        if (FAILED(device->d3dDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc, initialState, nullptr, IID_PPV_ARGS(&gpuResource))))
            return nullptr;

        if (initialData)
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

    if (readedBack)
    {
        D3D12_HEAP_PROPERTIES heapProperties;
        memset(&heapProperties, 0, sizeof(heapProperties));
        heapProperties.Type = D3D12_HEAP_TYPE_READBACK;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        auto initialState = D3D12_RESOURCE_STATE_COPY_DEST;

        if (FAILED(device->d3dDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc, initialState, nullptr, IID_PPV_ARGS(&readbackResource))))
            return nullptr;

    }

    texture->resourceDescription = desc;
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

size_t _agpu_texture::getPixelSize()
{
    return pixelSizeOfTextureFormat(description.format);
}

size_t _agpu_texture::pitchOfLevel(int level)
{
    size_t pixelSize = getPixelSize();
    auto width = description.width >> level;
    if (!width)
        width = 1;
    return (pixelSize * width + 3) & (~3);
}

size_t _agpu_texture::slicePitchOfLevel(int level)
{
    auto height = description.height >> level;
    if (!height)
        height = 1;
    return pitchOfLevel(level) * height;
}

size_t _agpu_texture::sizeOfLevel(int level)
{
    auto pitch = pitchOfLevel(level);
    auto slicePitch = pitchOfLevel(level);
    auto depth = description.depthOrArraySize >> level;
    if (!depth)
        depth = 1;

    switch (description.type)
    {
    case AGPU_TEXTURE_1D:
        return pitch*description.depthOrArraySize;
    case AGPU_TEXTURE_2D:
        return slicePitch*description.depthOrArraySize;
    case AGPU_TEXTURE_3D:
        return slicePitch * depth;
    case AGPU_TEXTURE_CUBE:
        return slicePitch*description.depthOrArraySize*6;
    default:
        abort();
    }
}

agpu_pointer _agpu_texture::mapLevel(agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags)
{
    return nullptr;
}

agpu_error _agpu_texture::unmapLevel()
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_texture::readTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_texture::uploadTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data)
{
    return AGPU_UNIMPLEMENTED;
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
