#include "pipeline_state.hpp"

_agpu_pipeline_state::_agpu_pipeline_state()
{

}

void _agpu_pipeline_state::lostReferences()
{

}

agpu_int _agpu_pipeline_state::getUniformLocation(agpu_cstring name)
{
    if (!name)
        return -1;

    // TODO: Implement this
    return -1;
}

// Exported C interface
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
    if (!pipeline_state)
        return -1;
    return pipeline_state->getUniformLocation(name);
}
