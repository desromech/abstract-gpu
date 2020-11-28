#include "texture.hpp"
#include "texture_view.hpp"
#include "texture_formats.hpp"
#include "common_commands.hpp"
#include "constants.hpp"
#include <assert.h>

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

static D3D12_SUBRESOURCE_FOOTPRINT getLevelFootprintExtent(const agpu_texture_description &description, int level)
{
    D3D12_SUBRESOURCE_FOOTPRINT footprint = {};
    footprint.Format = (DXGI_FORMAT)description.format;
    footprint.Width = description.width >> level;
    if (footprint.Width == 0)
        footprint.Width = 1;

    footprint.Height = description.height >> level;
    if (description.type == AGPU_TEXTURE_1D || footprint.Height == 0)
        footprint.Height = 1;

    footprint.Depth = description.depth >> level;
    if (description.type != AGPU_TEXTURE_3D || footprint.Depth == 0)
        footprint.Depth = 1;
    return footprint;
}

static void computeBufferImageTransferLayout(const agpu_texture_description &description, int level, D3D12_TEXTURE_COPY_LOCATION *bufferCopyLocation, size_t *transferRows, size_t *transferSlicePitch, size_t *transferSize)
{
    auto footprint = getLevelFootprintExtent(description, level);
    memset(bufferCopyLocation, 0, sizeof(*bufferCopyLocation));

    if (isCompressedTextureFormat(description.format))
    {
        auto compressedBlockSize = blockSizeOfCompressedTextureFormat(description.format);
        auto compressedBlockWidth = blockWidthOfCompressedTextureFormat(description.format);
        auto compressedBlockHeight = blockHeightOfCompressedTextureFormat(description.format);

        footprint.Width = (uint32_t)std::max(compressedBlockWidth, (footprint.Width + compressedBlockWidth - 1) / compressedBlockWidth * compressedBlockWidth);
        footprint.Height = (uint32_t)std::max(compressedBlockHeight, (footprint.Height + compressedBlockHeight - 1) / compressedBlockHeight * compressedBlockHeight);
    
        footprint.RowPitch = alignedTo(footprint.Width / compressedBlockWidth * compressedBlockSize, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
        *transferRows = footprint.Height / compressedBlockHeight;
        *transferSlicePitch = footprint.RowPitch * (footprint.Height / compressedBlockHeight);
        *transferSize = (*transferSlicePitch) * footprint.Depth;
    }
    else
    {
        auto uncompressedPixelSize = pixelSizeOfTextureFormat(description.format);
        footprint.RowPitch = alignedTo(footprint.Width*uncompressedPixelSize, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
        *transferRows = footprint.Height;
        *transferSlicePitch = footprint.RowPitch * footprint.Height;
        *transferSize = (*transferSlicePitch) * footprint.Depth;
    }

    bufferCopyLocation->Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    bufferCopyLocation->PlacedFootprint.Footprint = footprint;
}

ADXTexture::ADXTexture(const agpu::device_ref &cdevice)
    : device(cdevice), mapCount(0), mappedPointer(nullptr)
{
}

ADXTexture::~ADXTexture()
{
	resource.Reset();
	allocation.Reset();
}

agpu::texture_ref ADXTexture::create(const agpu::device_ref &device, agpu_texture_description* description)
{
    if (!description)
        return agpu::texture_ref();

    // The resource description.
    D3D12_RESOURCE_DESC desc = {};
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

    if (description->type == AGPU_TEXTURE_CUBE)
        desc.DepthOrArraySize *= 6;

    auto usageModes = description->usage_modes;
    auto mainUsageMode = description->main_usage_mode;
    if (usageModes & AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT)
        desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    auto isDepthStencil = (usageModes & (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT)) != 0;
	if (isDepthStencil)
	{
		desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		if ((usageModes & (AGPU_TEXTURE_USAGE_SAMPLED | AGPU_TEXTURE_USAGE_STORAGE)) == 0)
			desc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
	}
    if (usageModes & AGPU_TEXTURE_USAGE_STORAGE)
        desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    auto texture = agpu::makeObject<ADXTexture> (device);
    auto adxTexture = texture.as<ADXTexture> ();
    adxTexture->description = *description;

    D3D12MA::ALLOCATION_DESC allocationDesc = {};
    allocationDesc.HeapType = mapHeapType(description->heap_type);

    // Compute the initial clear value.
    D3D12_CLEAR_VALUE clearValueData = {};
	clearValueData.Format = (DXGI_FORMAT)defaultTypedFormatForTypeless(description->format, isDepthStencil);
    auto clearValuePointer = &clearValueData;
    if (usageModes & (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT | AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT))
    {
        clearValueData.DepthStencil.Depth = 1;
        clearValueData.DepthStencil.Stencil = 0;
    }
    else if((usageModes & AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT) == 0)
    {
        clearValuePointer = nullptr;
    }

    auto initialState = mapTextureUsageToResourceState(description->heap_type, description->main_usage_mode);
    if(FAILED(deviceForDX->memoryAllocator->CreateResource(&allocationDesc, &desc, initialState, clearValuePointer, &adxTexture->allocation, IID_PPV_ARGS(&adxTexture->resource))))
        return agpu::texture_ref();

    adxTexture->resourceDescription = desc;
    return texture;
}

agpu::texture_ref ADXTexture::createFromResource(const agpu::device_ref &device, agpu_texture_description* description, const ComPtr<ID3D12Resource> &resource)
{
    if (!description)
        return agpu::texture_ref();

    auto texture = agpu::makeObject<ADXTexture> (device);
    auto adxTexture = texture.as<ADXTexture> ();
    adxTexture->description = *description;
    adxTexture->resource = resource;

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
    return nullptr;
}

agpu_error ADXTexture::unmapLevel()
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error ADXTexture::readTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer)
{
    return readTextureSubData(level, arrayIndex, pitch, slicePitch, nullptr, nullptr, buffer);
}

agpu_error ADXTexture::readTextureSubData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_region3d* sourceRegion, agpu_size3d* destSize, agpu_pointer buffer)
{
    CHECK_POINTER(buffer);
    if ((description.usage_modes & AGPU_TEXTURE_USAGE_READED_BACK) == 0)
        return AGPU_INVALID_OPERATION;

    auto subresourceIndex = subresourceIndexFor(level, arrayIndex);

    // Compute the copy source footprint.
    D3D12_TEXTURE_COPY_LOCATION copySourceLocation= {};
    copySourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    copySourceLocation.SubresourceIndex = subresourceIndex;

    // Compute the copy source footprint.
    D3D12_TEXTURE_COPY_LOCATION copyDestinationLocation = {};
    size_t transferRows = 0;
    size_t transferSlicePitch = 0;
    size_t transferSize = 0;
    computeBufferImageTransferLayout(description, level, &copyDestinationLocation, &transferRows, &transferSlicePitch, &transferSize);

    agpu_error resultCode = AGPU_OK;
    deviceForDX->withReadbackCommandListDo(transferSize, 1, [&](ADXImplicitResourceReadbackCommandList& readbackList) {
        if (readbackList.currentStagingBufferSize < transferSize)
        {
            resultCode = AGPU_OUT_OF_MEMORY;
            return;
        }

        auto success = readbackList.setupCommandBuffer() &&
            readbackList.transitionTextureUsageMode(resource, description.heap_type, description.main_usage_mode, AGPU_TEXTURE_USAGE_COPY_SOURCE, subresourceIndex) &&
            readbackList.readbackImageDataToBuffer(copyDestinationLocation, resource, copySourceLocation) &&
            readbackList.transitionTextureUsageMode(resource, description.heap_type, AGPU_TEXTURE_USAGE_COPY_SOURCE, description.main_usage_mode, subresourceIndex) &&
            readbackList.submitCommandBufferAndWait() &&
            readbackList.lockBuffer();
        resultCode = success ? AGPU_OK : AGPU_ERROR;

        if (!success)
            return;
        // Copy the image data into the staging buffer.
        auto bufferPointer = readbackList.currentStagingBufferPointer;
        auto& copyFootprint = copyDestinationLocation.PlacedFootprint.Footprint;
        if (agpu_uint(pitch) == copyFootprint.RowPitch && agpu_uint(slicePitch) == transferSlicePitch && !destSize && !sourceRegion)
        {
            memcpy(buffer, bufferPointer, slicePitch);
        }
        else
        {
            auto srcRow = reinterpret_cast<uint8_t*> (bufferPointer);
            auto dstRow = reinterpret_cast<uint8_t*> (buffer);
            for (uint32_t y = 0; y < transferRows; ++y)
            {
                memcpy(dstRow, srcRow, pitch);
                dstRow += pitch;
                srcRow += copyFootprint.RowPitch;
            }
        }

    });

    assert(resultCode == AGPU_OK);
    return resultCode;
}

agpu_error ADXTexture::uploadTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data)
{
    return uploadTextureSubData(level, arrayIndex, pitch, slicePitch, nullptr, nullptr, data);
}

agpu_error ADXTexture::uploadTextureSubData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_size3d* sourceSize, agpu_region3d* destRegion, agpu_pointer data)
{
    CHECK_POINTER(data);
    if ((description.usage_modes & AGPU_TEXTURE_USAGE_UPLOADED) == 0)
        return AGPU_INVALID_OPERATION;

    auto subresourceIndex = subresourceIndexFor(level, arrayIndex);

    // Compute the copy source footprint.
    D3D12_TEXTURE_COPY_LOCATION copySourceLocation = {};
    size_t transferRows = 0;
    size_t transferSlicePitch = 0;
    size_t transferSize = 0;
    computeBufferImageTransferLayout(description, level, &copySourceLocation, &transferRows, &transferSlicePitch, &transferSize);

    // Compute the copy destination footprint.
    D3D12_TEXTURE_COPY_LOCATION copyDestinationLocation = {};
    copyDestinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    copyDestinationLocation.SubresourceIndex = subresourceIndex;

    agpu_error resultCode = AGPU_OK;
    deviceForDX->withUploadCommandListDo(transferSize, 1, [&](ADXImplicitResourceUploadCommandList &uploadList) {
        if(uploadList.currentStagingBufferSize < transferSize)
        {
            resultCode = AGPU_OUT_OF_MEMORY;
            return;
        }

        // Copy the image data into the staging buffer.
        auto bufferPointer = uploadList.currentStagingBufferPointer;
        auto &copyFootprint = copySourceLocation.PlacedFootprint.Footprint;
        if (agpu_uint(pitch) == copyFootprint.RowPitch && agpu_uint(slicePitch) == transferSlicePitch && !sourceSize && !destRegion)
        {
            memcpy(bufferPointer, data, slicePitch);
        }
        else
        {
            auto srcRow = reinterpret_cast<uint8_t*> (data);
            auto dstRow = reinterpret_cast<uint8_t*> (bufferPointer);
            for (uint32_t y = 0; y < transferRows; ++y)
            {
                memcpy(dstRow, srcRow, pitch);
                srcRow += pitch;
                dstRow += copyFootprint.RowPitch;
            }
        }

        auto success = uploadList.setupCommandBuffer() &&
            uploadList.transitionTextureUsageMode(resource, description.heap_type, description.main_usage_mode, AGPU_TEXTURE_USAGE_COPY_DESTINATION, subresourceIndex) &&
            uploadList.uploadBufferDataToImage(resource, copySourceLocation, copyDestinationLocation) &&
            uploadList.transitionTextureUsageMode(resource, description.heap_type, AGPU_TEXTURE_USAGE_COPY_DESTINATION, description.main_usage_mode, subresourceIndex) &&
            uploadList.submitCommandBufferAndWait();
        resultCode = success ? AGPU_OK : AGPU_ERROR;
    });

    assert(resultCode == AGPU_OK);
    return resultCode;
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
