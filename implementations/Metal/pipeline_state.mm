#include "pipeline_state.hpp"
#include "pipeline_builder.hpp"
#include "compute_pipeline_builder.hpp"

AGPUMTLRenderPipelineState::AGPUMTLRenderPipelineState()
{
    depthStencilState = nil;
}

AGPUMTLRenderPipelineState::~AGPUMTLRenderPipelineState()
{
    if(handle)
        [handle release];
    if(depthStencilState)
        [depthStencilState release];
}

void AGPUMTLRenderPipelineState::applyRenderCommands(id<MTLRenderCommandEncoder> renderEncoder)
{
    [renderEncoder setRenderPipelineState: handle];
    [renderEncoder setDepthStencilState: depthStencilState];
    [renderEncoder setCullMode: commandState.cullMode];
    [renderEncoder setFrontFacingWinding: commandState.frontFace];
}

AGPUMTLComputePipelineState::AGPUMTLComputePipelineState()
{
    handle = nil;
}

AGPUMTLComputePipelineState::~AGPUMTLComputePipelineState()
{
    if(handle)
        [handle release];
}

void AGPUMTLComputePipelineState::applyComputeCommands(id<MTLComputeCommandEncoder> computeEncoder)
{
    [computeEncoder setComputePipelineState: handle];
}

_agpu_pipeline_state::_agpu_pipeline_state(agpu_device *device)
    : device(device)
{
    extraState = nullptr;
}

void _agpu_pipeline_state::lostReferences()
{
    delete extraState;
}

agpu_pipeline_state *_agpu_pipeline_state::createRender(agpu_device *device, agpu_pipeline_builder *builder, id<MTLRenderPipelineState> handle)
{
    auto result = new agpu_pipeline_state(device);
    
    auto renderExtraState = new AGPUMTLRenderPipelineState;
    result->extraState = renderExtraState;
    
    renderExtraState->handle = handle;
    renderExtraState->commandState = builder->commandState;
    renderExtraState->depthStencilState = [device->device newDepthStencilStateWithDescriptor: builder->depthStencilDescriptor];
    return result;
}

agpu_pipeline_state *_agpu_pipeline_state::createCompute(agpu_device *device, agpu_compute_pipeline_builder *builder, id<MTLComputePipelineState> handle)
{
    auto result = new agpu_pipeline_state(device);
    
    auto renderExtraState = new AGPUMTLComputePipelineState;
    result->extraState = renderExtraState;
    
    renderExtraState->handle = handle;
    renderExtraState->localSize = builder->localSize;
    return result;
}

void _agpu_pipeline_state::applyRenderCommands(id<MTLRenderCommandEncoder> renderEncoder)
{
    extraState->applyRenderCommands(renderEncoder);
}

void _agpu_pipeline_state::applyComputeCommands(id<MTLComputeCommandEncoder> computeEncoder)
{
    extraState->applyComputeCommands(computeEncoder);
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
