#ifndef AGPU_VULKAN_SHADER_RESOURCE_BINDING_HPP
#define AGPU_VULKAN_SHADER_RESOURCE_BINDING_HPP

#include "device.hpp"
#include "shader_signature_builder.hpp"

namespace AgpuVulkan
{

class AVkShaderResourceBinding : public agpu::shader_resource_binding
{
public:
    AVkShaderResourceBinding(const agpu::device_ref &device);
    ~AVkShaderResourceBinding();

    static agpu::shader_resource_binding_ref create(const agpu::device_ref &device, const agpu::shader_signature_ref &signature, agpu_uint elementIndex, VkDescriptorSet descriptorSet, const ShaderSignatureElementDescription &elementDescription);

    virtual agpu_error bindUniformBuffer(agpu_int location, const agpu::buffer_ref &uniform_buffer) override;
    virtual agpu_error bindUniformBufferRange(agpu_int location, const agpu::buffer_ref &uniform_buffer, agpu_size offset, agpu_size size) override;
    virtual agpu_error bindStorageBuffer(agpu_int location, const agpu::buffer_ref &storage_buffer) override;
    virtual agpu_error bindStorageBufferRange(agpu_int location, const agpu::buffer_ref &storage_buffer, agpu_size offset, agpu_size size) override;
    virtual agpu_error bindTexture(agpu_int location, const agpu::texture_ref &texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp) override;
    virtual agpu_error bindTextureArrayRange(agpu_int location, const agpu::texture_ref &texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp) override;
    virtual agpu_error bindImage(agpu_int location, const agpu::texture_ref &texture, agpu_int level, agpu_int layer, agpu_mapping_access access, agpu_texture_format format) override;
    virtual agpu_error createSampler(agpu_int location, agpu_sampler_description* description) override;

    agpu::device_ref device;
    agpu::shader_signature_ref signature;
    agpu_uint elementIndex;
    VkDescriptorSet descriptorSet;
    const ShaderSignatureElementDescription *bindingDescription;
};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_SHADER_RESOURCE_BINDING_HPP
