#include <algorithm>
#include "texture.hpp"
#include "texture_format.hpp"
#include "texture_view.hpp"
#include "buffer.hpp"
#include "constants.hpp"

namespace AgpuVulkan
{

inline enum VkImageType mapImageType(agpu_texture_type type)
{
    switch (type)
    {
    case AGPU_TEXTURE_2D: return VK_IMAGE_TYPE_2D;
    case AGPU_TEXTURE_CUBE: return VK_IMAGE_TYPE_2D;
    case AGPU_TEXTURE_3D: return VK_IMAGE_TYPE_3D;
    case AGPU_TEXTURE_UNKNOWN:
    case AGPU_TEXTURE_BUFFER:
    case AGPU_TEXTURE_1D:
    default: return VK_IMAGE_TYPE_1D;
    }
}

inline enum VkImageViewType mapImageViewType(agpu_texture_type type, bool isArray)
{
    switch (type)
    {
    case AGPU_TEXTURE_UNKNOWN:
    case AGPU_TEXTURE_BUFFER:
    case AGPU_TEXTURE_1D: return isArray ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
    case AGPU_TEXTURE_2D: return isArray ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
    case AGPU_TEXTURE_CUBE: return isArray ? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : VK_IMAGE_VIEW_TYPE_CUBE;
    case AGPU_TEXTURE_3D: return VK_IMAGE_VIEW_TYPE_3D;
    default: return VK_IMAGE_VIEW_TYPE_1D;
    }
}

inline enum VkComponentSwizzle mapComponentSwizzle(agpu_component_swizzle swizzle)
{
    switch (swizzle)
    {
    case AGPU_COMPONENT_SWIZZLE_IDENTITY: return VK_COMPONENT_SWIZZLE_IDENTITY;
    case AGPU_COMPONENT_SWIZZLE_ONE: return VK_COMPONENT_SWIZZLE_ONE;
    case AGPU_COMPONENT_SWIZZLE_ZERO: return VK_COMPONENT_SWIZZLE_ZERO;
    case AGPU_COMPONENT_SWIZZLE_R: return VK_COMPONENT_SWIZZLE_R;
    case AGPU_COMPONENT_SWIZZLE_G: return VK_COMPONENT_SWIZZLE_G;
    case AGPU_COMPONENT_SWIZZLE_B: return VK_COMPONENT_SWIZZLE_B;
    case AGPU_COMPONENT_SWIZZLE_A: return VK_COMPONENT_SWIZZLE_A;
    default: return VK_COMPONENT_SWIZZLE_ZERO;
    }
}

static VkExtent3D getLevelExtent(const agpu_texture_description &description, int level)
{
    VkExtent3D extent;
    extent.width = description.width >> level;
    if (extent.width == 0)
        extent.width = 1;

    extent.height = description.height >> level;
    if (description.type == AGPU_TEXTURE_1D || extent.height == 0)
        extent.height = 1;

    extent.depth = description.depth >> level;
    if (description.type != AGPU_TEXTURE_3D || extent.depth == 0)
        extent.depth = 1;
    return extent;
}

static void computeBufferImageTransferLayout(const agpu_texture_description &description, int level, VkSubresourceLayout *layout, VkBufferImageCopy *copy)
{
    auto extent = getLevelExtent(description, level);
    memset(copy, 0, sizeof(*copy));
    memset(layout, 0, sizeof(*layout));

    // vkGetImageSubResource layout is not appropiate for this.
    if(isCompressedTextureFormat(description.format))
    {
        auto compressedBlockSize = blockSizeOfCompressedTextureFormat(description.format);
        auto compressedBlockWidth = blockWidthOfCompressedTextureFormat(description.format);
        auto compressedBlockHeight = blockHeightOfCompressedTextureFormat(description.format);

        auto alignedExtent = extent;
        alignedExtent.width = (uint32_t)std::max(compressedBlockWidth, (extent.width + compressedBlockWidth - 1)/compressedBlockWidth*compressedBlockWidth);
        alignedExtent.height = (uint32_t)std::max(compressedBlockHeight, (extent.height + compressedBlockHeight - 1)/compressedBlockHeight*compressedBlockHeight);
        copy->imageExtent = extent;
        copy->bufferRowLength = alignedExtent.width;
        copy->bufferImageHeight = alignedExtent.height;

        layout->rowPitch = copy->bufferRowLength / compressedBlockWidth * compressedBlockSize;
        layout->depthPitch = layout->rowPitch * (copy->bufferImageHeight / compressedBlockHeight) ;
        layout->size = layout->depthPitch * extent.depth;
    }
    else
    {
        copy->imageExtent = extent;

        auto uncompressedPixelSize = pixelSizeOfTextureFormat(description.format);
        layout->rowPitch = (extent.width*uncompressedPixelSize + 3) & -4;
        layout->depthPitch = layout->rowPitch * extent.height;
        layout->size = layout->depthPitch * extent.depth;
        copy->bufferRowLength = uint32_t(layout->rowPitch / uncompressedPixelSize);
        copy->bufferImageHeight = uint32_t(extent.height);
    }
}

AVkTexture::AVkTexture(const agpu::device_ref &device)
    : device(device)
{
    image = VK_NULL_HANDLE;
    owned = false;
}

AVkTexture::~AVkTexture()
{
    if (!owned)
        return;

    if(image)
        vmaDestroyImage(deviceForVk->sharedContext->memoryAllocator, image, memory);
}

agpu::texture_ref AVkTexture::create(const agpu::device_ref &device, agpu_texture_description *description)
{
    if (!description)
        return agpu::texture_ref();

    if (description->type == AGPU_TEXTURE_CUBE && description->layers % 6 != 0)
        return agpu::texture_ref();

    auto isDepthStencil = (description->usage_modes & (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT)) != 0;

    // Create the image
    VkImageCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = mapImageType(description->type);
    createInfo.format = mapTextureFormat(description->format, isDepthStencil);
    createInfo.extent.width = description->width;
    createInfo.extent.height = description->height;
    createInfo.extent.depth = description->depth;
    createInfo.arrayLayers = description->layers;
    createInfo.mipLevels = description->miplevels;
    createInfo.samples = mapSampleCount(description->sample_count);
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    auto initialUsageMode = AGPU_TEXTURE_USAGE_NONE;

    if(description->type == AGPU_TEXTURE_CUBE)
        createInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

    auto usageModes = description->usage_modes;
    auto mainUsageMode = description->main_usage_mode;
    //VkImageLayout initialLayout = mapTextureUsageModeToLayout(usageModes, mainUsageMode);
    VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;

    if(usageModes & AGPU_TEXTURE_USAGE_SAMPLED)
        createInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
    if(usageModes & AGPU_TEXTURE_USAGE_STORAGE)
        createInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    if (usageModes & AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT)
        createInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (isDepthStencil)
    {
        createInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageAspect = 0;
        if (usageModes & (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT))
            imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (usageModes & (AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT))
            imageAspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    createInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = mapHeapType(description->heap_type);
    VkImage image;
    VmaAllocation textureMemory;
    auto error = vmaCreateImage(deviceForVk->sharedContext->memoryAllocator, &createInfo, &allocInfo, &image, &textureMemory, nullptr);
    if(error)
        return agpu::texture_ref();

    VkImageSubresourceRange wholeImageSubresource = {};
    wholeImageSubresource.layerCount = createInfo.arrayLayers;
    wholeImageSubresource.levelCount = description->miplevels;
    wholeImageSubresource.aspectMask = imageAspect;

    bool success = false;
    auto& descriptionClearValue = description->clear_value;
    VkClearColorValue colorClearValue = {};
    colorClearValue.float32[0] = descriptionClearValue.color.r;
    colorClearValue.float32[1] = descriptionClearValue.color.g;
    colorClearValue.float32[2] = descriptionClearValue.color.b;
    colorClearValue.float32[3] = descriptionClearValue.color.a;

    if (usageModes & (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT))
    {
        VkClearDepthStencilValue depthStencilClearValue = {};
        depthStencilClearValue.depth = descriptionClearValue.depth_stencil.depth;
        depthStencilClearValue.stencil = descriptionClearValue.depth_stencil.stencil;
        deviceForVk->withSetupCommandListDo([&] (AVkImplicitResourceSetupCommandList &setupList) {
            success =
                setupList.setupCommandBuffer() &&
                setupList.transitionImageUsageMode(image, usageModes, AGPU_TEXTURE_USAGE_NONE, AGPU_TEXTURE_USAGE_COPY_DESTINATION, wholeImageSubresource) &&
                setupList.clearImageWithDepthStencil(image, wholeImageSubresource, usageModes, AGPU_TEXTURE_USAGE_COPY_DESTINATION, &depthStencilClearValue) &&
                setupList.transitionImageUsageMode(image, usageModes, AGPU_TEXTURE_USAGE_COPY_DESTINATION, mainUsageMode, wholeImageSubresource) &&
                setupList.submitCommandBuffer();
        });
    }
    else if (usageModes == AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT)
    {
        deviceForVk->withSetupCommandListDo([&] (AVkImplicitResourceSetupCommandList &setupList) {
            success =
                setupList.setupCommandBuffer() &&
                setupList.transitionImageUsageMode(image, usageModes, initialUsageMode, AGPU_TEXTURE_USAGE_COPY_DESTINATION, wholeImageSubresource) &&
                setupList.clearImageWithColor(image, wholeImageSubresource, usageModes, AGPU_TEXTURE_USAGE_COPY_DESTINATION, &colorClearValue) &&
                setupList.transitionImageUsageMode(image, usageModes, AGPU_TEXTURE_USAGE_COPY_DESTINATION, mainUsageMode, wholeImageSubresource) &&
                setupList.submitCommandBuffer();
        });
    }
    else
    {
        if(!isCompressedTextureFormat(description->format))
        {
            deviceForVk->withSetupCommandListDo([&] (AVkImplicitResourceSetupCommandList &setupList) {
                success =
                    setupList.setupCommandBuffer() &&
                    setupList.transitionImageUsageMode(image, usageModes, initialUsageMode, AGPU_TEXTURE_USAGE_COPY_DESTINATION, wholeImageSubresource) &&
                    setupList.clearImageWithColor(image, wholeImageSubresource, usageModes, AGPU_TEXTURE_USAGE_COPY_DESTINATION, &colorClearValue) &&
                    setupList.transitionImageUsageMode(image, usageModes, AGPU_TEXTURE_USAGE_COPY_DESTINATION, mainUsageMode, wholeImageSubresource) &&
                    setupList.submitCommandBuffer();
            });
        }
        else
        {
            deviceForVk->withSetupCommandListDo([&] (AVkImplicitResourceSetupCommandList &setupList) {
                success =
                    setupList.setupCommandBuffer() &&
                    setupList.transitionImageUsageMode(image, usageModes, initialUsageMode, mainUsageMode, wholeImageSubresource) &&
                    setupList.submitCommandBuffer();
            });
        }
    }

    if (!success)
    {
        vmaDestroyImage(deviceForVk->sharedContext->memoryAllocator, image, textureMemory);
        return agpu::texture_ref();
    }

    auto result = agpu::makeObject<AVkTexture> (device);
    auto texture = result.as<AVkTexture> ();
    texture->description = *description;
    texture->image = image;
    texture->memory = textureMemory;
    texture->owned = true;
    texture->imageAspect = imageAspect;

    if (isCompressedTextureFormat(description->format))
    {
        texture->texelSize = uint8_t(blockSizeOfCompressedTextureFormat(description->format));
        texture->texelWidth = uint8_t(blockWidthOfCompressedTextureFormat(description->format));
        texture->texelHeight = uint8_t(blockHeightOfCompressedTextureFormat(description->format));
    }
    else
    {
        texture->texelSize = uint8_t(pixelSizeOfTextureFormat(description->format));
        texture->texelWidth = 1;
        texture->texelHeight = 1;
    }
    
    return result;
}

agpu::texture_ref AVkTexture::createFromImage(const agpu::device_ref &device, agpu_texture_description *description, VkImage image)
{
    auto result = agpu::makeObject<AVkTexture> (device);
    auto texture = result.as<AVkTexture> ();
    texture->description = *description;
    texture->image = image;
    return result;
}

agpu::texture_view_ptr AVkTexture::createView(agpu_texture_view_description* viewDescription)
{
    if(!viewDescription) return nullptr;
    if(viewDescription->type == AGPU_TEXTURE_CUBE && viewDescription->subresource_range.layer_count % 6 != 0) return nullptr;

    bool isArray = viewDescription->type == AGPU_TEXTURE_CUBE
        ? viewDescription->subresource_range.layer_count > 6
        : viewDescription->subresource_range.layer_count > 1;
	VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;
    createInfo.viewType = mapImageViewType(viewDescription->type, isArray);
    createInfo.format = mapTextureFormat(description.format, imageAspect & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT));

    auto &components = createInfo.components;
    components.r = mapComponentSwizzle(viewDescription->components.r);
    components.g = mapComponentSwizzle(viewDescription->components.g);
    components.b = mapComponentSwizzle(viewDescription->components.b);
    components.a = mapComponentSwizzle(viewDescription->components.a);

    auto &subresource = createInfo.subresourceRange;
    subresource.baseMipLevel = viewDescription->subresource_range.base_miplevel;
    subresource.levelCount = viewDescription->subresource_range.level_count;
    subresource.baseArrayLayer = viewDescription->subresource_range.base_arraylayer;
    subresource.layerCount = viewDescription->subresource_range.layer_count;
    subresource.aspectMask = viewDescription->subresource_range.aspect;

    VkImageView viewHandle;
    auto error = vkCreateImageView(deviceForVk->device, &createInfo, nullptr, &viewHandle);
    if (error)
        return VK_NULL_HANDLE;

    return AVkTextureView::create(device, refFromThis<agpu::texture> (), viewHandle, mapTextureUsageModeToLayout(viewDescription->usage_mode), *viewDescription).disown();
}

agpu::texture_view_ptr AVkTexture::getOrCreateFullView()
{
    if(!fullTextureView)
    {
        agpu_texture_view_description fullTextureViewDescription = {};
        getFullViewDescription(&fullTextureViewDescription);
        fullTextureView = agpu::texture_view_ref(createView(&fullTextureViewDescription));
    }

    return fullTextureView.disownedNewRef();
}

agpu_error AVkTexture::getDescription(agpu_texture_description* description)
{
    CHECK_POINTER(description);
    *description = this->description;
    return AGPU_OK;
}

agpu_error AVkTexture::getFullViewDescription(agpu_texture_view_description *viewDescription)
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
    viewDescription->usage_mode = description.main_usage_mode;
    viewDescription->subresource_range.aspect = agpu_texture_aspect(imageAspect);
    viewDescription->subresource_range.base_miplevel = 0;
    viewDescription->subresource_range.level_count = description.miplevels;
    viewDescription->subresource_range.base_arraylayer = 0;
    viewDescription->subresource_range.layer_count = description.layers;
    return AGPU_OK;
}

agpu_pointer AVkTexture::mapLevel(agpu_int level, agpu_int arrayIndex, agpu_mapping_access usageModes, agpu_region3d *region)
{
    return nullptr;
}

agpu_error AVkTexture::unmapLevel()
{
    return AGPU_UNIMPLEMENTED;
}

VkExtent3D AVkTexture::getLevelExtent(int level)
{
    return AgpuVulkan::getLevelExtent(description, level);
}

void AVkTexture::computeBufferImageTransferLayout(int level, VkSubresourceLayout *layout, VkBufferImageCopy *copy)
{
    return AgpuVulkan::computeBufferImageTransferLayout(description, level, layout, copy);
}

agpu_error AVkTexture::readTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer)
{
	return readTextureSubData(level, arrayIndex, pitch, slicePitch, nullptr, nullptr, buffer);
}

agpu_error AVkTexture::readTextureSubData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_region3d* sourceRegion, agpu_size3d* destSize, agpu_pointer buffer)
{
    CHECK_POINTER(buffer);
    if ((description.usage_modes & AGPU_TEXTURE_USAGE_READED_BACK) == 0)
    {
        return AGPU_INVALID_OPERATION;
    }

    VkImageSubresourceRange range;
    memset(&range, 0, sizeof(range));
    range.baseMipLevel = level;
    range.baseArrayLayer = arrayIndex;
    range.layerCount = 1;
    range.levelCount = 1;
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    VkSubresourceLayout layout;
    VkBufferImageCopy copy;
    computeBufferImageTransferLayout(level, &layout, &copy);

    auto extent = getLevelExtent(level);
    copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.imageSubresource.mipLevel = level;
    copy.imageSubresource.baseArrayLayer = arrayIndex;
    copy.imageSubresource.layerCount = 1;
    copy.imageExtent = extent;

    agpu_error resultCode = AGPU_ERROR;
    deviceForVk->withReadbackCommandListDo(layout.size, 1, [&](AVkImplicitResourceReadbackCommandList &readbackList) {
        if(readbackList.currentStagingBufferSize < layout.size)
        {
            resultCode = AGPU_OUT_OF_MEMORY;
            return;
        }

        // Copy the image data into staging buffer.
        auto success = readbackList.setupCommandBuffer() &&
            readbackList.transitionImageUsageMode(image, description.usage_modes, description.main_usage_mode, AGPU_TEXTURE_USAGE_COPY_SOURCE, range) &&
            readbackList.readbackImageDataToBuffer(image, copy) &&
            readbackList.transitionImageUsageMode(image, description.usage_modes, AGPU_TEXTURE_USAGE_COPY_SOURCE, description.main_usage_mode, range) &&
            readbackList.submitCommandBuffer();

        if(success)
        {
            auto readbackPointer = readbackList.currentStagingBufferPointer;
            if (agpu_uint(pitch) == layout.rowPitch && agpu_uint(slicePitch) == layout.depthPitch)
            {
                memcpy(buffer, readbackPointer, slicePitch);
            }
            else
            {
                auto srcSlice = reinterpret_cast<uint8_t*> (readbackPointer);
                auto dstSlice = reinterpret_cast<uint8_t*> (buffer);
                for (uint32_t z = 0; z < copy.imageExtent.depth; ++z)
                {
                    auto srcRow = srcSlice;
                    auto dstRow = dstSlice;
                    for (uint32_t y = 0; y < copy.imageExtent.height; ++y)
                    {
                        memcpy(dstRow, srcRow, pitch);
                        srcRow += layout.rowPitch;
                        dstRow += pitch;
                    }

                    srcSlice += layout.depthPitch;
                    destSize += slicePitch;
                }
            }
        }
        resultCode = success ? AGPU_OK : AGPU_ERROR;
    });

    return resultCode;
}

agpu_error AVkTexture::uploadTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data)
{
    return uploadTextureSubData(level, arrayIndex, pitch, slicePitch, nullptr, nullptr, data);
}

agpu_error AVkTexture::uploadTextureSubData (agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_size3d* sourceSize, agpu_region3d* destRegion, agpu_pointer data )
{
    CHECK_POINTER(data);
    if ((description.usage_modes & AGPU_TEXTURE_USAGE_UPLOADED) == 0)
    {
        return AGPU_INVALID_OPERATION;
    }

	VkImageSubresourceRange range = {};
    range.baseMipLevel = level;
    range.baseArrayLayer = arrayIndex;
    range.layerCount = 1;
    range.levelCount = 1;
	range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    VkSubresourceLayout layout;
    VkBufferImageCopy copy;
    computeBufferImageTransferLayout(level, &layout, &copy);

    copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.imageSubresource.mipLevel = level;
    copy.imageSubresource.baseArrayLayer = arrayIndex;
    copy.imageSubresource.layerCount = 1;

    agpu_error resultCode = AGPU_OK;
    deviceForVk->withUploadCommandListDo(layout.size, 1, [&](AVkImplicitResourceUploadCommandList &uploadList) {
        if(uploadList.currentStagingBufferSize < layout.size)
        {
            resultCode = AGPU_OUT_OF_MEMORY;
            return;
        }

        // Copy the image data into the staging buffer.
        auto bufferPointer = uploadList.currentStagingBufferPointer;
        auto &extent = copy.imageExtent;
        if (agpu_uint(pitch) == layout.rowPitch && agpu_uint(slicePitch) == layout.depthPitch && !sourceSize && !destRegion)
        {
            memcpy(bufferPointer, data, slicePitch);
        }
        else
        {
            auto srcSlice = reinterpret_cast<uint8_t*> (data);
            auto dstSlice = reinterpret_cast<uint8_t*> (bufferPointer);
            auto absPitch = pitch < 0 ? -pitch : pitch;
            auto rowCopySize = std::min(absPitch, agpu_int(layout.rowPitch));
            for(uint32_t z = 0; z < extent.depth; ++z)
            {
                auto srcRow = srcSlice;
                auto dstRow = dstSlice;
                for (uint32_t y = 0; y < extent.height; ++y)
                {
                    memcpy(dstRow, srcRow, rowCopySize);
                    srcRow += pitch;
                    dstRow += layout.rowPitch;
                }

                srcSlice += slicePitch;
                dstSlice += layout.depthPitch;
            }
        }

        auto success = uploadList.setupCommandBuffer() &&
            uploadList.transitionImageUsageMode(image, description.usage_modes, description.main_usage_mode, AGPU_TEXTURE_USAGE_COPY_DESTINATION, range) &&
            uploadList.uploadBufferDataToImage(image, copy) &&
            uploadList.transitionImageUsageMode(image, description.usage_modes, AGPU_TEXTURE_USAGE_COPY_DESTINATION, description.main_usage_mode, range) &&
            uploadList.submitCommandBuffer();
        resultCode = success ? AGPU_OK : AGPU_ERROR;
    });

    return resultCode;
}

} // End of namespace AgpuVulkan
