#ifndef AGPU_VULKAN_COMPUTE_PIPELINE_BUILDER_HPP
#define AGPU_VULKAN_COMPUTE_PIPELINE_BUILDER_HPP

#include "device.hpp"
#include <string>

namespace AgpuVulkan
{

struct AVkComputePipelineBuilder : public agpu::compute_pipeline_builder
{
public:
    AVkComputePipelineBuilder(const agpu::device_ref &device);
    ~AVkComputePipelineBuilder();

    static agpu::compute_pipeline_builder_ref create(const agpu::device_ref &device);

    virtual agpu::pipeline_state_ptr build() override;
    virtual agpu_error attachShader(const agpu::shader_ref &newShader) override;
    virtual agpu_error attachShaderWithEntryPoint(const agpu::shader_ref &newShader, agpu_shader_type type, agpu_cstring entry_point) override;
    virtual agpu_size getBuildingLogLength() override;
    virtual agpu_error getBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer) override;
    virtual agpu_error setShaderSignature(const agpu::shader_signature_ref &newSignature) override;

    agpu::device_ref device;

private:
    agpu::shader_ref shader;
    agpu::shader_signature_ref shaderSignature;
    std::string shaderEntryPointName;

    VkComputePipelineCreateInfo pipelineInfo;

};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_COMPUTE_PIPELINE_BUILDER_HPP
