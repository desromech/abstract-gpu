#ifndef AGPU_D3D12_COMPUTE_PIPELINE_BUILDER_HPP
#define AGPU_D3D12_COMPUTE_PIPELINE_BUILDER_HPP

#include "device.hpp"

namespace AgpuD3D12
{
class ADXComputePipelineBuilder : public agpu::compute_pipeline_builder
{
public:
	ADXComputePipelineBuilder(const agpu::device_ref& device);
	~ADXComputePipelineBuilder();

	static agpu::compute_pipeline_builder_ref create(const agpu::device_ref& device);

	virtual agpu::pipeline_state_ptr build() override;

	virtual agpu_error attachShader(const agpu::shader_ref& shader) override;
	virtual agpu_error attachShaderWithEntryPoint(const agpu::shader_ref& shader, agpu_shader_type type, agpu_cstring entry_point) override;
	virtual agpu_size getBuildingLogLength() override;
	virtual agpu_error getBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer) override;
	virtual agpu_error setShaderSignature(const agpu::shader_signature_ref& signature) override;

	agpu::device_ref device;

	D3D12_COMPUTE_PIPELINE_STATE_DESC description;

	std::string buildingLogError;
	std::string entryPointName;
	std::string buildingLog;
	agpu::shader_ref shader;
	agpu::shader_signature_ref shaderSignature;
};

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_COMPUTE_PIPELINE_BUILDER_HPP
