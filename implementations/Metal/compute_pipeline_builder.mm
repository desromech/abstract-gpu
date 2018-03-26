#include "compute_pipeline_builder.hpp"

_agpu_compute_pipeline_builder::_agpu_compute_pipeline_builder(agpu_device *device)
    : device(device)
{
}

void _agpu_compute_pipeline_builder::lostReferences()
{
}

_agpu_compute_pipeline_builder *_agpu_compute_pipeline_builder::create(agpu_device *device)
{
    return nullptr;
}

agpu_pipeline_state* _agpu_compute_pipeline_builder::build ()
{
    return nullptr;
}

agpu_error _agpu_compute_pipeline_builder::attachShader ( agpu_shader* shader )
{
    return attachShaderWithEntryPoint(shader, "main");
}

agpu_error _agpu_compute_pipeline_builder::attachShaderWithEntryPoint ( agpu_shader* shader, agpu_cstring entry_point )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_size _agpu_compute_pipeline_builder::getPipelineBuildingLogLength ( )
{
    return 0;
}

agpu_error _agpu_compute_pipeline_builder::getPipelineBuildingLog ( agpu_size buffer_size, agpu_string_buffer buffer )
{
    return AGPU_OK;
}

agpu_error _agpu_compute_pipeline_builder::setShaderSignature ( agpu_shader_signature* newSignature )
{
    return AGPU_UNIMPLEMENTED;
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

AGPU_EXPORT agpu_error agpuAttachComputeShaderWithEntryPoint ( agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader* shader, agpu_cstring entry_point )
{
    CHECK_POINTER(compute_pipeline_builder);
    return compute_pipeline_builder->attachShaderWithEntryPoint(shader, entry_point);
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
