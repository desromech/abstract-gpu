#ifndef AGPU_VULKAN_SHADER_SIGNATURE_BUILDER_HPP
#define AGPU_VULKAN_SHADER_SIGNATURE_BUILDER_HPP

#include "device.hpp"

struct ShaderSignatureElementDescription
{
    ShaderSignatureElementDescription() {}
    ShaderSignatureElementDescription(bool bank, agpu_shader_binding_type type, VkDescriptorType vulkanDescriptionType, agpu_uint bindingPointCount, agpu_uint maxBindings)
        : valid(true), bank(bank), type(type), vulkanDescriptionType(vulkanDescriptionType), bindingPointCount(bindingPointCount), maxBindings(maxBindings) {}

    bool valid;
    bool bank;
    agpu_shader_binding_type type;
    VkDescriptorType vulkanDescriptionType;
    agpu_uint bindingPointCount;
    agpu_uint maxBindings;
};

struct _agpu_shader_signature_builder : public Object<_agpu_shader_signature_builder>
{
    _agpu_shader_signature_builder(agpu_device *device);
    void lostReferences();

    static _agpu_shader_signature_builder *create(agpu_device *device);

    agpu_shader_signature* buildShaderSignature();
    agpu_error addBindingConstant();
    agpu_error addBindingElement(agpu_shader_binding_type type, agpu_uint maxBindings);
    agpu_error addBindingBank(agpu_shader_binding_type type, agpu_uint bindingPointCount, agpu_uint maxBindings);

    agpu_device *device;

    std::vector<VkDescriptorSetLayout> descriptorSets;
    std::vector<VkPushConstantRange> pushConstantRanges;
    std::vector<ShaderSignatureElementDescription> elementDescription;
    agpu_uint maxBindingsCount[AGPU_SHADER_BINDING_TYPE_COUNT];
};

#endif //AGPU_VULKAN_SHADER_SIGNATURE_BUILDER_HPP
