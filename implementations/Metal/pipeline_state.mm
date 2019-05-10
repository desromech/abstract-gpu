#include "pipeline_state.hpp"
#include "pipeline_builder.hpp"
#include "compute_pipeline_builder.hpp"

namespace AgpuMetal
{
    
AGPUMTLRenderPipelineState::AGPUMTLRenderPipelineState()
{
    depthStencilState = nil;
    
    depthBiasConstantFactor = 0.0f;
    depthBiasClamp = 0.0f;
    depthBiasSlopeFactor = 0.0f;
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
    [renderEncoder setDepthBias: depthBiasConstantFactor slopeScale: depthBiasSlopeFactor clamp: depthBiasClamp];
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

AMtlPipelineState::AMtlPipelineState(const agpu::device_ref &device)
    : device(device)
{
}

AMtlPipelineState::~AMtlPipelineState()
{
}

agpu::pipeline_state_ref AMtlPipelineState::createRender(const agpu::device_ref &device, AMtlGraphicsPipelineBuilder *builder, id<MTLRenderPipelineState> handle)
{
    auto result = agpu::makeObject<AMtlPipelineState> (device);
    auto pipeline = result.as<AMtlPipelineState> ();
    
    auto renderExtraState = new AGPUMTLRenderPipelineState;
    pipeline->extraState.reset(renderExtraState);
    
    renderExtraState->handle = handle;
    renderExtraState->commandState = builder->commandState;
    if(builder->hasDepthBias)
    {
        renderExtraState->depthBiasConstantFactor = builder->depthBiasConstantFactor;
        renderExtraState->depthBiasSlopeFactor = builder->depthBiasSlopeFactor;
        renderExtraState->depthBiasClamp = builder->depthBiasClamp;        
    }
    
    renderExtraState->depthStencilState = [deviceForMetal->device newDepthStencilStateWithDescriptor: builder->depthStencilDescriptor];
    return result;
}

agpu::pipeline_state_ref AMtlPipelineState::createCompute(const agpu::device_ref &device, AMtlComputePipelineBuilder *builder, id<MTLComputePipelineState> handle)
{
    auto result = agpu::makeObject<AMtlPipelineState> (device);
    auto pipeline = result.as<AMtlPipelineState> ();
    
    auto renderExtraState = new AGPUMTLComputePipelineState;
    pipeline->extraState.reset(renderExtraState);
    
    renderExtraState->handle = handle;
    renderExtraState->localSize = builder->localSize;
    return result;
}

void AMtlPipelineState::applyRenderCommands(id<MTLRenderCommandEncoder> renderEncoder)
{
    extraState->applyRenderCommands(renderEncoder);
}

void AMtlPipelineState::applyComputeCommands(id<MTLComputeCommandEncoder> computeEncoder)
{
    extraState->applyComputeCommands(computeEncoder);
}

} // End of namespace AgpuMetal
