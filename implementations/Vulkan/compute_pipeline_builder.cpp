#include "compute_pipeline_builder.hpp"
#include "pipeline_state.hpp"
#include "shader.hpp"
#include "shader_signature.hpp"

namespace AgpuVulkan
{

AVkComputePipelineBuilder::AVkComputePipelineBuilder(const agpu::device_ref &device)
    : device(device)
{
    // Pipeline state info.
    memset(&pipelineInfo, 0, sizeof(pipelineInfo));
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
}

AVkComputePipelineBuilder::~AVkComputePipelineBuilder()
{
}

agpu::compute_pipeline_builder_ref AVkComputePipelineBuilder::create(const agpu::device_ref &device)
{
    return agpu::makeObject<AVkComputePipelineBuilder> (device);
}

agpu::pipeline_state_ptr AVkComputePipelineBuilder::build ()
{
	if (!shader)
		return nullptr;

	VkPipeline pipeline;
	auto error = vkCreateComputePipelines(deviceForVk->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
	if (error)
	{
		return nullptr;
	}

    auto result = agpu::makeObject<AVkPipelineState> (device);
    auto pipelineState = result.as<AVkPipelineState> ();
	pipelineState->pipeline = pipeline;
	pipelineState->bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
	return result.disown();
}

agpu_error AVkComputePipelineBuilder::attachShader (const agpu::shader_ref &newShader)
{
    CHECK_POINTER(newShader);
    return attachShaderWithEntryPoint(newShader, AGPU_COMPUTE_SHADER, "main");
}

agpu_error AVkComputePipelineBuilder::attachShaderWithEntryPoint(const agpu::shader_ref &newShader, agpu_shader_type type, agpu_cstring entry_point )
{
    CHECK_POINTER(newShader);
    CHECK_POINTER(entry_point);
    if(type != AGPU_COMPUTE_SHADER)
        return AGPU_INVALID_PARAMETER;

    shader = newShader;
    shaderEntryPointName = entry_point;
	pipelineInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineInfo.stage.module = newShader.as<AVkShader> ()->shaderModule;
	pipelineInfo.stage.pName = shaderEntryPointName.c_str();
	pipelineInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    return AGPU_OK;

}

agpu_size AVkComputePipelineBuilder::getBuildingLogLength()
{
    return 0;
}

agpu_error AVkComputePipelineBuilder::getBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
    return AGPU_OK;
}

agpu_error AVkComputePipelineBuilder::setShaderSignature(const agpu::shader_signature_ref &newSignature)
{
    CHECK_POINTER(newSignature);
    pipelineInfo.layout = newSignature.as<AVkShaderSignature> ()->layout;
    this->shaderSignature = newSignature;
    return AGPU_OK;
}

} // End of namespace AgpuVulkan
