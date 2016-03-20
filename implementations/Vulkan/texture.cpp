#include "texture.hpp"
#include "texture_format.hpp"

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
    image = nullptr;
    owned = false;
}

void _agpu_texture::lostReferences()
{
    if (!owned)
        return;

    if(image)
        vkDestroyImage(device->device, image, nullptr);
    if(memory)
        vkFreeMemory(device->device, memory, nullptr);
}

agpu_texture *_agpu_texture::create(agpu_device *device, agpu_texture_description *description)
{
    if (!description)
        return nullptr;

    uint32_t depth = 1;
    uint32_t arrayLayers = description->depthOrArraySize;
    if (description->type = AGPU_TEXTURE_3D)
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
    
    auto flags = description->flags;
    if (flags & (AGPU_TEXTURE_FLAG_DEPTH | AGPU_TEXTURE_FLAG_STENCIL))
        createInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    if (flags & AGPU_TEXTURE_FLAG_RENDER_TARGET)
        createInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (flags & AGPU_TEXTURE_FLAG_READED_BACK)
        createInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (flags & AGPU_TEXTURE_FLAG_UPLOADED)
        createInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

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

    std::unique_ptr<agpu_texture> texture(new agpu_texture(device));
    texture->description = *description;
    texture->image = image;
    texture->memory = textureMemory;
    texture->owned = true;
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
        return nullptr;

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

    if(viewDescription->subresource_range.usage_flags & AGPU_TEXTURE_FLAG_RENDER_TARGET)
        subresource.aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
    if (viewDescription->subresource_range.usage_flags & AGPU_TEXTURE_FLAG_DEPTH)
        subresource.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
    if (viewDescription->subresource_range.usage_flags & AGPU_TEXTURE_FLAG_STENCIL)
        subresource.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

    VkImageView view;
    auto error = vkCreateImageView(device->device, &createInfo, nullptr, &view);
    if (error)
        return nullptr;

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
    return nullptr;
}

AGPU_EXPORT agpu_error agpuUnmapTextureLevel(agpu_texture* texture)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReadTextureData(agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuUploadTextureData(agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuDiscardTextureUploadBuffer(agpu_texture* texture)
{
    return AGPU_UNSUPPORTED;
}

AGPU_EXPORT agpu_error agpuDiscardTextureReadbackBuffer(agpu_texture* texture)
{
    return AGPU_UNSUPPORTED;
}

AGPU_EXPORT agpu_error agpuGetTextureFullViewDescription(agpu_texture* texture, agpu_texture_view_description *description)
{
    CHECK_POINTER(texture);
    return texture->getFullViewDescription(description);
}
