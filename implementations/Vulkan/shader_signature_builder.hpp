#ifndef AGPU_VULKAN_SHADER_SIGNATURE_BUILDER_HPP
#define AGPU_VULKAN_SHADER_SIGNATURE_BUILDER_HPP

#include "device.hpp"

namespace AgpuVulkan
{

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

class AVkShaderSignatureBuilder : public agpu::shader_signature_builder
{
public:
    AVkShaderSignatureBuilder(const agpu::device_ref &device);
    ~AVkShaderSignatureBuilder();

    static agpu::shader_signature_builder_ref create(const agpu::device_ref &device);

    virtual agpu::shader_signature_ptr build() override;
    virtual agpu_error addBindingConstant() override;
    virtual agpu_error addBindingElement(agpu_shader_binding_type type, agpu_uint maxBindings) override;
    virtual agpu_error beginBindingBank(agpu_uint maxBindings) override;
    virtual agpu_error addBindingBankElement(agpu_shader_binding_type type, agpu_uint bindingPointCount) override;

    agpu_error finishBindingBank();

    agpu::device_ref device;

    std::vector<VkPushConstantRange> pushConstantRanges;
    std::vector<ShaderSignatureElementDescription> elementDescription;
    ShaderSignatureElementDescription *currentElementSet;
};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_SHADER_SIGNATURE_BUILDER_HPP
