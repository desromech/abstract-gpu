#include "texture.hpp"
#include "texture_format.hpp"
#include "buffer.hpp"

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

inline enum VkImageViewType mapImageViewType(agpu_texture_type type)
{
    switch (type)
    {
    case AGPU_TEXTURE_UNKNOWN:
    case AGPU_TEXTURE_BUFFER:
    case AGPU_TEXTURE_1D: return VK_IMAGE_VIEW_TYPE_1D;
    case AGPU_TEXTURE_2D: return VK_IMAGE_VIEW_TYPE_2D;
    case AGPU_TEXTURE_CUBE: return VK_IMAGE_VIEW_TYPE_CUBE;
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
_agpu_texture::_agpu_texture(agpu_device *device)
    : device(device)
{
    image = VK_NULL_HANDLE;
    owned = false;
    uploadBuffer = nullptr;
    readbackBuffer = nullptr;
}

void _agpu_texture::lostReferences()
{
    if (!owned)
        return;

    if(image)
        vkDestroyImage(device->device, image, nullptr);
    if(memory)
        vkFreeMemory(device->device, memory, nullptr);
    if (uploadBuffer)
        uploadBuffer->release();
    if (readbackBuffer)
        readbackBuffer->release();
}

agpu_texture *_agpu_texture::create(agpu_device *device, agpu_texture_description *description)
{
    if (!description)
        return nullptr;

    uint32_t depth = 1;
    uint32_t arrayLayers = description->depthOrArraySize;
    if (description->type == AGPU_TEXTURE_3D)
    {
        depth = description->depthOrArraySize;
        arrayLayers = 1;
    }

    // Create the image
    VkImageCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(createInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = mapImageType(description->type);
    createInfo.format = mapTextureFormat(description->format);
    createInfo.extent.width = description->width;
    createInfo.extent.height = description->height;
    createInfo.extent.depth = depth;
    createInfo.arrayLayers = arrayLayers;
    createInfo.mipLevels = description->miplevels;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkImageLayout initialLayout = VK_IMAGE_LAYOUT_GENERAL;
    VkAccessFlagBits initialLayoutAccessBits = VkAccessFlagBits(0);
    VkImageAspectFlagBits imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
    auto flags = description->flags;
    if (flags & (AGPU_TEXTURE_FLAG_DEPTH | AGPU_TEXTURE_FLAG_STENCIL))
    {
        createInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        initialLayoutAccessBits = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        imageAspect = VkImageAspectFlagBits(0);
        if (flags & (AGPU_TEXTURE_FLAG_DEPTH))
            imageAspect = VkImageAspectFlagBits(imageAspect | VK_IMAGE_ASPECT_DEPTH_BIT);
        if (flags & (AGPU_TEXTURE_FLAG_STENCIL))
            imageAspect = VkImageAspectFlagBits(imageAspect | VK_IMAGE_ASPECT_STENCIL_BIT);
    }
    else if (flags & AGPU_TEXTURE_FLAG_RENDER_TARGET)
    {
        createInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    }
    else
    {
        createInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }

    createInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    // Image memory allocation
    VkMemoryAllocateInfo allocateInfo;
    memset(&allocateInfo, 0, sizeof(allocateInfo));
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    // Create the image.
    VkImage image;
    auto error = vkCreateImage(device->device, &createInfo, nullptr, &image);
    if (error)
        return nullptr;

    // Get the memory requirements
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device->device, image, &memRequirements);
    allocateInfo.allocationSize = memRequirements.size;
    if (!device->findMemoryType(memRequirements.memoryTypeBits, 0, &allocateInfo.memoryTypeIndex))
    {
        vkDestroyImage(device->device, image, nullptr);
        return nullptr;
    }

    // Allocate memory for the image.
    VkDeviceMemory textureMemory;
    error = vkAllocateMemory(device->device, &allocateInfo, nullptr, &textureMemory);
    if (error)
    {
        vkDestroyImage(device->device, image, nullptr);
        return nullptr;
    }

    error = vkBindImageMemory(device->device, image, textureMemory, 0);
    if (error)
    {
        vkDestroyImage(device->device, image, nullptr);
        vkFreeMemory(device->device, textureMemory, nullptr);
        return nullptr;
    }

    if (initialLayout != createInfo.initialLayout)
    {
        bool success = false;
        if (flags & (AGPU_TEXTURE_FLAG_DEPTH | AGPU_TEXTURE_FLAG_STENCIL))
        {
            VkClearDepthStencilValue clearValue;
            clearValue.depth = 1.0;
            clearValue.stencil = 0;
            success = device->clearImageWithDepthStencil(image, imageAspect, createInfo.initialLayout, initialLayout, VkAccessFlagBits(0), &clearValue);
        }
        else if (flags & AGPU_TEXTURE_FLAG_RENDER_TARGET)
        {
            VkClearColorValue clearValue;
            memset(&clearValue, 0, sizeof(clearValue));
            success = device->clearImageWithColor(image, imageAspect, createInfo.initialLayout, initialLayout, VkAccessFlagBits(0), &clearValue);
        }
        else
        {
            VkClearColorValue clearValue;
            memset(&clearValue, 0, sizeof(clearValue));
            success = device->clearImageWithColor(image, imageAspect, createInfo.initialLayout, initialLayout, VkAccessFlagBits(0), &clearValue);
            //success = device->setImageLayout(image, imageAspect, createInfo.initialLayout, initialLayout, VkAccessFlagBits(0));
        }

        if (!success)
        {
            vkDestroyImage(device->device, image, nullptr);
            vkFreeMemory(device->device, textureMemory, nullptr);
            return nullptr;
        }
    }

    // Create the the buffers
    bool hasUploadBuffer = (description->flags & AGPU_TEXTURE_FLAG_UPLOADED) != 0;
    bool hasReadbackBuffer = (description->flags & AGPU_TEXTURE_FLAG_READED_BACK) != 0;
    VkSubresourceLayout transferLayout;
    memset(&transferLayout, 0, sizeof(transferLayout));

    agpu_buffer *uploadBuffer = nullptr;
    agpu_buffer *readbackBuffer = nullptr;
    if (hasUploadBuffer || hasReadbackBuffer)
    {
        VkImageSubresource subresource;
        subresource.arrayLayer = 0;
        subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresource.mipLevel = 0;
        vkGetImageSubresourceLayout(device->device, image, &subresource, &transferLayout);
        
        agpu_buffer_description bufferDesc;
        memset(&bufferDesc, 0, sizeof(bufferDesc));
        bufferDesc.binding = AGPU_GENERIC_DATA_BUFFER;
        bufferDesc.usage = AGPU_STREAM;
        bufferDesc.stride = pixelSizeOfTextureFormat(description->format);
        bufferDesc.size = transferLayout.size;

        // Create the upload buffer
        bool success = true;
        if (hasUploadBuffer)
        {
            auto uploadDesc = bufferDesc;
            uploadDesc.mapping_flags = AGPU_MAP_WRITE_BIT;
            uploadBuffer = agpuCreateBuffer(device, &uploadDesc, nullptr);
            success = success && (uploadBuffer != nullptr);
        }

        // Create the read back buffer
        if (hasReadbackBuffer)
        {
            auto readbackDesc = bufferDesc;
            readbackDesc.mapping_flags = AGPU_MAP_READ_BIT;
            readbackBuffer = agpuCreateBuffer(device, &readbackDesc, nullptr);
            success = success && (readbackBuffer != nullptr);
        }

        if (!success)
        {
            if (uploadBuffer)
                uploadBuffer->release();
            if (readbackBuffer)
                readbackBuffer->release();
            vkDestroyImage(device->device, image, nullptr);
            vkFreeMemory(device->device, textureMemory, nullptr);
            return nullptr;
        }
    }

    std::unique_ptr<agpu_texture> texture(new agpu_texture(device));
    texture->description = *description;
    texture->image = image;
    texture->memory = textureMemory;
    texture->owned = true;
    texture->transferLayout = transferLayout;
    texture->uploadBuffer = uploadBuffer;
    texture->readbackBuffer = readbackBuffer;
    texture->initialLayout = initialLayout;
    texture->initialLayoutAccessBits = initialLayoutAccessBits;
    return texture.release();
}

agpu_texture *_agpu_texture::createFromImage(agpu_device *device, agpu_texture_description *description, VkImage image)
{
    std::unique_ptr<agpu_texture> texture(new agpu_texture(device));
    texture->description = *description;
    texture->image = image;
    return texture.release();
}

VkImageView _agpu_texture::createImageView(agpu_device *device, agpu_texture_view_description *viewDescription)
{
    if (!viewDescription || !viewDescription->texture)
        return VK_NULL_HANDLE;

    auto texture = viewDescription->texture;
    VkImageViewCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(createInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = texture->image;
    createInfo.viewType = mapImageViewType(viewDescription->type);
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

    if((viewDescription->subresource_range.usage_flags & (AGPU_TEXTURE_FLAG_DEPTH | AGPU_TEXTURE_FLAG_STENCIL)) == 0)
        subresource.aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
    if (viewDescription->subresource_range.usage_flags & AGPU_TEXTURE_FLAG_DEPTH)
        subresource.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
    if (viewDescription->subresource_range.usage_flags & AGPU_TEXTURE_FLAG_STENCIL)
        subresource.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

    VkImageView view;
    auto error = vkCreateImageView(device->device, &createInfo, nullptr, &view);
    if (error)
        return VK_NULL_HANDLE;

    return view;
}

agpu_error _agpu_texture::getFullViewDescription(agpu_texture_view_description *viewDescription)
{
    CHECK_POINTER(viewDescription);
    memset(viewDescription, 0, sizeof(*viewDescription));
    viewDescription->type = description.type;
    viewDescription->texture = this;
    viewDescription->format = description.format;
    viewDescription->components.r = AGPU_COMPONENT_SWIZZLE_R;
    viewDescription->components.g = AGPU_COMPONENT_SWIZZLE_G;
    viewDescription->components.b = AGPU_COMPONENT_SWIZZLE_B;
    viewDescription->components.a = AGPU_COMPONENT_SWIZZLE_A;
    viewDescription->subresource_range.usage_flags = description.flags;
    viewDescription->subresource_range.base_miplevel = 0;
    viewDescription->subresource_range.level_count = description.miplevels;
    viewDescription->subresource_range.base_arraylayer = 0;
    viewDescription->subresource_range.layer_count = description.depthOrArraySize;
    return AGPU_OK;
}

agpu_pointer _agpu_texture::mapLevel(agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags)
{
    return nullptr;
}

agpu_error _agpu_texture::unmapLevel()
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_texture::readData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer)
{
    CHECK_POINTER(buffer);
    if ((description.flags & AGPU_TEXTURE_FLAG_READED_BACK) == 0)
        return AGPU_INVALID_OPERATION;

    VkImageSubresource subresource;
    subresource.arrayLayer = arrayIndex;
    subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource.mipLevel = level;

    VkSubresourceLayout layout;
    vkGetImageSubresourceLayout(device->device, image, &subresource, &layout);

    // Read the render target into the readback buffer.
    VkBufferImageCopy copy;
    memset(&copy, 0, sizeof(copy));
    copy.bufferRowLength = layout.rowPitch / pixelSizeOfTextureFormat(description.format);
    copy.bufferImageHeight = layout.depthPitch / layout.rowPitch;
    copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.imageSubresource.mipLevel = level;
    copy.imageSubresource.baseArrayLayer = arrayIndex;
    copy.imageSubresource.layerCount = 1;
    copy.imageExtent = getLevelExtent(level);
    device->copyImageToBuffer(image, VK_IMAGE_ASPECT_COLOR_BIT, initialLayout, initialLayoutAccessBits, readbackBuffer->uploadBuffer, 1, &copy);

    auto readbackPointer = readbackBuffer->map(AGPU_READ_ONLY);
    if (!readbackPointer)
        return AGPU_ERROR;

    if (pitch == layout.rowPitch && slicePitch == layout.depthPitch)
    {
        memcpy(buffer, readbackPointer, slicePitch);
    }
    else
    {
        auto srcRow = reinterpret_cast<uint8_t*> (readbackPointer);
        auto dstRow = reinterpret_cast<uint8_t*> (buffer);
        for (int y = 0; y < copy.imageExtent.height; ++y)
        {
            memcpy(dstRow, srcRow, pitch);
            srcRow += layout.rowPitch;
            dstRow += pitch;
        }
    }

    readbackBuffer->unmap();

    return AGPU_OK;
}

agpu_error _agpu_texture::uploadData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data)
{
    CHECK_POINTER(data);
    if ((description.flags & AGPU_TEXTURE_FLAG_UPLOADED) == 0)
        return AGPU_INVALID_OPERATION;

    auto bufferPointer = uploadBuffer->map(AGPU_WRITE_ONLY);
    if (!bufferPointer)
        return AGPU_ERROR;
    
    VkImageSubresource subresource;
    subresource.arrayLayer = arrayIndex;
    subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource.mipLevel = level;

    VkSubresourceLayout layout;
    vkGetImageSubresourceLayout(device->device, image, &subresource, &layout);

    auto copyHeight = layout.depthPitch / layout.rowPitch;
    if (pitch == layout.rowPitch && slicePitch == layout.depthPitch)
    {
        memcpy(bufferPointer, data, slicePitch);
    }
    else
    {
        auto srcRow = reinterpret_cast<uint8_t*> (data);
        auto dstRow = reinterpret_cast<uint8_t*> (bufferPointer);
        for (int y = 0; y < copyHeight; ++y)
        {
            memcpy(dstRow, srcRow, pitch);
            srcRow += pitch;
            dstRow += layout.rowPitch;
        }
    }

    uploadBuffer->unmap();

    VkBufferImageCopy copy;
    memset(&copy, 0, sizeof(copy));
    copy.bufferRowLength = layout.rowPitch / pixelSizeOfTextureFormat(description.format);
    copy.bufferImageHeight = copyHeight;
    copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.imageSubresource.mipLevel = level;
    copy.imageSubresource.baseArrayLayer = arrayIndex;
    copy.imageSubresource.layerCount = 1;
    copy.imageExtent = getLevelExtent(level);
    device->copyBufferToImage(uploadBuffer->uploadBuffer, image, VK_IMAGE_ASPECT_COLOR_BIT, initialLayout, initialLayoutAccessBits, 1, &copy);

    return AGPU_OK;
}

agpu_error _agpu_texture::discardUploadBuffer()
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_texture::discardReadbackBuffer()
{
    return AGPU_UNIMPLEMENTED;
}

// The exported C interface
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
    CHECK_POINTER(description);
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
    return texture->readData(level, arrayIndex, pitch, slicePitch, buffer);
}

AGPU_EXPORT agpu_error agpuUploadTextureData(agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data)
{
    CHECK_POINTER(texture);
    return texture->uploadData(level, arrayIndex, pitch, slicePitch, data);
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

AGPU_EXPORT agpu_error agpuGetTextureFullViewDescription(agpu_texture* texture, agpu_texture_view_description *description)
{
    CHECK_POINTER(texture);
    return texture->getFullViewDescription(description);
}
