#ifndef AGPU_METAL_COMPUTE_PIPELINE_BUILDER_HPP
#define AGPU_METAL_COMPUTE_PIPELINE_BUILDER_HPP

#include "device.hpp"
#include <string>

namespace AgpuMetal
{
    
class AMtlComputePipelineBuilder : public agpu::compute_pipeline_builder
{
public:
    AMtlComputePipelineBuilder(const agpu::device_ref &device);
    ~AMtlComputePipelineBuilder();

    static agpu::compute_pipeline_builder_ref create(const agpu::device_ref &device);

    virtual agpu::pipeline_state_ptr build() override;
    virtual agpu_error attachShader(const agpu::shader_ref &shader) override;
    virtual agpu_error attachShaderWithEntryPoint(const agpu::shader_ref &shader, agpu_shader_type type, agpu_cstring entry_point) override;
    virtual agpu_size getBuildingLogLength() override;
    virtual agpu_error getBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer) override;
    virtual agpu_error setShaderSignature(const agpu::shader_signature_ref &newSignature) override;

    agpu::device_ref device;

    agpu::shader_ref shader;
    std::string shaderEntryPoint;
    agpu::shader_signature_ref shaderSignature;

    std::string buildingLog;

    MTLSize localSize;
};

} // End of namespace AgpuMetal

#endif //AGPU_METAL_COMPUTE_PIPELINE_BUILDER_HPP
