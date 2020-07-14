#ifndef AGPU_VULKAN_SHADER_SIGNATURE_HPP
#define AGPU_VULKAN_SHADER_SIGNATURE_HPP

#include "device.hpp"
#include "shader_signature_builder.hpp"
#include "descriptor_pool.hpp"

namespace AgpuVulkan
{

struct AVkShaderSignature : public agpu::shader_signature
{
public:
    AVkShaderSignature(const agpu::device_ref &device);
    ~AVkShaderSignature();

    static agpu::shader_signature_ref create(const agpu::device_ref &device, AVkShaderSignatureBuilder *builder, VkPipelineLayout layout);

    virtual agpu::shader_resource_binding_ptr createShaderResourceBinding(agpu_uint element) override;

    agpu::device_ref device;
    VkPipelineLayout layout;

    std::vector<AVkDescriptorSetPoolPtr> descriptorPools;
};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_SHADER_SIGNATURE_HPP
