#include "shader_resource_binding.hpp"
#include "shader_signature.hpp"
#include "buffer.hpp"
#include "texture.hpp"

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
    result->type = elementDescription.type;
    result->bindingPointCount = elementDescription.bindingPointCount;
    result->vulkanDescriptorType = elementDescription.vulkanDescriptionType;
    return result;
}

agpu_error _agpu_shader_resource_binding::bindUniformBuffer(agpu_int location, agpu_buffer* uniform_buffer)
{
    if (type != AGPU_SHADER_BINDING_TYPE_CBV)
        return AGPU_INVALID_OPERATION;

    CHECK_POINTER(uniform_buffer);
    if (location < 0 || location >= bindingPointCount)
        return AGPU_OUT_OF_BOUNDS;

    return bindUniformBufferRange(location, uniform_buffer, 0, uniform_buffer->description.size);
}

agpu_error _agpu_shader_resource_binding::bindUniformBufferRange(agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size)
{
    CHECK_POINTER(uniform_buffer);
    if (type != AGPU_SHADER_BINDING_TYPE_CBV)
        return AGPU_INVALID_OPERATION;
    if (uniform_buffer->description.binding != AGPU_UNIFORM_BUFFER)
        return AGPU_INVALID_PARAMETER;
    if (location < 0 || location >= bindingPointCount)
        return AGPU_OUT_OF_BOUNDS;

    // Align the size to 256 Kb
    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = uniform_buffer->getDrawBuffer();
    bufferInfo.offset = offset;
    bufferInfo.range = (size + 255) & (~255);

    VkWriteDescriptorSet write;
    memset(&write, 0, sizeof(VkWriteDescriptorSet));
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorCount = 1;
    write.descriptorType = vulkanDescriptorType;
    write.dstSet = descriptorSet;
    write.dstBinding = location;
    write.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(device->device, 1, &write, 0, nullptr);

    return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::bindTexture(agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp)
{
    CHECK_POINTER(texture);
    if (type != AGPU_SHADER_BINDING_TYPE_SRV)
        return AGPU_INVALID_OPERATION;

    if (location < 0 || location >= bindingPointCount)
        return AGPU_OUT_OF_BOUNDS;
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_shader_resource_binding::bindTextureArrayRange(agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp)
{
    CHECK_POINTER(texture);
    if (type != AGPU_SHADER_BINDING_TYPE_SRV)
        return AGPU_INVALID_OPERATION;
    if (location < 0 || location >= bindingPointCount)
        return AGPU_OUT_OF_BOUNDS;
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_shader_resource_binding::createSampler(agpu_int location, agpu_sampler_description* description)
{
    CHECK_POINTER(description);
    if (type != AGPU_SHADER_BINDING_TYPE_SAMPLER)
        return AGPU_INVALID_OPERATION;
    if (location < 0 || location >= bindingPointCount)
        return AGPU_OUT_OF_BOUNDS;
    return AGPU_UNIMPLEMENTED;
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
