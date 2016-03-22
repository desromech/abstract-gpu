#ifndef AGPU_VULKAN_SHADER_SIGNATURE_HPP
#define AGPU_VULKAN_SHADER_SIGNATURE_HPP

#include "device.hpp"
#include "shader_signature_builder.hpp"

struct _agpu_shader_signature : public Object<_agpu_shader_signature>
{
    _agpu_shader_signature(agpu_device *device);
    void lostReferences();

    static agpu_shader_signature *create(agpu_device *device, agpu_shader_signature_builder *builder, VkPipelineLayout layout);

    agpu_shader_resource_binding* createShaderResourceBinding(agpu_uint element);

    agpu_device *device;
    agpu_shader_signature_builder *builder;
    VkPipelineLayout layout;

    std::vector<ShaderSignatureElementDescription> elementDescription;
    std::vector<VkDescriptorPool> elementPools;
};

#endif //AGPU_VULKAN_SHADER_SIGNATURE_HPP
