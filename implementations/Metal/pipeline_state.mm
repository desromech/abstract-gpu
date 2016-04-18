#include "pipeline_state.hpp"

_agpu_pipeline_state::_agpu_pipeline_state(agpu_device *device)
    : device(device)
{
}

void _agpu_pipeline_state::lostReferences()
{
}

agpu_pipeline_state *_agpu_pipeline_state::create(agpu_device *device, id<MTLRenderPipelineState> handle)
{
    auto result = new agpu_pipeline_state(device);
    result->handle = handle;
    return result;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddPipelineStateReference ( agpu_pipeline_state* pipeline_state )
{
    CHECK_POINTER(pipeline_state);
    return pipeline_state->retain();
}

AGPU_EXPORT agpu_error agpuReleasePipelineState ( agpu_pipeline_state* pipeline_state )
{
    CHECK_POINTER(pipeline_state);
    return pipeline_state->release();
}

AGPU_EXPORT agpu_int agpuGetUniformLocation ( agpu_pipeline_state* pipeline_state, agpu_cstring name )
{
    return AGPU_UNIMPLEMENTED;
}
