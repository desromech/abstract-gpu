#include <algorithm>
#include "texture.hpp"
#include "texture_format.hpp"
#include "buffer.hpp"

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

inline enum VkImageViewType mapImageViewType(agpu_texture_type type, agpu_uint layerCount)
{
    switch (type)
    {
    case AGPU_TEXTURE_UNKNOWN:
    case AGPU_TEXTURE_BUFFER:
    case AGPU_TEXTURE_1D: return layerCount ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
    case AGPU_TEXTURE_2D: return layerCount ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
    case AGPU_TEXTURE_CUBE: return layerCount ? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : VK_IMAGE_VIEW_TYPE_CUBE;
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

        extent.width = (uint32_t)std::max(compressedBlockWidth, (extent.width + compressedBlockWidth - 1)/compressedBlockWidth*compressedBlockWidth);
        extent.height = (uint32_t)std::max(compressedBlockHeight, (extent.height + compressedBlockHeight - 1)/compressedBlockHeight*compressedBlockHeight);
        copy->imageExtent = extent;
        copy->bufferRowLength = extent.width;
        copy->bufferImageHeight = extent.height;

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
        layout->size = layout->depthPitch;
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
        vkDestroyImage(deviceForVk->device, image, nullptr);
    if(memory)
        vkFreeMemory(deviceForVk->device, memory, nullptr);
}

agpu::texture_ref AVkTexture::create(const agpu::device_ref &device, agpu_texture_description *description)
{
    if (!description)
        return agpu::texture_ref();

    // Create the image
    VkImageCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(createInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = mapImageType(description->type);
    createInfo.format = mapTextureFormat(description->format);
    createInfo.extent.width = description->width;
    createInfo.extent.height = description->height;
    createInfo.extent.depth = description->depth;
    createInfo.arrayLayers = description->layers;
    createInfo.mipLevels = description->miplevels;
    createInfo.samples = mapSampleCount(description->sample_count);
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(description->type == AGPU_TEXTURE_CUBE)
    {
        createInfo.arrayLayers *= 6;
        createInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }

    VkImageLayout initialLayout = VK_IMAGE_LAYOUT_GENERAL;
    VkAccessFlags initialLayoutAccessBits = VkAccessFlagBits(0);
    VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
    auto usageModes = description->usage_modes;
    if (usageModes & (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT))
    {
        createInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        initialLayoutAccessBits = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        imageAspect = 0;
        if (usageModes & (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT))
            imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (usageModes & (AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT))
            imageAspect |= VK_IMAGE_ASPECT_STENCIL_BIT;

        if (usageModes & AGPU_TEXTURE_USAGE_SAMPLED)
        {
            createInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
            initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            initialLayoutAccessBits = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        }
    }
    else if (usageModes & AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT)
    {
        createInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if (usageModes & AGPU_TEXTURE_USAGE_SAMPLED)
            createInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }
    else
    {
        createInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        if(!(usageModes & AGPU_TEXTURE_USAGE_STORAGE))
            initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    if(usageModes & AGPU_TEXTURE_USAGE_STORAGE)
        createInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    createInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    // Image memory allocation
    VkMemoryAllocateInfo allocateInfo;
    memset(&allocateInfo, 0, sizeof(allocateInfo));
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    // Create the image.
    VkImage image;
    auto error = vkCreateImage(deviceForVk->device, &createInfo, nullptr, &image);
    if (error)
        return agpu::texture_ref();

    // Get the memory requirements
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(deviceForVk->device, image, &memRequirements);
    allocateInfo.allocationSize = memRequirements.size;
    if (!deviceForVk->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &allocateInfo.memoryTypeIndex))
    {
        vkDestroyImage(deviceForVk->device, image, nullptr);
        return agpu::texture_ref();
    }

    // Allocate memory for the image.
    VkDeviceMemory textureMemory;
    error = vkAllocateMemory(deviceForVk->device, &allocateInfo, nullptr, &textureMemory);
    if (error)
    {
        vkDestroyImage(deviceForVk->device, image, nullptr);
        return agpu::texture_ref();
    }

    error = vkBindImageMemory(deviceForVk->device, image, textureMemory, 0);
    if (error)
    {
        vkDestroyImage(deviceForVk->device, image, nullptr);
        vkFreeMemory(deviceForVk->device, textureMemory, nullptr);
        return agpu::texture_ref();
    }

    VkImageSubresourceRange wholeImageSubresource;
    memset(&wholeImageSubresource, 0, sizeof(wholeImageSubresource));
    wholeImageSubresource.layerCount = createInfo.arrayLayers;
    wholeImageSubresource.levelCount = description->miplevels;

    if (initialLayout != createInfo.initialLayout)
    {
        bool success = false;
        if (usageModes & (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT))
        {
            VkClearDepthStencilValue clearValue;
            clearValue.depth = 1.0;
            clearValue.stencil = 0;
            success = deviceForVk->clearImageWithDepthStencil(image, wholeImageSubresource, imageAspect, createInfo.initialLayout, initialLayout, VkAccessFlagBits(0), &clearValue);
        }
        else if (usageModes & AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT)
        {
            VkClearColorValue clearValue;
            memset(&clearValue, 0, sizeof(clearValue));
            success = deviceForVk->clearImageWithColor(image, wholeImageSubresource, imageAspect, createInfo.initialLayout, initialLayout, VkAccessFlagBits(0), &clearValue);
        }
        else
        {
            VkClearColorValue clearValue;
            memset(&clearValue, 0, sizeof(clearValue));
            if(!isCompressedTextureFormat(description->format))
                success = deviceForVk->clearImageWithColor(image, wholeImageSubresource, imageAspect, createInfo.initialLayout, initialLayout, VkAccessFlagBits(0), &clearValue);
            else
                success = deviceForVk->setImageLayout(image, wholeImageSubresource, imageAspect, createInfo.initialLayout, initialLayout, VkAccessFlagBits(0));
        }

        if (!success)
        {
            vkDestroyImage(deviceForVk->device, image, nullptr);
            vkFreeMemory(deviceForVk->device, textureMemory, nullptr);
            return agpu::texture_ref();
        }
    }

    // Create the the buffers
    /// TODO: Remove this and use a shared buffer for uploading and reading textures!!.
    bool hasUploadBuffer = (description->usage_modes & AGPU_TEXTURE_USAGE_UPLOADED) != 0;
    bool hasReadbackBuffer = (description->usage_modes & AGPU_TEXTURE_USAGE_READED_BACK) != 0;
    VkSubresourceLayout transferLayout;
    memset(&transferLayout, 0, sizeof(transferLayout));

    agpu::buffer_ref uploadBuffer;
    agpu::buffer_ref readbackBuffer;
    if (hasUploadBuffer || hasReadbackBuffer)
    {
        VkBufferImageCopy copy;
        AgpuVulkan::computeBufferImageTransferLayout(*description, 0, &transferLayout, &copy);

        agpu_buffer_description bufferDesc;
        memset(&bufferDesc, 0, sizeof(bufferDesc));
        bufferDesc.binding = AGPU_GENERIC_DATA_BUFFER;
        bufferDesc.heap_type = AGPU_STREAM;
        bufferDesc.stride = 1;
        bufferDesc.size = (agpu_uint)transferLayout.size;

        // Create the upload buffer
        bool success = true;
        if (hasUploadBuffer)
        {
            auto uploadDesc = bufferDesc;
            uploadDesc.mapping_flags = AGPU_MAP_WRITE_BIT;
            uploadBuffer = AVkBuffer::create(device, &uploadDesc, nullptr);
            success = success && uploadBuffer;
        }

        // Create the read back buffer
        if (hasReadbackBuffer)
        {
            auto readbackDesc = bufferDesc;
            readbackDesc.mapping_flags = AGPU_MAP_READ_BIT;
            readbackBuffer = AVkBuffer::create(device, &readbackDesc, nullptr);
            success = success && readbackBuffer;
        }

        if (!success)
        {
            vkDestroyImage(deviceForVk->device, image, nullptr);
            vkFreeMemory(deviceForVk->device, textureMemory, nullptr);
            return agpu::texture_ref();
        }
    }

    auto result = agpu::makeObject<AVkTexture> (device);
    auto texture = result.as<AVkTexture> ();
    texture->description = *description;
    texture->image = image;
    texture->memory = textureMemory;
    texture->owned = true;
    texture->uploadBuffer = uploadBuffer;
    texture->readbackBuffer = readbackBuffer;
    texture->initialLayout = initialLayout;
    texture->initialLayoutAccessBits = initialLayoutAccessBits;
    texture->imageAspect = imageAspect;
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

VkImageView AVkTexture::createImageView(const agpu::device_ref &device, agpu_texture_view_description *viewDescription)
{
    if (!viewDescription || !viewDescription->texture)
        return VK_NULL_HANDLE;

    auto texture = agpu::texture_ref::import(viewDescription->texture);
    VkImageViewCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(createInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = texture.as<AVkTexture> ()->image;
    createInfo.viewType = mapImageViewType(viewDescription->type, viewDescription->subresource_range.layer_count);
    createInfo.format = mapTextureFormat(viewDescription->format);

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
    if(subresource.layerCount == 0)
        subresource.layerCount = 1;
    if(viewDescription->type == AGPU_TEXTURE_CUBE)
        subresource.layerCount *= 6;

    if((viewDescription->subresource_range.usage_mode & (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT)) == 0)
        subresource.aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
    if (viewDescription->subresource_range.usage_mode & AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT)
        subresource.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
    if (viewDescription->subresource_range.usage_mode & AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT)
        subresource.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

    VkImageView view;
    auto error = vkCreateImageView(deviceForVk->device, &createInfo, nullptr, &view);
    if (error)
        return VK_NULL_HANDLE;

    return view;
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
    viewDescription->texture = reinterpret_cast<agpu_texture*> (refFromThis().asPtrWithoutNewRef());
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
    CHECK_POINTER(buffer);
    if ((description.usage_modes & AGPU_TEXTURE_USAGE_READED_BACK) == 0)
        return AGPU_INVALID_OPERATION;

    VkImageSubresourceRange range;
    memset(&range, 0, sizeof(range));
    range.baseMipLevel = level;
    range.baseArrayLayer = arrayIndex;
    range.layerCount = 1;
    range.levelCount = 1;

    VkSubresourceLayout layout;
    VkBufferImageCopy copy;
    computeBufferImageTransferLayout(level, &layout, &copy);

    // Read the render target into the readback buffer.
    auto extent = getLevelExtent(level);
    copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.imageSubresource.mipLevel = level;
    copy.imageSubresource.baseArrayLayer = arrayIndex;
    copy.imageSubresource.layerCount = 1;
    copy.imageExtent = extent;
    deviceForVk->copyImageToBuffer(image, range, VK_IMAGE_ASPECT_COLOR_BIT, initialLayout, initialLayoutAccessBits, readbackBuffer.as<AVkBuffer> ()->uploadBuffer, 1, &copy);

    auto readbackPointer = readbackBuffer->mapBuffer(AGPU_READ_ONLY);
    if (!readbackPointer)
        return AGPU_ERROR;

    if (agpu_uint(pitch) == layout.rowPitch && agpu_uint(slicePitch) == layout.depthPitch)
    {
        memcpy(buffer, readbackPointer, slicePitch);
    }
    else
    {
        auto srcRow = reinterpret_cast<uint8_t*> (readbackPointer);
        auto dstRow = reinterpret_cast<uint8_t*> (buffer);
        for (uint32_t y = 0; y < copy.imageExtent.height; ++y)
        {
            memcpy(dstRow, srcRow, pitch);
            srcRow += layout.rowPitch;
            dstRow += pitch;
        }
    }

    readbackBuffer->unmapBuffer();

    return AGPU_OK;
}

agpu_error AVkTexture::uploadTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data)
{
    return uploadTextureSubData(level, arrayIndex, pitch, slicePitch, nullptr, nullptr, data);
}

agpu_error AVkTexture::uploadTextureSubData (agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_size3d* sourceSize, agpu_region3d* destRegion, agpu_pointer data )
{
    CHECK_POINTER(data);
    if ((description.usage_modes & AGPU_TEXTURE_USAGE_UPLOADED) == 0)
        return AGPU_INVALID_OPERATION;

    auto bufferPointer = uploadBuffer->mapBuffer(AGPU_WRITE_ONLY);
    if (!bufferPointer)
        return AGPU_ERROR;

    VkImageSubresourceRange range;
    memset(&range, 0, sizeof(range));
    range.baseMipLevel = level;
    range.baseArrayLayer = arrayIndex;
    range.layerCount = 1;
    range.levelCount = 1;

    VkSubresourceLayout layout;
    VkBufferImageCopy copy;
    computeBufferImageTransferLayout(level, &layout, &copy);

    auto &extent = copy.imageExtent;
    if (agpu_uint(pitch) == layout.rowPitch && agpu_uint(slicePitch) == layout.depthPitch && !sourceSize && !destRegion)
    {
        memcpy(bufferPointer, data, slicePitch);
    }
    else
    {
        auto srcRow = reinterpret_cast<uint8_t*> (data);
        auto dstRow = reinterpret_cast<uint8_t*> (bufferPointer);
        for (uint32_t y = 0; y < extent.height; ++y)
        {
            memcpy(dstRow, srcRow, pitch);
            srcRow += pitch;
            dstRow += layout.rowPitch;
        }
    }

    uploadBuffer->unmapBuffer();

    copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.imageSubresource.mipLevel = level;
    copy.imageSubresource.baseArrayLayer = arrayIndex;
    copy.imageSubresource.layerCount = 1;
    deviceForVk->copyBufferToImage(uploadBuffer.as<AVkBuffer> ()->uploadBuffer, image, range, VK_IMAGE_ASPECT_COLOR_BIT, initialLayout, initialLayoutAccessBits, 1, &copy);

    return AGPU_OK;
}

agpu_error AVkTexture::discardUploadBuffer()
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AVkTexture::discardReadbackBuffer()
{
    return AGPU_UNIMPLEMENTED;
}

} // End of namespace AgpuVulkan
