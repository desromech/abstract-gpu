#include "compute_pipeline_builder.hpp"
#include "pipeline_state.hpp"
#include "shader.hpp"
#include "shader_signature.hpp"

_agpu_compute_pipeline_builder::_agpu_compute_pipeline_builder(agpu_device *device)
    : device(device)
{
    shader = nullptr;
    shaderSignature = nullptr;

    // Pipeline state info.
    memset(&pipelineInfo, 0, sizeof(pipelineInfo));
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
}
void _agpu_compute_pipeline_builder::lostReferences()
{
    if(shader)
        shader->release();
    if(shaderSignature)
        shaderSignature->release();
}

_agpu_compute_pipeline_builder *_agpu_compute_pipeline_builder::create(agpu_device *device)
{
	std::unique_ptr<_agpu_compute_pipeline_builder> builder(new _agpu_compute_pipeline_builder(device));
	return builder.release();
}

agpu_pipeline_state* _agpu_compute_pipeline_builder::build ()
{
	if (!shader)
		return nullptr;

	VkPipeline pipeline;
	auto error = vkCreateComputePipelines(device->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
	if (error)
	{
		return nullptr;
	}

	auto result = new agpu_pipeline_state(device);
	result->pipeline = pipeline;
	result->bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
	return result;
}

agpu_error _agpu_compute_pipeline_builder::attachShader ( agpu_shader* newShader )
{
    CHECK_POINTER(newShader);

    return attachShaderWithEntryPoint(newShader, AGPU_COMPUTE_SHADER, "main");
}

agpu_error _agpu_compute_pipeline_builder::attachShaderWithEntryPoint ( agpu_shader* newShader, agpu_shader_type type, agpu_cstring entry_point )
{
    CHECK_POINTER(newShader);
    CHECK_POINTER(entry_point);
    if(type != AGPU_COMPUTE_SHADER)
        return AGPU_INVALID_PARAMETER;

    newShader->retain();
    if(shader)
        shader->release();
    shader = newShader;
    shaderEntryPointName = entry_point;
	pipelineInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineInfo.stage.module = newShader->shaderModule;
	pipelineInfo.stage.pName = shaderEntryPointName.c_str();
	pipelineInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    return AGPU_OK;

}

agpu_size _agpu_compute_pipeline_builder::getPipelineBuildingLogLength()
{
    return 0;
}

agpu_error _agpu_compute_pipeline_builder::getPipelineBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
    return AGPU_OK;
}

agpu_error _agpu_compute_pipeline_builder::setShaderSignature ( agpu_shader_signature* newSignature )
{
    CHECK_POINTER(newSignature);
    pipelineInfo.layout = newSignature->layout;
    newSignature->retain();
    if (this->shaderSignature)
        this->shaderSignature->release();
    this->shaderSignature = newSignature;
    return AGPU_OK;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddComputePipelineBuilderReference ( agpu_compute_pipeline_builder* compute_pipeline_builder )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->retain();
}

AGPU_EXPORT agpu_error agpuReleaseComputePipelineBuilder ( agpu_compute_pipeline_builder* compute_pipeline_builder )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->release();
}

AGPU_EXPORT agpu_pipeline_state* agpuBuildComputePipelineState ( agpu_compute_pipeline_builder* compute_pipeline_builder )
{
    if(!compute_pipeline_builder)
        return nullptr;

    return compute_pipeline_builder->build();
}

AGPU_EXPORT agpu_error agpuAttachComputeShader ( agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader* shader )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->attachShader(shader);
}

AGPU_EXPORT agpu_error agpuAttachComputeShaderWithEntryPoint ( agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader* shader, agpu_shader_type type, agpu_cstring entry_point )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->attachShaderWithEntryPoint(shader, type, entry_point);
}

AGPU_EXPORT agpu_size agpuGetComputePipelineBuildingLogLength ( agpu_compute_pipeline_builder* compute_pipeline_builder )
{
    if(!compute_pipeline_builder)
        return 0;
    return compute_pipeline_builder->getPipelineBuildingLogLength();
}

AGPU_EXPORT agpu_error agpuGetComputePipelineBuildingLog ( agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->getPipelineBuildingLog(buffer_size, buffer);
}

AGPU_EXPORT agpu_error agpuSetComputePipelineShaderSignature ( agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader_signature* signature )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->setShaderSignature(signature);
}
