#ifndef AGPU_VULKAN_SHADER_SIGNATURE_BUILDER_HPP
#define AGPU_VULKAN_SHADER_SIGNATURE_BUILDER_HPP

#include "device.hpp"

struct ShaderSignatureElementDescription
{
    ShaderSignatureElementDescription() {}
    ShaderSignatureElementDescription(bool bank, agpu_uint maxBindings)
        : valid(true), bank(bank), maxBindings(maxBindings), descriptorSetLayout(VK_NULL_HANDLE) {}

    bool valid;
    bool bank;
    agpu_uint maxBindings;
    VkDescriptorSetLayout descriptorSetLayout;

    std::vector<agpu_shader_binding_type> types;
    std::vector<VkDescriptorSetLayoutBinding> bindings;

};

struct _agpu_shader_signature_builder : public Object<_agpu_shader_signature_builder>
{
    _agpu_shader_signature_builder(agpu_device *device);
    void lostReferences();

    static _agpu_shader_signature_builder *create(agpu_device *device);

    agpu_shader_signature* buildShaderSignature();
    agpu_error addBindingConstant();
    agpu_error addBindingElement(agpu_shader_binding_type type, agpu_uint maxBindings);
    agpu_error beginBindingBank ( agpu_uint maxBindings );
    agpu_error addBindingBankElement ( agpu_shader_binding_type type, agpu_uint bindingPointCount );

    agpu_error finishBindingBank();

    agpu_device *device;

    std::vector<VkPushConstantRange> pushConstantRanges;
    std::vector<ShaderSignatureElementDescription> elementDescription;
    ShaderSignatureElementDescription *currentElementSet;
};

#endif //AGPU_VULKAN_SHADER_SIGNATURE_BUILDER_HPP
