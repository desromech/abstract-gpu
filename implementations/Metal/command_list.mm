#include "command_list.hpp"
#include "command_allocator.hpp"
#include "command_queue.hpp"
#include "framebuffer.hpp"
#include "pipeline_state.hpp"
#include "renderpass.hpp"
#include "vertex_binding.hpp"
#include "buffer.hpp"
#include "shader_signature.hpp"
#include "shader_resource_binding.hpp"
#include "texture_view.hpp"

namespace AgpuMetal
{
    
inline MTLIndexType mapIndexType(agpu_size stride)
{
    switch(stride)
    {
    default:
    case 2: return MTLIndexTypeUInt16;
    case 4: return MTLIndexTypeUInt32;
    }
}

AMtlCommandList::AMtlCommandList(const agpu::device_ref &device)
    : device(device)
{
    buffer = nil;
    blitEncoder = nil;
    renderEncoder = nil;
    computeEncoder = nil;
    used = false;
    pushConstantsModified = true;
    memset(pushConstantsBuffer, 0, sizeof(pushConstantsBuffer));
}

AMtlCommandList::~AMtlCommandList()
{
    if(buffer)
        [buffer release];
}

agpu::command_list_ref AMtlCommandList::create(const agpu::device_ref &device, agpu_command_list_type type, const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initial_pipeline_state)
{
    if(!allocator)
        return agpu::command_list_ref();

    auto result = agpu::makeObject<AMtlCommandList> (device);
    auto commandList = result.as<AMtlCommandList> ();
    commandList->type = type;
    auto error = commandList->reset(allocator, initial_pipeline_state);
    if(error != AGPU_OK)
        return agpu::command_list_ref();

    if(initial_pipeline_state)
        commandList->usePipelineState(initial_pipeline_state);

    return result;
}

agpu_error AMtlCommandList::setShaderSignature(const agpu::shader_signature_ref &signature)
{
    currentShaderSignature = signature;

    currentVertexBinding.reset();
    auto oldPipeline = currentPipeline;
    currentPipeline.reset();

    for(size_t i = 0; i < MaxActiveResourceBindings; ++i)
        activeShaderResourceBindings[i].reset();
    for(size_t i = 0; i < MaxActiveResourceBindings; ++i)
        activeComputeShaderResourceBindings[i].reset();
        
    if(oldPipeline)
        usePipelineState(oldPipeline);

    return AGPU_OK;
}

agpu_error AMtlCommandList::setViewport ( agpu_int x, agpu_int y, agpu_int w, agpu_int h )
{
    MTLViewport viewport;
    viewport.originX = x;
    viewport.originY = y;
    viewport.width = w;
    viewport.height = h;
    viewport.znear = 0.0;
    viewport.zfar = 1.0;
    [renderEncoder setViewport: viewport];

    return AGPU_OK;
}

agpu_error AMtlCommandList::setScissor ( agpu_int x, agpu_int y, agpu_int w, agpu_int h )
{
    MTLScissorRect scissor;
    scissor.x = x;
    scissor.y = y;
    scissor.width = w;
    scissor.height = h;
    [renderEncoder setScissorRect: scissor];

    return AGPU_OK;
}

agpu_error AMtlCommandList::usePipelineState(const agpu::pipeline_state_ref &pipeline)
{
    CHECK_POINTER(pipeline);

    currentPipeline = pipeline;

    if(currentShaderSignature)
    {
        auto amtlPipeline = currentPipeline.as<AMtlPipelineState> ();
        if(renderEncoder)
            amtlPipeline->applyRenderCommands(renderEncoder);
        if(computeEncoder)
            amtlPipeline->applyComputeCommands(computeEncoder);        
    }

    return AGPU_OK;
}

agpu_error AMtlCommandList::useVertexBinding(const agpu::vertex_binding_ref &vertex_binding)
{
    currentVertexBinding = vertex_binding;
    for(auto &buffer : currentVertexBinding.as<AMtlVertexBinding> ()->buffers)
    {
        if(!buffer)
            return AGPU_INVALID_PARAMETER;
    }

    return AGPU_OK;
}

agpu_error AMtlCommandList::useIndexBuffer(const agpu::buffer_ref &index_buffer)
{
    CHECK_POINTER(index_buffer);
    return useIndexBufferAt(index_buffer, 0, index_buffer.as<AMtlBuffer> ()->description.stride);
}


agpu_error AMtlCommandList::useIndexBufferAt(const agpu::buffer_ref &index_buffer, agpu_size offset, agpu_size index_size)
{
    CHECK_POINTER(index_buffer);
    currentIndexBuffer = index_buffer;
    currentIndexBufferOffset = offset;
    currentIndexBufferStride = index_size;
    return AGPU_OK;    
}

agpu_error AMtlCommandList::useDrawIndirectBuffer(const agpu::buffer_ref &draw_buffer)
{
    currentIndirectBuffer = draw_buffer;
    return AGPU_OK;
}

agpu_error AMtlCommandList::useComputeDispatchIndirectBuffer(const agpu::buffer_ref &dispatch_buffer)
{
    currentComputeDispatchIndirectBuffer = dispatch_buffer;
    return AGPU_OK;
}

agpu_error AMtlCommandList::useShaderResources(const agpu::shader_resource_binding_ref &binding)
{
    CHECK_POINTER(binding);
    auto bindingPoint = binding.as<AMtlShaderResourceBinding> ()->elementIndex;
    if(bindingPoint >= MaxActiveResourceBindings)
        return AGPU_UNSUPPORTED;

    activeShaderResourceBindings[bindingPoint] = binding;
    return AGPU_OK;
}

agpu_error AMtlCommandList::useComputeShaderResources(const agpu::shader_resource_binding_ref &binding)
{
    CHECK_POINTER(binding);
    auto bindingPoint = binding.as<AMtlShaderResourceBinding> ()->elementIndex;
    if(bindingPoint >= MaxActiveResourceBindings)
        return AGPU_UNSUPPORTED;

    activeComputeShaderResourceBindings[bindingPoint] = binding;
    return AGPU_OK;
}

void AMtlCommandList::updateRenderState()
{
    activateVertexBinding();
    activateShaderResourceBindings();
    uploadPushConstants();
}

void AMtlCommandList::activateVertexBinding ( )
{
    if(!currentVertexBinding)
    {
        return;
    }

    auto amtlVertexBinding = currentVertexBinding.as<AMtlVertexBinding> ();
    auto boundVertexBufferCount = currentShaderSignature.as<AMtlShaderSignature> ()->boundVertexBufferCount;
    auto &buffers = amtlVertexBinding->buffers;
    auto &offsets = amtlVertexBinding->offsets;
    auto vertexBufferCount = buffers.size();
    for(size_t i = 0; i < vertexBufferCount; ++i)
    {
        auto buffer = buffers[i];
        if(!buffer)
            return;

        auto handle = buffer.as<AMtlBuffer> ()->handle;
        [renderEncoder setVertexBuffer: handle offset: offsets[i] atIndex: boundVertexBufferCount + i];
    }
}


void AMtlCommandList::activateShaderResourceBindings()
{
    for(size_t i = 0; i < MaxActiveResourceBindings; ++i)
    {
        auto activeBinding = activeShaderResourceBindings[i];
        if(!activeBinding)
            continue;
            
        activeBinding.as<AMtlShaderResourceBinding> ()->activateOn(0, renderEncoder);
    }
}

void AMtlCommandList::uploadPushConstants()
{
    auto signature = currentShaderSignature.as<AMtlShaderSignature> ();
    if(/*!pushConstantsModified || */signature->pushConstantBufferSize == 0)
        return;

    [renderEncoder setVertexBytes: pushConstantsBuffer length: signature->pushConstantBufferSize atIndex: signature->pushConstantBufferIndex];
    [renderEncoder setFragmentBytes: pushConstantsBuffer length: signature->pushConstantBufferSize atIndex: signature->pushConstantBufferIndex];
    pushConstantsModified = false;
}

void AMtlCommandList::updateComputeState()
{
    if(!computeEncoder)
    {
        computeEncoder = [buffer computeCommandEncoder];
        currentPipeline.as<AMtlPipelineState> ()->applyComputeCommands(computeEncoder);
    }

    activateComputeShaderResourceBindings();
    uploadComputePushConstants();
}

void AMtlCommandList::activateComputeShaderResourceBindings()
{
    for(size_t i = 0; i < MaxActiveResourceBindings; ++i)
    {
        auto activeBinding = activeComputeShaderResourceBindings[i];
        if(!activeBinding)
            continue;
            
        activeBinding.as<AMtlShaderResourceBinding> ()->activateComputeOn(computeEncoder);
    }
}

void AMtlCommandList::uploadComputePushConstants()
{
    auto signature = currentShaderSignature.as<AMtlShaderSignature> ();
    if(/*!pushConstantsModified || */signature->pushConstantBufferSize == 0)
        return;

    [computeEncoder setBytes: pushConstantsBuffer length: signature->pushConstantBufferSize atIndex: signature->pushConstantBufferIndex];
    pushConstantsModified = false;
}

agpu_error AMtlCommandList::drawArrays ( agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance )
{
    if(!currentPipeline)
        return AGPU_INVALID_OPERATION;

    updateRenderState();
    [renderEncoder drawPrimitives: currentPipeline.as<AMtlPipelineState> ()->getCommandState().primitiveType
                       vertexStart: first_vertex
                        vertexCount: vertex_count
                    instanceCount: instance_count
                     baseInstance: base_instance];
    return AGPU_OK;
}

agpu_error AMtlCommandList::drawArraysIndirect ( agpu_size offset, agpu_size drawcount )
{
    if(!currentIndexBuffer || !currentIndirectBuffer || !currentPipeline)
        return AGPU_INVALID_OPERATION;
    return AGPU_UNIMPLEMENTED;
}

agpu_error AMtlCommandList::drawElements ( agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance )
{
    if(!currentIndexBuffer || !currentPipeline)
        return AGPU_INVALID_OPERATION;

    updateRenderState();
    auto indexBuffer = currentIndexBuffer.as<AMtlBuffer> ();
    [renderEncoder drawIndexedPrimitives: currentPipeline.as<AMtlPipelineState> ()->getCommandState().primitiveType
                   indexCount: index_count
                    indexType: mapIndexType(currentIndexBufferStride)
                  indexBuffer: indexBuffer->handle
            indexBufferOffset: currentIndexBufferOffset + first_index*currentIndexBufferStride
                instanceCount: instance_count
                   baseVertex: base_vertex
                 baseInstance: base_instance];
    return AGPU_OK;
}

agpu_error AMtlCommandList::drawElementsIndirect ( agpu_size offset, agpu_size drawcount )
{
    if(!currentIndexBuffer || !currentIndirectBuffer || !currentPipeline)
        return AGPU_INVALID_OPERATION;
    return AGPU_UNIMPLEMENTED;
}

agpu_error AMtlCommandList::dispatchCompute ( agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z )
{
    if(!currentPipeline)
        return AGPU_INVALID_OPERATION;

    auto pipeline = currentPipeline.as<AMtlPipelineState> ();
    if(!pipeline->extraState->isCompute() || renderEncoder)
        return AGPU_INVALID_OPERATION;

    updateComputeState();
    MTLSize threadgroups;
    threadgroups.width = group_count_x;
    threadgroups.height = group_count_y;
    threadgroups.depth = group_count_z;
    [computeEncoder dispatchThreadgroups: threadgroups threadsPerThreadgroup: pipeline->extraState->getLocalSize()];
    return AGPU_OK;
}

agpu_error AMtlCommandList::dispatchComputeIndirect ( agpu_size offset )
{
    if(!currentPipeline)
        return AGPU_INVALID_OPERATION;

    auto pipeline = currentPipeline.as<AMtlPipelineState> ();
    if(!pipeline->extraState->isCompute() || renderEncoder)
        return AGPU_INVALID_OPERATION;

    updateComputeState();
    return AGPU_UNIMPLEMENTED;
}

agpu_error AMtlCommandList::setStencilReference(agpu_uint reference)
{
    [renderEncoder setStencilReferenceValue: reference];
    return AGPU_OK;
}

agpu_error AMtlCommandList::executeBundle(const agpu::command_list_ref &bundle)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AMtlCommandList::close()
{
    if(computeEncoder)
    {
        [computeEncoder endEncoding];
        computeEncoder = nil;
    }

    return AGPU_OK;
}

agpu_error AMtlCommandList::reset(const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initial_pipeline_state)
{
    CHECK_POINTER(allocator);

    // Store the new allocator.
    this->allocator = allocator;

    // Create the buffer.
    if(buffer)
        [buffer release];
    auto commandQueueHandle = allocator.as<AMtlCommandAllocator> ()->queue.as<AMtlCommandQueue> ()->handle;
    buffer = [commandQueueHandle commandBuffer];
    used = false;

    currentIndirectBuffer.reset();
    currentComputeDispatchIndirectBuffer.reset();
    currentIndexBuffer.reset();
    currentVertexBinding.reset();

    for(size_t i = 0; i < MaxActiveResourceBindings; ++i)
    {
        activeShaderResourceBindings[i].reset();
        activeComputeShaderResourceBindings[i].reset();
    }

    currentPipeline = initial_pipeline_state;
    currentShaderSignature.reset();

    pushConstantsModified = true;
    memset(pushConstantsBuffer, 0, sizeof(pushConstantsBuffer));

    return AGPU_OK;
}

agpu_error AMtlCommandList::resetBundle(const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initial_pipeline_state, agpu_inheritance_info* inheritance_info)
{
    return reset(allocator, initial_pipeline_state);
}

agpu_error AMtlCommandList::beginRenderPass(const agpu::renderpass_ref &renderpass, const agpu::framebuffer_ref &framebuffer, agpu_bool bundle_content)
{
    CHECK_POINTER(renderpass);
    CHECK_POINTER(framebuffer);

    if(computeEncoder)
    {
        [computeEncoder endEncoding];
        computeEncoder = nil;
    }

    auto descriptor = renderpass.as<AMtlRenderPass> ()->createDescriptor(framebuffer);
    renderEncoder = [buffer renderCommandEncoderWithDescriptor: descriptor];
    [descriptor release];
    
    currentPipeline.reset();
    return AGPU_OK;
}

agpu_error AMtlCommandList::endRenderPass (  )
{
    if(!renderEncoder)
        return AGPU_INVALID_OPERATION;

    [renderEncoder endEncoding];
    renderEncoder = nil;
    return AGPU_OK;
}

agpu_error AMtlCommandList::resolveFramebuffer(const agpu::framebuffer_ref &destFramebuffer, const agpu::framebuffer_ref &sourceFramebuffer)
{
    CHECK_POINTER(destFramebuffer);
    CHECK_POINTER(sourceFramebuffer);
    
    auto amtlSourceFramebuffer = sourceFramebuffer.as<AMtlFramebuffer> ();
    auto amtlDestFramebuffer = destFramebuffer.as<AMtlFramebuffer> ();
    
    auto sourceTexture = amtlSourceFramebuffer->getColorTexture(0);
    auto destTexture = amtlDestFramebuffer->getColorTexture(0);
    if(sourceTexture.sampleCount == destTexture.sampleCount)
    {
        MTLOrigin copyOrigin = {};
        MTLSize copySize = {sourceTexture.width, sourceTexture.height, sourceTexture.depth};
        blitEncoder = [buffer blitCommandEncoder];
        [blitEncoder copyFromTexture: sourceTexture
                sourceSlice: 0 sourceLevel: 0
                sourceOrigin: copyOrigin sourceSize: copySize
              toTexture: destTexture
              destinationSlice: 0 destinationLevel: 0
              destinationOrigin: copyOrigin];
        [blitEncoder endEncoding];
        [blitEncoder release];
        blitEncoder = nil;
        return AGPU_OK;
    }

    auto descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    auto passAttachment = descriptor.colorAttachments[0];

    // Set the source attachment
    passAttachment.texture = sourceTexture;
    passAttachment.loadAction = MTLLoadActionLoad;
    passAttachment.storeAction = MTLStoreActionStoreAndMultisampleResolve;

    if(!amtlSourceFramebuffer->ownedBySwapChain)
    {
        auto &view = amtlSourceFramebuffer->colorBufferViews[0];
        auto &viewDescription = view.as<AMtlTextureView> ()->description;
        passAttachment.level = viewDescription.subresource_range.base_miplevel;
        passAttachment.slice = viewDescription.subresource_range.base_arraylayer;
    }

    // Set the resolve attachment
    passAttachment.resolveTexture = destTexture;
    if(!amtlDestFramebuffer->ownedBySwapChain)
    {
        auto &view = amtlDestFramebuffer->colorBufferViews[0];
        auto &viewDescription = view.as<AMtlTextureView> () ->description;
        passAttachment.resolveLevel = viewDescription.subresource_range.base_miplevel;
        passAttachment.resolveSlice = viewDescription.subresource_range.base_arraylayer;
    }

    renderEncoder = [buffer renderCommandEncoderWithDescriptor: descriptor];
    [descriptor release];
    [renderEncoder endEncoding];
    renderEncoder = nil;
    return AGPU_OK;
}

agpu_error AMtlCommandList::resolveTexture(const agpu::texture_ref &sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, const agpu::texture_ref &destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AMtlCommandList::pushConstants ( agpu_uint offset, agpu_uint size, agpu_pointer values )
{
    if(size + offset > MaxPushConstantBufferSize)
        return AGPU_OUT_OF_BOUNDS;

    memcpy(pushConstantsBuffer + offset, values, size);
    pushConstantsModified = true;
    return AGPU_OK;
}

agpu_error AMtlCommandList::memoryBarrier(agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses)
{
    // Disabled, not found on CI server.
#if 0
    MTLBarrierScope scope = MTLBarrierScopeBuffers | MTLBarrierScopeTextures;
    if((source_stage & AGPU_PIPELINE_STAGE_COMPUTE_SHADER) != 0 && computeEncoder)
        [computeEncoder memoryBarrierWithScope: MTLBarrierScopeBuffers | MTLBarrierScopeTextures];
        
    if(renderEncoder)
    {
        auto combinedAccesses = source_accesses | dest_accesses;
        if(combinedAccesses & (
            AGPU_ACCESS_COLOR_ATTACHMENT_READ | AGPU_ACCESS_COLOR_ATTACHMENT_WRITE |
            AGPU_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ | AGPU_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE
        ))
            scope |= MTLBarrierScopeRenderTargets;
        
        MTLRenderStages sourceStages = 0;
        MTLRenderStages destStages = 0;
        if(source_stage & AGPU_PIPELINE_STAGE_VERTEX_SHADER)
            sourceStages |= MTLRenderStageVertex;
        if(source_stage & AGPU_PIPELINE_STAGE_FRAGMENT_SHADER)
            sourceStages |= MTLRenderStageFragment;
        if(source_stage & AGPU_PIPELINE_STAGE_BOTTOM_OF_PIPE)
            sourceStages = MTLRenderStageVertex | MTLRenderStageFragment;

        if(dest_stage & AGPU_PIPELINE_STAGE_VERTEX_SHADER)
            destStages |= MTLRenderStageVertex;
        if(dest_stage & AGPU_PIPELINE_STAGE_FRAGMENT_SHADER)
            destStages |= MTLRenderStageFragment;
        if(dest_stage & AGPU_PIPELINE_STAGE_TOP_OF_PIPE)
            destStages = MTLRenderStageVertex | MTLRenderStageFragment;            
            
        [renderEncoder memoryBarrierWithScope: scope
            afterStages: sourceStages beforeStages: destStages];
    }
#endif
    return AGPU_OK;
}

} // End of namespace AgpuMetal
