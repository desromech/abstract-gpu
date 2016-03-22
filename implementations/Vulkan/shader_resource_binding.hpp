#ifndef AGPU_VULKAN_SHADER_RESOURCE_BINDING_HPP
#define AGPU_VULKAN_SHADER_RESOURCE_BINDING_HPP

#include "device.hpp"
#include "shader_signature_builder.hpp"

struct _agpu_shader_resource_binding : public Object<_agpu_shader_resource_binding>
{
    _agpu_shader_resource_binding(agpu_device *device);
    void lostReferences();

    static _agpu_shader_resource_binding *create(agpu_device *device, agpu_shader_signature *signature, agpu_uint elementIndex, VkDescriptorSet descriptorSet, const ShaderSignatureElementDescription &elementDescription);

    agpu_error bindUniformBuffer(agpu_int location, agpu_buffer* uniform_buffer);
    agpu_error bindUniformBufferRange(agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size);
    agpu_error bindTexture(agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp);
    agpu_error bindTextureArrayRange(agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp);
    agpu_error createSampler(agpu_int location, agpu_sampler_description* description);

    agpu_device *device;
    agpu_shader_signature *signature;
    agpu_uint elementIndex;
    agpu_int bindingPointCount;
    VkDescriptorSet descriptorSet;
    agpu_shader_binding_type type;
    VkDescriptorType vulkanDescriptorType;
};

#endif //AGPU_VULKAN_SHADER_RESOURCE_BINDING_HPP
