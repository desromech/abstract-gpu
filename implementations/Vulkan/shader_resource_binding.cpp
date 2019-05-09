#include "shader_resource_binding.hpp"
#include "shader_signature.hpp"
#include "buffer.hpp"
#include "texture.hpp"
#include "constants.hpp"

namespace AgpuVulkan
{

AVkShaderResourceBinding::AVkShaderResourceBinding(const agpu::device_ref &device)
    : device(device)
{
}

AVkShaderResourceBinding::~AVkShaderResourceBinding()
{
}

agpu::shader_resource_binding_ref AVkShaderResourceBinding::create(const agpu::device_ref &device, const agpu::shader_signature_ref &signature, agpu_uint elementIndex, VkDescriptorSet descriptorSet, const ShaderSignatureElementDescription &elementDescription)
{
    auto result = agpu::makeObject<AVkShaderResourceBinding> (device);
    auto resourceBinding = result.as<AVkShaderResourceBinding> ();
    resourceBinding->elementIndex = elementIndex;
    resourceBinding->signature = signature;
    resourceBinding->descriptorSet = descriptorSet;
    resourceBinding->bindingDescription = &elementDescription;
    return result;
}

agpu_error AVkShaderResourceBinding::bindUniformBuffer(agpu_int location, const agpu::buffer_ref &uniform_buffer)
{
    CHECK_POINTER(uniform_buffer);
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;

    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER)
        return AGPU_INVALID_OPERATION;

    return bindUniformBufferRange(location, uniform_buffer, 0, uniform_buffer.as<AVkBuffer> ()->description.size);
}

agpu_error AVkShaderResourceBinding::bindUniformBufferRange(agpu_int location, const agpu::buffer_ref &uniform_buffer, agpu_size offset, agpu_size size)
{
    CHECK_POINTER(uniform_buffer);

    if ((uniform_buffer.as<AVkBuffer> ()->description.binding & AGPU_UNIFORM_BUFFER) == 0)
        return AGPU_INVALID_PARAMETER;
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;
    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER)
        return AGPU_INVALID_OPERATION;

    // Align the size to 256 Kb
    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = uniform_buffer.as<AVkBuffer> ()->getDrawBuffer();
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

    vkUpdateDescriptorSets(deviceForVk->device, 1, &write, 0, nullptr);

    return AGPU_OK;
}

agpu_error AVkShaderResourceBinding::bindStorageBuffer(agpu_int location, const agpu::buffer_ref &storage_buffer)
{
    CHECK_POINTER(storage_buffer);
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;

    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER)
        return AGPU_INVALID_OPERATION;

    return bindStorageBufferRange(location, storage_buffer, 0, storage_buffer.as<AVkBuffer> ()->description.size);
}

agpu_error AVkShaderResourceBinding::bindStorageBufferRange(agpu_int location, const agpu::buffer_ref &storage_buffer, agpu_size offset, agpu_size size)
{
    CHECK_POINTER(storage_buffer);

    if ((storage_buffer.as<AVkBuffer> ()->description.binding & AGPU_STORAGE_BUFFER) == 0)
        return AGPU_INVALID_PARAMETER;
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;
    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER)
        return AGPU_INVALID_OPERATION;

    // Align the size to 256 Kb
    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = storage_buffer.as<AVkBuffer> ()->getDrawBuffer();
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

    vkUpdateDescriptorSets(deviceForVk->device, 1, &write, 0, nullptr);

    return AGPU_OK;
}

agpu_error AVkShaderResourceBinding::bindTexture(agpu_int location, const agpu::texture_ref &texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp)
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

    auto view = AVkTexture::createImageView(device, &viewDesc);
    if (!view)
        return AGPU_ERROR;

    VkDescriptorImageInfo imageInfo;
    imageInfo.imageLayout = texture.as<AVkTexture> ()->initialLayout;
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

    vkUpdateDescriptorSets(deviceForVk->device, 1, &write, 0, nullptr);

    return AGPU_OK;
}

agpu_error AVkShaderResourceBinding::bindTextureArrayRange(agpu_int location, const agpu::texture_ref &texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp)
{
    CHECK_POINTER(texture);
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;
    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE)
        return AGPU_INVALID_OPERATION;

    return AGPU_UNIMPLEMENTED;
}

agpu_error AVkShaderResourceBinding::bindImage(agpu_int location, const agpu::texture_ref &texture, agpu_int level, agpu_int layer, agpu_mapping_access access, agpu_texture_format format)
{
    CHECK_POINTER(texture);
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;

    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE)
        return AGPU_INVALID_OPERATION;

    agpu_texture_view_description viewDesc;
    auto myError = texture->getFullViewDescription(&viewDesc);
    if (myError != AGPU_OK)
        return myError;

    viewDesc.subresource_range.base_miplevel = level;
    viewDesc.subresource_range.level_count = 1;
    if(layer >= 0)
    {
        viewDesc.subresource_range.base_arraylayer = layer;
        viewDesc.subresource_range.layer_count = 0;
    }

    auto view = AVkTexture::createImageView(device, &viewDesc);
    if (!view)
        return AGPU_ERROR;

    VkDescriptorImageInfo imageInfo;
    imageInfo.imageLayout = texture.as<AVkTexture> ()->initialLayout;
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

    vkUpdateDescriptorSets(deviceForVk->device, 1, &write, 0, nullptr);
    return AGPU_OK;
}

agpu_error AVkShaderResourceBinding::createSampler(agpu_int location, agpu_sampler_description* description)
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

    info.compareEnable = description->comparison_enabled ? VK_TRUE : VK_FALSE;
    info.compareOp = mapCompareFunction(description->comparison_function);
    /*
    TODO:
    agpu_color4f border_color;
    */

    VkSampler sampler;
    auto error = vkCreateSampler(deviceForVk->device, &info, nullptr, &sampler);
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

    vkUpdateDescriptorSets(deviceForVk->device, 1, &write, 0, nullptr);
    return AGPU_OK;
}

} // End of namespace AgpuVulkan
