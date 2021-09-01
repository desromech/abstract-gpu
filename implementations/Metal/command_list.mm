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
#include "texture.hpp"
#include "texture_view.hpp"
#include "../Common/memory_profiler.hpp"

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
    AgpuProfileConstructor(AMtlCommandList);
    isOpen = false;
    inRenderPass = false;
    pushConstantsModified = true;
    memset(pushConstantsBuffer, 0, sizeof(pushConstantsBuffer));
}

AMtlCommandList::~AMtlCommandList()
{
    AgpuProfileDestructor(AMtlCommandList);
    close();
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

    return result;
}

agpu_error AMtlCommandList::setShaderSignature(const agpu::shader_signature_ref &signature)
{
    recordCommand([=]{
        currentShaderSignature = signature;

        auto oldPipeline = currentPipeline;
        currentPipeline.reset();

        for(size_t i = 0; i < MaxActiveResourceBindings; ++i)
            activeShaderResourceBindings[i].reset();
        for(size_t i = 0; i < MaxActiveResourceBindings; ++i)
            activeComputeShaderResourceBindings[i].reset();

        if(oldPipeline)
            usePipelineState(oldPipeline);
    });

    return AGPU_OK;
}

agpu_error AMtlCommandList::setViewport ( agpu_int x, agpu_int y, agpu_int w, agpu_int h )
{
    recordCommand([=]{
        MTLViewport viewport;
        viewport.originX = x;
        viewport.originY = y;
        viewport.width = w;
        viewport.height = h;
        viewport.znear = 0.0;
        viewport.zfar = 1.0;
        [renderEncoder setViewport: viewport];
    });

    return AGPU_OK;
}

agpu_error AMtlCommandList::setScissor ( agpu_int x, agpu_int y, agpu_int w, agpu_int h )
{
    recordCommand([=]{
        MTLScissorRect scissor;
        scissor.x = x;
        scissor.y = y;
        scissor.width = w;
        scissor.height = h;
        [renderEncoder setScissorRect: scissor];
    });

    return AGPU_OK;
}

agpu_error AMtlCommandList::usePipelineState(const agpu::pipeline_state_ref &pipeline)
{
    CHECK_POINTER(pipeline);

    currentPipeline = pipeline; // Keep this copy for validation purposes.

    recordCommand([=]{
        currentPipeline = pipeline;

        if(currentShaderSignature)
        {
            auto amtlPipeline = currentPipeline.as<AMtlPipelineState> ();
            if(renderEncoder)
                amtlPipeline->applyRenderCommands(renderEncoder);
            if(computeEncoder)
                amtlPipeline->applyComputeCommands(computeEncoder);
        }
    });

    return AGPU_OK;
}

agpu_error AMtlCommandList::useVertexBinding(const agpu::vertex_binding_ref &vertex_binding)
{
    for(auto &buffer : vertex_binding.as<AMtlVertexBinding> ()->buffers)
    {
        if(!buffer)
            return AGPU_INVALID_PARAMETER;
    }

    // Keep this copy for validation purposes.
    currentVertexBinding = vertex_binding;

    recordCommand([=] {
        currentVertexBinding = vertex_binding;
    });

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

    // Keep this copy for validation purposes.
    currentIndexBuffer = index_buffer;
    currentIndexBufferOffset = offset;
    currentIndexBufferStride = index_size;

    recordCommand([=] {
        currentIndexBuffer = index_buffer;
        currentIndexBufferOffset = offset;
        currentIndexBufferStride = index_size;
    });
    return AGPU_OK;
}

agpu_error AMtlCommandList::useDrawIndirectBuffer(const agpu::buffer_ref &draw_buffer)
{
    // Keep this copy for validation purposes.
    currentIndirectBuffer = draw_buffer;

    recordCommand([=] {
        currentIndirectBuffer = draw_buffer;
    });
    return AGPU_OK;
}

agpu_error AMtlCommandList::useComputeDispatchIndirectBuffer(const agpu::buffer_ref &dispatch_buffer)
{
    // Keep this copy for validation purposes.
    currentComputeDispatchIndirectBuffer = dispatch_buffer;
    
    recordCommand([=] {
        currentComputeDispatchIndirectBuffer = dispatch_buffer;
    });
    return AGPU_OK;
}

agpu_error AMtlCommandList::useShaderResources(const agpu::shader_resource_binding_ref &binding)
{
    CHECK_POINTER(binding);
    return useShaderResourcesInSlot(binding, binding.as<AMtlShaderResourceBinding> ()->elementIndex);
}

agpu_error AMtlCommandList::useShaderResourcesInSlot(const agpu::shader_resource_binding_ref & binding, agpu_uint slot)
{
    CHECK_POINTER(binding);
    if(slot >= MaxActiveResourceBindings) return AGPU_UNSUPPORTED;

    recordCommand([=] {
        activeShaderResourceBindings[slot] = binding;
    });
    return AGPU_OK;
}

agpu_error AMtlCommandList::useComputeShaderResources(const agpu::shader_resource_binding_ref &binding)
{
    CHECK_POINTER(binding);
    return useComputeShaderResourcesInSlot(binding, binding.as<AMtlShaderResourceBinding> ()->elementIndex);
}

agpu_error AMtlCommandList::useComputeShaderResourcesInSlot(const agpu::shader_resource_binding_ref & binding, agpu_uint slot)
{
    CHECK_POINTER(binding);
    if(slot >= MaxActiveResourceBindings)
        return AGPU_UNSUPPORTED;

    recordCommand([=] {
        activeComputeShaderResourceBindings[slot] = binding;
    });
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
        computeEncoder = [handle computeCommandEncoder];
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

    recordCommand([=] {
        updateRenderState();
        [renderEncoder drawPrimitives: currentPipeline.as<AMtlPipelineState> ()->getCommandState().primitiveType
                        vertexStart: first_vertex
                            vertexCount: vertex_count
                        instanceCount: instance_count
                        baseInstance: base_instance];
    });
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

    recordCommand([=] {
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
    });
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
    if(!pipeline->extraState->isCompute() || inRenderPass)
        return AGPU_INVALID_OPERATION;

    recordCommand([=] {
        updateComputeState();
        MTLSize threadgroups;
        threadgroups.width = group_count_x;
        threadgroups.height = group_count_y;
        threadgroups.depth = group_count_z;
        [computeEncoder dispatchThreadgroups: threadgroups threadsPerThreadgroup: pipeline->extraState->getLocalSize()];
    });
    return AGPU_OK;
}

agpu_error AMtlCommandList::dispatchComputeIndirect ( agpu_size offset )
{
    if(!currentPipeline)
        return AGPU_INVALID_OPERATION;

    auto pipeline = currentPipeline.as<AMtlPipelineState> ();
    if(!pipeline->extraState->isCompute() || inRenderPass)
        return AGPU_INVALID_OPERATION;
    recordCommand([=] {
        updateComputeState();
        // TODO: Implement this method.
    });
    return AGPU_UNIMPLEMENTED;
}

agpu_error AMtlCommandList::setStencilReference(agpu_uint reference)
{
    recordCommand([=] {
        [renderEncoder setStencilReferenceValue: reference];
    });
    return AGPU_OK;
}

agpu_error AMtlCommandList::executeBundle(const agpu::command_list_ref &bundle)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AMtlCommandList::close()
{
    isOpen = false;
    concretizeRecordedCommands();
    return AGPU_OK;
}

agpu_error AMtlCommandList::reset(const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initial_pipeline_state)
{
    CHECK_POINTER(allocator);

    // Store the new allocator.
    this->allocator = allocator;

    handle = nil;
    isOpen = true;
    inRenderPass = false;
    recordedCommands.clear();
    resetCommandState(); // For validation purposes.
    
    if(initial_pipeline_state)
        usePipelineState(initial_pipeline_state);

    return AGPU_OK;
}

agpu_error AMtlCommandList::resetBundle(const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initial_pipeline_state, agpu_inheritance_info* inheritance_info)
{
    return reset(allocator, initial_pipeline_state);
}

void AMtlCommandList::resetCommandState()
{
    currentIndirectBuffer.reset();
    currentComputeDispatchIndirectBuffer.reset();
    currentIndexBuffer.reset();
    currentVertexBinding.reset();

    for(size_t i = 0; i < MaxActiveResourceBindings; ++i)
    {
        activeShaderResourceBindings[i].reset();
        activeComputeShaderResourceBindings[i].reset();
    }

    currentPipeline.reset();
    currentShaderSignature.reset();

    pushConstantsModified = true;
    memset(pushConstantsBuffer, 0, sizeof(pushConstantsBuffer));
}

void AMtlCommandList::concretizeRecordedCommands()
{
    @autoreleasepool {
        auto commandQueueHandle = allocator.as<AMtlCommandAllocator> ()->queue.as<AMtlCommandQueue> ()->handle;
        handle = [commandQueueHandle commandBufferWithUnretainedReferences];

        // Reset the active command state.
        resetCommandState();

        // Execute the recorded commands.
        for(auto &command : recordedCommands)
            command();

        // Clear any remaining references.
        resetCommandState();
        if(computeEncoder)
        {
            [computeEncoder endEncoding];
            computeEncoder = nil;
        }

        if(renderEncoder)
        {
            [renderEncoder endEncoding];
            renderEncoder = nil;
        }
        
        if(blitEncoder)
        {
            [blitEncoder endEncoding];
            renderEncoder = nil;
        }
    }
}

id<MTLCommandBuffer> AMtlCommandList::getValidHandleForCommitting()
{
    if(!handle && !isOpen)
        concretizeRecordedCommands();

    auto result = handle;
    handle = nil;
    return result;
}

agpu_error AMtlCommandList::beginRenderPass(const agpu::renderpass_ref &renderpass, const agpu::framebuffer_ref &framebuffer, agpu_bool bundle_content)
{
    CHECK_POINTER(renderpass);
    CHECK_POINTER(framebuffer);

    if(inRenderPass)
        return AGPU_INVALID_OPERATION;

    recordCommand([=] {
        if(computeEncoder)
        {
            [computeEncoder endEncoding];
            computeEncoder = nil;
        }

        auto descriptor = renderpass.as<AMtlRenderPass> ()->createDescriptor(framebuffer);
        renderEncoder = [handle renderCommandEncoderWithDescriptor: descriptor];

        currentPipeline.reset();
    });
    inRenderPass = true;
    return AGPU_OK;
}

agpu_error AMtlCommandList::endRenderPass (  )
{
    if(!inRenderPass)
        return AGPU_INVALID_OPERATION;

    recordCommand([=] {
        [renderEncoder endEncoding];
        renderEncoder = nil;
    });
    inRenderPass = false;
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

    agpu_uint sourceLevel = 0;
    agpu_uint sourceLayer = 0;
    if(!amtlSourceFramebuffer->ownedBySwapChain)
    {
        auto &view = amtlSourceFramebuffer->colorBufferViews[0];
        auto &viewDescription = view.as<AMtlTextureView> ()->description;
        sourceLevel = viewDescription.subresource_range.base_miplevel;
        sourceLayer = viewDescription.subresource_range.base_arraylayer;
    }

    agpu_uint destLevel = 0;
    agpu_uint destLayer = 0;
    if(!amtlDestFramebuffer->ownedBySwapChain)
    {
        auto &view = amtlDestFramebuffer->colorBufferViews[0];
        auto &viewDescription = view.as<AMtlTextureView> ()->description;
        destLevel = viewDescription.subresource_range.base_miplevel;
        destLayer = viewDescription.subresource_range.base_arraylayer;
    }

    return doResolveTexture(sourceTexture, sourceLevel, sourceLayer,
            destTexture, destLevel, destLayer,
            1, 1, AGPU_TEXTURE_ASPECT_COLOR);
}

agpu_error AMtlCommandList::resolveTexture(const agpu::texture_ref &sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, const agpu::texture_ref &destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect)
{
    CHECK_POINTER(sourceTexture);
    CHECK_POINTER(destTexture);
    
    auto amtlSourceTexture = sourceTexture.as<AMtlTexture> ();
    auto amtlDestTexture = destTexture.as<AMtlTexture> ();

    return doResolveTexture(amtlSourceTexture->handle, sourceLevel, sourceLayer,
        amtlDestTexture->handle, destLevel, destLayer,
        levelCount, layerCount, aspect);
}

agpu_error AMtlCommandList::doResolveTexture(id<MTLTexture> sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, id<MTLTexture> destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect)
{
    if(levelCount == 0 || layerCount == 0)
        return AGPU_OK;

    recordCommand([=] {
        if(sourceTexture.sampleCount == destTexture.sampleCount)
        {
            MTLOrigin copyOrigin = {};
            MTLSize copySize = {sourceTexture.width, sourceTexture.height, sourceTexture.depth};
            blitEncoder = [handle blitCommandEncoder];

            for(agpu_uint layerIndex = 0; layerIndex < layerCount; ++layerIndex)
            {
                for(agpu_uint levelIndex = 0; levelIndex < levelCount; ++levelIndex)
                {
                    [blitEncoder copyFromTexture: sourceTexture
                            sourceSlice: sourceLayer + layerIndex sourceLevel: sourceLevel + levelIndex
                            sourceOrigin: copyOrigin sourceSize: copySize
                        toTexture: destTexture
                        destinationSlice: destLayer + layerIndex destinationLevel: destLevel + levelIndex
                        destinationOrigin: copyOrigin];
                }            
            }
            [blitEncoder endEncoding];
            blitEncoder = nil;
            return;
        }

        for(agpu_uint layerIndex = 0; layerIndex < layerCount; ++layerIndex)
        {
            for(agpu_uint levelIndex = 0; levelIndex < levelCount; ++levelIndex)
            {
                auto descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
                auto passAttachment = descriptor.colorAttachments[0];

                // Set the source attachment
                passAttachment.texture = sourceTexture;
                passAttachment.loadAction = MTLLoadActionLoad;
                passAttachment.storeAction = MTLStoreActionStoreAndMultisampleResolve;
                passAttachment.level = sourceLevel + levelIndex;
                passAttachment.slice = sourceLayer + layerIndex;

                // Set the resolve attachment
                passAttachment.resolveTexture = destTexture;
                passAttachment.resolveLevel = destLevel + levelIndex;
                passAttachment.resolveSlice = destLayer + layerIndex;

                renderEncoder = [handle renderCommandEncoderWithDescriptor: descriptor];
                [renderEncoder endEncoding];
                renderEncoder = nil;
            }
        }
    });
    return AGPU_OK;
}

agpu_error AMtlCommandList::pushConstants ( agpu_uint offset, agpu_uint size, agpu_pointer values )
{
    if(size + offset > MaxPushConstantBufferSize)
        return AGPU_OUT_OF_BOUNDS;

    recordCommand([=] {
        memcpy(pushConstantsBuffer + offset, values, size);
        pushConstantsModified = true;
    });
    return AGPU_OK;
}

agpu_error AMtlCommandList::memoryBarrier(agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses)
{
    recordCommand([=] {
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
    });
    return AGPU_OK;
}

agpu_error AMtlCommandList::bufferMemoryBarrier(const agpu::buffer_ref & buffer, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_size offset, agpu_size size)
{
    return memoryBarrier(source_stage, dest_stage, source_accesses, dest_accesses);
}

agpu_error AMtlCommandList::textureMemoryBarrier(const agpu::texture_ref & texture, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_texture_usage_mode_mask old_usage, agpu_texture_usage_mode_mask new_usage, agpu_texture_subresource_range* subresource_range)
{
    return memoryBarrier(source_stage, dest_stage, source_accesses, dest_accesses);
}

agpu_error AMtlCommandList::pushBufferTransitionBarrier(const agpu::buffer_ref & buffer, agpu_buffer_usage_mask old_usage, agpu_buffer_usage_mask new_usage)
{
    (void)buffer;
    return AGPU_OK;
}

agpu_error AMtlCommandList::pushTextureTransitionBarrier(const agpu::texture_ref & texture, agpu_texture_usage_mode_mask old_usage, agpu_texture_usage_mode_mask new_usage, agpu_texture_subresource_range* subresource_range)
{
    (void)texture;
    (void)subresource_range;
    return AGPU_OK;
}

agpu_error AMtlCommandList::popBufferTransitionBarrier()
{
    return AGPU_OK;
}

agpu_error AMtlCommandList::popTextureTransitionBarrier()
{
    return AGPU_OK;
}

void AMtlCommandList::beginBlitting()
{
    if(computeEncoder)
    {
        [computeEncoder endEncoding];
        computeEncoder = nil;
    }
    blitEncoder = [handle blitCommandEncoder];
}

void AMtlCommandList::endBlitting()
{
    [blitEncoder endEncoding];
    blitEncoder = nil;
}

agpu_error AMtlCommandList::copyBuffer(const agpu::buffer_ref & source_buffer, agpu_size source_offset, const agpu::buffer_ref & dest_buffer, agpu_size dest_offset, agpu_size copy_size)
{
    CHECK_POINTER(source_buffer);
    CHECK_POINTER(dest_buffer);

    recordCommand([=] {
        beginBlitting();
        [blitEncoder copyFromBuffer: source_buffer.as<AMtlBuffer> ()->handle
            sourceOffset: source_offset
            toBuffer: dest_buffer.as<AMtlBuffer> ()->handle
            destinationOffset: dest_offset
            size: copy_size];
        endBlitting();
    });
    return AGPU_OK;
}

agpu_error AMtlCommandList::copyBufferToTexture(const agpu::buffer_ref & buffer, const agpu::texture_ref & texture, agpu_buffer_image_copy_region* copy_region)
{
    CHECK_POINTER(buffer);
    CHECK_POINTER(texture);
    CHECK_POINTER(copy_region);

    recordCommand([=] {
        beginBlitting();
        MTLOrigin regionOrigin = {copy_region->texture_region.x, copy_region->texture_region.y, copy_region->texture_region.z};
        MTLSize regionSize = {copy_region->texture_region.width, copy_region->texture_region.height, copy_region->texture_region.depth};
        auto bufferOffset = copy_region->buffer_offset;
        auto offsetIncrement = copy_region->buffer_slice_pitch * copy_region->texture_region.depth;
        for(agpu_uint layer = 0; layer < copy_region->texture_subresource_level.layer_count; ++layer)
        {
            [blitEncoder copyFromBuffer: buffer.as<AMtlBuffer> ()->handle
                    sourceOffset: bufferOffset
                sourceBytesPerRow: copy_region->buffer_pitch
            sourceBytesPerImage: copy_region->buffer_slice_pitch
                        sourceSize: regionSize
                        toTexture: texture.as<AMtlTexture> ()->handle
                destinationSlice: copy_region->texture_subresource_level.base_arraylayer + layer
                destinationLevel: copy_region->texture_subresource_level.miplevel
                destinationOrigin: regionOrigin];

            bufferOffset += offsetIncrement;
        }
        endBlitting();
    });
    return AGPU_OK;
}

agpu_error AMtlCommandList::copyTextureToBuffer(const agpu::texture_ref & texture, const agpu::buffer_ref & buffer, agpu_buffer_image_copy_region* copy_region)
{
    CHECK_POINTER(buffer);
    CHECK_POINTER(texture);
    CHECK_POINTER(copy_region);

    recordCommand([=] {
        beginBlitting();
        MTLOrigin regionOrigin = {copy_region->texture_region.x, copy_region->texture_region.y, copy_region->texture_region.z};
        MTLSize regionSize = {copy_region->texture_region.width, copy_region->texture_region.height, copy_region->texture_region.depth};
        auto bufferOffset = copy_region->buffer_offset;
        auto offsetIncrement = copy_region->buffer_slice_pitch * copy_region->texture_region.depth;
        for(agpu_uint layer = 0; layer < copy_region->texture_subresource_level.layer_count; ++layer)
        {
            [blitEncoder copyFromTexture: texture.as<AMtlTexture> ()->handle
                    sourceSlice: copy_region->texture_subresource_level.base_arraylayer + layer
                    sourceLevel: copy_region->texture_subresource_level.miplevel
                sourceOrigin: regionOrigin
                    sourceSize: regionSize
                    toBuffer: buffer.as<AMtlBuffer> ()->handle
            destinationOffset: bufferOffset
        destinationBytesPerRow: copy_region->buffer_pitch
        destinationBytesPerImage: copy_region->buffer_slice_pitch];

            bufferOffset += offsetIncrement;
        }
        endBlitting();
    });
    return AGPU_OK;
}

agpu_error AMtlCommandList::copyTexture(const agpu::texture_ref & source_texture, const agpu::texture_ref & dest_texture, agpu_image_copy_region* copy_region)
{
    CHECK_POINTER(source_texture);
    CHECK_POINTER(dest_texture);
    CHECK_POINTER(copy_region);
    if(copy_region->source_subresource_level.layer_count != copy_region->destination_subresource_level.layer_count)
        return AGPU_INVALID_PARAMETER;

    recordCommand([=] {
        beginBlitting();
        MTLOrigin sourceOrigin = {copy_region->source_offset.x, copy_region->source_offset.y, copy_region->source_offset.z};
        MTLOrigin destinationOrigin = {copy_region->destination_offset.x, copy_region->destination_offset.y, copy_region->destination_offset.z};
        MTLSize extent = {copy_region->extent.width, copy_region->extent.height, copy_region->extent.depth};

        for(agpu_uint layer = 0; layer < copy_region->source_subresource_level.layer_count; ++layer)
        {
            [blitEncoder copyFromTexture: source_texture.as<AMtlTexture> ()->linearViewHandle
                sourceSlice: copy_region->source_subresource_level.base_arraylayer + layer
                sourceLevel: copy_region->source_subresource_level.miplevel
            sourceOrigin: sourceOrigin
                sourceSize: extent
                toTexture: dest_texture.as<AMtlTexture> ()->linearViewHandle
        destinationSlice: copy_region->destination_subresource_level.base_arraylayer + layer
        destinationLevel: copy_region->destination_subresource_level.miplevel
        destinationOrigin: destinationOrigin];
        }
        endBlitting();
    });
    return AGPU_OK;
}

} // End of namespace AgpuMetal
