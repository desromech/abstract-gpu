#include "compute_pipeline_builder.hpp"
#include "shader_signature.hpp"
#include "shader.hpp"
#include "pipeline_state.hpp"

namespace AgpuD3D12
{
ADXComputePipelineBuilder::ADXComputePipelineBuilder(const agpu::device_ref& cdevice)
	: device(cdevice)
{
	memset(&description, 0, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
}

ADXComputePipelineBuilder::~ADXComputePipelineBuilder()
{
}

agpu::compute_pipeline_builder_ref ADXComputePipelineBuilder::create(const agpu::device_ref& device)
{
	return agpu::makeObject<ADXComputePipelineBuilder> (device);
}

agpu::pipeline_state_ptr ADXComputePipelineBuilder::build()
{
	if (!shaderSignature)
	{
		buildingLog = "A shader signature is required.";
		return nullptr;
	}

	if (!shader)
	{
		buildingLog = "A shader is required.";
		return nullptr;
	}

	// Build the pipeline
	ComPtr<ID3D12PipelineState> pipelineState;

	// Compile the shader instances.
	auto pipelineDescription = description;

	auto error = shader.as<ADXShader> ()->getShaderBytecodeForEntryPoint(shaderSignature, AGPU_COMPUTE_SHADER, entryPointName, buildingLog, &pipelineDescription.CS);
	if (error) return nullptr;

	// Create the actual compute pipeline state
	if (FAILED(deviceForDX->d3dDevice->CreateComputePipelineState(&pipelineDescription, IID_PPV_ARGS(&pipelineState))))
	{
		buildingLog = "Failed to create graphics pipeline.";
		return nullptr;
	}

	// Create the wrapper
	auto pipeline = agpu::makeObject<ADXComputePipelineState>();
	auto adxPipeline = pipeline.as<ADXComputePipelineState>();
	adxPipeline->device = device;
	adxPipeline->state = pipelineState;
	return pipeline.disown();

}

agpu_error ADXComputePipelineBuilder::attachShader(const agpu::shader_ref& shader)
{
	CHECK_POINTER(shader);
	return attachShaderWithEntryPoint(shader, shader.as<ADXShader>()->type, "main");
}

agpu_error ADXComputePipelineBuilder::attachShaderWithEntryPoint(const agpu::shader_ref& shader, agpu_shader_type type, agpu_cstring entry_point)
{
	CHECK_POINTER(shader);
	CHECK_POINTER(entry_point);
	if (type != AGPU_COMPUTE_SHADER)
		return AGPU_INVALID_PARAMETER;

	this->entryPointName = entry_point;
	this->shader = shader;
	return AGPU_OK;
}

agpu_size ADXComputePipelineBuilder::getBuildingLogLength()
{
	return (agpu_size)buildingLog.size();
}

agpu_error ADXComputePipelineBuilder::getBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
	strncpy_s(buffer, buffer_size, buildingLog.c_str(), buildingLog.size() + 1);
	return AGPU_OK;
}

agpu_error ADXComputePipelineBuilder::setShaderSignature(const agpu::shader_signature_ref& signature)
{
	CHECK_POINTER(signature);
	this->shaderSignature = signature;
	description.pRootSignature = signature.as<ADXShaderSignature>()->rootSignature.Get();
	return AGPU_OK;
}

} // End of namespace AgpuD3D12
