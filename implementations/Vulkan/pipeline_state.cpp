#include "pipeline_state.hpp"

_agpu_pipeline_state::_agpu_pipeline_state(agpu_device *device)
    : device(device)
{
    pipeline = nullptr;
}

void _agpu_pipeline_state::lostReferences()
{
    vkDestroyPipeline(device->device, pipeline, nullptr);
    vkDestroyRenderPass(device->device, renderPass, nullptr);
}

agpu_int _agpu_pipeline_state::getUniformLocation(agpu_cstring name)
{
    return -1;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddPipelineStateReference(agpu_pipeline_state* pipeline_state)
{
    CHECK_POINTER(pipeline_state);
    return pipeline_state->retain();
}

AGPU_EXPORT agpu_error agpuReleasePipelineState(agpu_pipeline_state* pipeline_state)
{
    CHECK_POINTER(pipeline_state);
    return pipeline_state->release();
}

AGPU_EXPORT agpu_int agpuGetUniformLocation(agpu_pipeline_state* pipeline_state, agpu_cstring name)
{
    CHECK_POINTER(pipeline_state);
    return pipeline_state->getUniformLocation(name);
}
