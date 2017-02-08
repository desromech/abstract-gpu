#include "pipeline_state.hpp"
#include "pipeline_builder.hpp"

_agpu_pipeline_state::_agpu_pipeline_state(agpu_device *device)
    : device(device)
{
    depthStencilState = nil;
}

void _agpu_pipeline_state::lostReferences()
{
    if(handle)
        [handle release];
    if(depthStencilState)
        [depthStencilState release];
}

agpu_pipeline_state *_agpu_pipeline_state::create(agpu_device *device, agpu_pipeline_builder *builder, id<MTLRenderPipelineState> handle)
{
    auto result = new agpu_pipeline_state(device);
    result->handle = handle;
    result->commandState = builder->commandState;
    result->depthStencilState = [device->device newDepthStencilStateWithDescriptor: builder->depthStencilDescriptor];
    return result;
}

void _agpu_pipeline_state::applyRenderCommands(id<MTLRenderCommandEncoder> renderEncoder)
{
    [renderEncoder setRenderPipelineState: handle];
    [renderEncoder setDepthStencilState: depthStencilState];
    [renderEncoder setCullMode: commandState.cullMode];
    [renderEncoder setFrontFacingWinding: commandState.frontFace];

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
