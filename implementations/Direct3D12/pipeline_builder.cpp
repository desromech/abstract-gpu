#include "pipeline_builder.hpp"

_agpu_pipeline_builder::_agpu_pipeline_builder()
{

}

void _agpu_pipeline_builder::lostReferences()
{

}

// Exported C interface
AGPU_EXPORT agpu_error agpuAddPipelineBuilderReference(agpu_pipeline_builder* pipeline_builder)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->retain();
}

AGPU_EXPORT agpu_error agpuReleasePipelineBuilder(agpu_pipeline_builder* pipeline_builder)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->release();
}

AGPU_EXPORT agpu_pipeline_state* agpuBuildPipelineState(agpu_pipeline_builder* pipeline_builder)
{
    return nullptr;
}

AGPU_EXPORT agpu_error agpuAttachShader(agpu_pipeline_builder* pipeline_builder, agpu_shader* shader)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_size agpuGetPipelineBuildingLogLength(agpu_pipeline_builder* pipeline_builder)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuGetPipelineBuildingLog(agpu_pipeline_builder* pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetDepthState(agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetStencilState(agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_int writeMask, agpu_int readMask)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetAlphaTestingState(agpu_pipeline_builder* pipeline_builder, agpu_bool enable, agpu_compare_function function)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetRenderTargetCount(agpu_pipeline_builder* pipeline_builder, agpu_int count)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetPrimitiveTopology(agpu_pipeline_builder* pipeline_builder, agpu_primitive_mode topology)
{
    return AGPU_UNIMPLEMENTED;
}
