#include "shader_resource_binding.hpp"
#include "shader_signature.hpp"
#include "buffer.hpp"
#include "texture.hpp"

inline VkFilter mapMinFilter(agpu_filter filter)
{
    switch (filter)
    {
    default:
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_NEAREST:return VK_FILTER_NEAREST;
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_LINEAR: return VK_FILTER_NEAREST;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_NEAREST: return VK_FILTER_NEAREST;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_LINEAR:  return VK_FILTER_NEAREST;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_NEAREST: return VK_FILTER_LINEAR;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_LINEAR:  return VK_FILTER_LINEAR;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST:  return VK_FILTER_LINEAR;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR:   return VK_FILTER_LINEAR;
    case AGPU_FILTER_ANISOTROPIC:                           return VK_FILTER_LINEAR;
    }
}

inline VkFilter mapMagFilter(agpu_filter filter)
{
    switch (filter)
    {
    default:
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_NEAREST: return VK_FILTER_NEAREST;
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_LINEAR:  return VK_FILTER_NEAREST;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_NEAREST:  return VK_FILTER_LINEAR;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_LINEAR:   return VK_FILTER_LINEAR;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_NEAREST:  return VK_FILTER_NEAREST;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_LINEAR:   return VK_FILTER_NEAREST;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST:   return VK_FILTER_LINEAR;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR:    return VK_FILTER_LINEAR;
    case AGPU_FILTER_ANISOTROPIC:                            return VK_FILTER_LINEAR;
    }
}

inline VkSamplerMipmapMode mapMipmapMode(agpu_filter filter)
{
    switch (filter)
    {
    default:
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_NEAREST: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_LINEAR:  return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_NEAREST:  return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_LINEAR:   return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_NEAREST:  return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_LINEAR:   return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST:   return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR:    return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    case AGPU_FILTER_ANISOTROPIC:                            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

inline VkSamplerAddressMode mapAddressMode(agpu_texture_address_mode mode)
{
    switch (mode)
    {
    default:
    case AGPU_TEXTURE_ADDRESS_MODE_WRAP:    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case AGPU_TEXTURE_ADDRESS_MODE_MIRROR:  return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case AGPU_TEXTURE_ADDRESS_MODE_CLAMP:   return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case AGPU_TEXTURE_ADDRESS_MODE_BORDER:  return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    case AGPU_TEXTURE_ADDRESS_MODE_MIRROR_ONCE: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    }
}

_agpu_shader_resource_binding::_agpu_shader_resource_binding(agpu_device *device)
    : device(device)
{
}

void _agpu_shader_resource_binding::lostReferences()
{
    signature->release();
}

_agpu_shader_resource_binding *_agpu_shader_resource_binding::create(agpu_device *device, agpu_shader_signature *signature, agpu_uint elementIndex, VkDescriptorSet descriptorSet, const ShaderSignatureElementDescription &elementDescription)
{
    auto result = new _agpu_shader_resource_binding(device);
    result->elementIndex = elementIndex;
    result->signature = signature;
    signature->retain();
    result->descriptorSet = descriptorSet;
    result->bindingDescription = &elementDescription;
    return result;
}

agpu_error _agpu_shader_resource_binding::bindUniformBuffer(agpu_int location, agpu_buffer* uniform_buffer)
{
    CHECK_POINTER(uniform_buffer);
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;

    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER)
        return AGPU_INVALID_OPERATION;

    return bindUniformBufferRange(location, uniform_buffer, 0, uniform_buffer->description.size);
}

agpu_error _agpu_shader_resource_binding::bindUniformBufferRange(agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size)
{
    CHECK_POINTER(uniform_buffer);

    if (uniform_buffer->description.binding != AGPU_UNIFORM_BUFFER)
        return AGPU_INVALID_PARAMETER;
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;
    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER)
        return AGPU_INVALID_OPERATION;

    // Align the size to 256 Kb
    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = uniform_buffer->getDrawBuffer();
    bufferInfo.offset = offset;
    bufferInfo.range = (size + 255) & (~255);

    VkWriteDescriptorSet write;
    memset(&write, 0, sizeof(VkWriteDescriptorSet));
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorCount = 1;
    write.descriptorType = bindingDescription->bindings[location].descriptorType;
    write.dstSet = descriptorSet;
    write.dstBinding = location;
    write.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(device->device, 1, &write, 0, nullptr);

    return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::bindStorageBuffer(agpu_int location, agpu_buffer* storage_buffer)
{
    CHECK_POINTER(storage_buffer);
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;

    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER)
        return AGPU_INVALID_OPERATION;

    return bindUniformBufferRange(location, storage_buffer, 0, storage_buffer->description.size);
}

agpu_error _agpu_shader_resource_binding::bindStorageBufferRange(agpu_int location, agpu_buffer* storage_buffer, agpu_size offset, agpu_size size)
{
    CHECK_POINTER(storage_buffer);

    if (storage_buffer->description.binding != AGPU_STORAGE_BUFFER)
        return AGPU_INVALID_PARAMETER;
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;
    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER)
        return AGPU_INVALID_OPERATION;

    // Align the size to 256 Kb
    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = storage_buffer->getDrawBuffer();
    bufferInfo.offset = offset;
    bufferInfo.range = (size + 255) & (~255);

    VkWriteDescriptorSet write;
    memset(&write, 0, sizeof(VkWriteDescriptorSet));
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorCount = 1;
    write.descriptorType = bindingDescription->bindings[location].descriptorType;
    write.dstSet = descriptorSet;
    write.dstBinding = location;
    write.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(device->device, 1, &write, 0, nullptr);

    return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::bindTexture(agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp)
{
    CHECK_POINTER(texture);
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;

    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE)
        return AGPU_INVALID_OPERATION;

    agpu_texture_view_description viewDesc;
    auto myError = texture->getFullViewDescription(&viewDesc);
    if (myError != AGPU_OK)
        return myError;

    viewDesc.subresource_range.base_miplevel = startMiplevel;
    if(miplevels >= 0)
        viewDesc.subresource_range.level_count = miplevels;

    auto view = agpu_texture::createImageView(device, &viewDesc);
    if (!view)
        return AGPU_ERROR;

    VkDescriptorImageInfo imageInfo;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageInfo.imageView = view;
    imageInfo.sampler = VK_NULL_HANDLE;

    VkWriteDescriptorSet write;
    memset(&write, 0, sizeof(VkWriteDescriptorSet));
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorCount = 1;
    write.descriptorType = bindingDescription->bindings[location].descriptorType;
    write.dstSet = descriptorSet;
    write.dstBinding = location;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device->device, 1, &write, 0, nullptr);

    return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::bindTextureArrayRange(agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp)
{
    CHECK_POINTER(texture);
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;
    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE)
        return AGPU_INVALID_OPERATION;

    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_shader_resource_binding::createSampler(agpu_int location, agpu_sampler_description* description)
{
    CHECK_POINTER(description);
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;
    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_SAMPLER)
        return AGPU_INVALID_OPERATION;

    VkSamplerCreateInfo info;
    memset(&info, 0, sizeof(info));
    info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    info.minFilter = mapMinFilter(description->filter);
    info.magFilter = mapMagFilter(description->filter);
    info.mipmapMode = mapMipmapMode(description->filter);
    info.addressModeU = mapAddressMode(description->address_u);
    info.addressModeV = mapAddressMode(description->address_v);
    info.addressModeW = mapAddressMode(description->address_w);
    info.minLod = description->min_lod;
    info.maxLod = description->max_lod;
    info.anisotropyEnable = description->filter == AGPU_FILTER_ANISOTROPIC;
    info.maxAnisotropy = description->maxanisotropy;
    info.mipLodBias = description->mip_lod_bias;

    /*
    agpu_compare_function comparison_function;
    agpu_color4f border_color;
    */

    VkSampler sampler;
    auto error = vkCreateSampler(device->device, &info, nullptr, &sampler);
    CONVERT_VULKAN_ERROR(error);

    VkDescriptorImageInfo imageInfo;
    memset(&imageInfo, 0, sizeof(imageInfo));
    imageInfo.imageView = VK_NULL_HANDLE;
    imageInfo.sampler = sampler;

    VkWriteDescriptorSet write;
    memset(&write, 0, sizeof(VkWriteDescriptorSet));
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorCount = 1;
    write.descriptorType = bindingDescription->bindings[location].descriptorType;
    write.dstSet = descriptorSet;
    write.dstBinding = location;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device->device, 1, &write, 0, nullptr);
    return AGPU_OK;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddShaderResourceBindingReference(agpu_shader_resource_binding* shader_resource_binding)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->retain();
}

AGPU_EXPORT agpu_error agpuReleaseShaderResourceBinding(agpu_shader_resource_binding* shader_resource_binding)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->release();
}

AGPU_EXPORT agpu_error agpuBindUniformBuffer(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindUniformBuffer(location, uniform_buffer);
}

AGPU_EXPORT agpu_error agpuBindUniformBufferRange(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindUniformBufferRange(location, uniform_buffer, offset, size);
}

AGPU_EXPORT agpu_error agpuBindStorageBuffer(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* storage_buffer)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindStorageBuffer(location, storage_buffer);
}

AGPU_EXPORT agpu_error agpuBindStorageBufferRange(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* storage_buffer, agpu_size offset, agpu_size size)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindStorageBufferRange(location, storage_buffer, offset, size);
}

AGPU_EXPORT agpu_error agpuBindTexture(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindTexture(location, texture, startMiplevel, miplevels, lodclamp);
}

AGPU_EXPORT agpu_error agpuBindTextureArrayRange(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindTextureArrayRange(location, texture, startMiplevel, miplevels, firstElement, numberOfElements, lodclamp);
}

AGPU_EXPORT agpu_error agpuCreateSampler(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_sampler_description* description)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->createSampler(location, description);
}
