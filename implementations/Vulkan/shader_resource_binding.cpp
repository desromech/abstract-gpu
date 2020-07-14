#include "shader_resource_binding.hpp"
#include "shader_signature.hpp"
#include "buffer.hpp"
#include "texture.hpp"
#include "constants.hpp"
#include "texture_view.hpp"
#include "sampler.hpp"

namespace AgpuVulkan
{

AVkShaderResourceBinding::AVkShaderResourceBinding(const agpu::device_ref &device)
    : device(device)
{
}

AVkShaderResourceBinding::~AVkShaderResourceBinding()
{
    if(descriptorSetPool && descriptorSetAllocation)
    {
        descriptorSetPool->free(descriptorSetAllocation);
        descriptorSetAllocation = nullptr;        
    }
}

agpu::shader_resource_binding_ref AVkShaderResourceBinding::create(const agpu::device_ref &device, const agpu::shader_signature_ref &signature, agpu_uint elementIndex,
    const AVkDescriptorSetPoolPtr &descriptorSetPool,
    AVkDescriptorSetPoolAllocation *descriptorSetAllocation)
{
    auto result = agpu::makeObject<AVkShaderResourceBinding> (device);
    auto resourceBinding = result.as<AVkShaderResourceBinding> ();
    resourceBinding->elementIndex = elementIndex;
    resourceBinding->signature = signature;
    resourceBinding->descriptorSet = descriptorSetAllocation->descriptorSet;
    resourceBinding->descriptorSetAllocation = descriptorSetAllocation;
    resourceBinding->descriptorSetPool = descriptorSetPool;
    resourceBinding->bindingDescription = &descriptorSetPool->setDescription;
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

    if ((uniform_buffer.as<AVkBuffer> ()->description.usage_modes & AGPU_UNIFORM_BUFFER) == 0)
        return AGPU_INVALID_PARAMETER;
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;
    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER)
        return AGPU_INVALID_OPERATION;

    // Align the size to 256 Kb
    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = uniform_buffer.as<AVkBuffer> ()->handle;
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

    if ((storage_buffer.as<AVkBuffer> ()->description.usage_modes & AGPU_STORAGE_BUFFER) == 0)
        return AGPU_INVALID_PARAMETER;
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;
    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER)
        return AGPU_INVALID_OPERATION;

    // Align the size to 256 Kb
    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = storage_buffer.as<AVkBuffer> ()->handle;
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


agpu_error AVkShaderResourceBinding::bindSampledTextureView(agpu_int location, const agpu::texture_view_ref &view)
{
    CHECK_POINTER(view);
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;

    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE)
        return AGPU_INVALID_OPERATION;

    auto avkView = view.as<AVkTextureView> ();

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = avkView->imageLayout;
    imageInfo.imageView = avkView->handle;
    imageInfo.sampler = VK_NULL_HANDLE;

    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorCount = 1;
    write.descriptorType = bindingDescription->bindings[location].descriptorType;
    write.dstSet = descriptorSet;
    write.dstBinding = location;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(deviceForVk->device, 1, &write, 0, nullptr);
    return AGPU_OK;
}

agpu_error AVkShaderResourceBinding::bindStorageImageView(agpu_int location, const agpu::texture_view_ref &view)
{
    CHECK_POINTER(view);
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;

    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE)
        return AGPU_INVALID_OPERATION;

    auto avkView = view.as<AVkTextureView> ();

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = avkView->imageLayout;
    imageInfo.imageView = avkView->handle;
    imageInfo.sampler = VK_NULL_HANDLE;

    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorCount = 1;
    write.descriptorType = bindingDescription->bindings[location].descriptorType;
    write.dstSet = descriptorSet;
    write.dstBinding = location;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(deviceForVk->device, 1, &write, 0, nullptr);
    return AGPU_OK;
}

agpu_error AVkShaderResourceBinding::bindSampler(agpu_int location, const agpu::sampler_ref &sampler)
{
    CHECK_POINTER(sampler);
    if (location < 0 || location >= (int)bindingDescription->types.size())
        return AGPU_OUT_OF_BOUNDS;

    if (bindingDescription->types[location] != AGPU_SHADER_BINDING_TYPE_SAMPLER)
        return AGPU_INVALID_OPERATION;

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageView = VK_NULL_HANDLE;
    imageInfo.sampler = sampler.as<AVkSampler> ()->handle;

    VkWriteDescriptorSet write = {};
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
