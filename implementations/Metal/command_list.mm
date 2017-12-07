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

inline MTLIndexType mapIndexType(agpu_size stride)
{
    switch(stride)
    {
    default:
    case 2: return MTLIndexTypeUInt16;
    case 4: return MTLIndexTypeUInt32;
    }
}

_agpu_command_list::_agpu_command_list(agpu_device *device)
    : device(device)
{
    allocator = nullptr;
    currentIndexBuffer = nullptr;
    currentIndirectBuffer = nullptr;
    currentVertexBinding = nullptr;
    currentPipeline = nullptr;
    currentShaderSignature = nullptr;
    vertexBufferCount = 0;
    buffer = nil;
    renderEncoder = nil;
    used = false;
    pushConstantsModified = true;
    memset(activeShaderResourceBindings, 0, sizeof(activeShaderResourceBindings));
    memset(pushConstantsBuffer, 0, sizeof(pushConstantsBuffer));
}

void _agpu_command_list::lostReferences()
{
    if(buffer)
        [buffer release];
    if(allocator)
        allocator->release();
}

agpu_command_list* _agpu_command_list::create ( agpu_device* device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
{
    if(!allocator)
        return nullptr;

    auto result = new agpu_command_list(device);
    result->type = type;
    auto error = result->reset(allocator, initial_pipeline_state);
    if(error != AGPU_OK)
    {
        result->release();
        return nullptr;
    }

    if(initial_pipeline_state)
        result->usePipelineState(initial_pipeline_state);

    return result;
}

agpu_error _agpu_command_list::setShaderSignature ( agpu_shader_signature* signature )
{
    if(signature)
        signature->retain();
    if(currentShaderSignature)
        currentShaderSignature->release();
    currentShaderSignature = signature;
    return AGPU_OK;
}

agpu_error _agpu_command_list::setViewport ( agpu_int x, agpu_int y, agpu_int w, agpu_int h )
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

agpu_error _agpu_command_list::setScissor ( agpu_int x, agpu_int y, agpu_int w, agpu_int h )
{
    MTLScissorRect scissor;
    scissor.x = x;
    scissor.y = y;
    scissor.width = w;
    scissor.height = h;
    [renderEncoder setScissorRect: scissor];

    return AGPU_OK;
}

agpu_error _agpu_command_list::usePipelineState ( agpu_pipeline_state* pipeline )
{
    CHECK_POINTER(pipeline);

    pipeline->retain();
    if(currentPipeline)
        currentPipeline->release();

    currentPipeline = pipeline;
    if(renderEncoder)
        currentPipeline->applyRenderCommands(renderEncoder);

    return AGPU_OK;
}

agpu_error _agpu_command_list::useVertexBinding ( agpu_vertex_binding* vertex_binding )
{
    if(vertex_binding)
        vertex_binding->retain();
    if(currentVertexBinding)
        currentVertexBinding->release();
    currentVertexBinding = vertex_binding;
    for(auto buffer : currentVertexBinding->buffers)
    {
        if(!buffer)
            return AGPU_INVALID_PARAMETER;
    }

    return AGPU_OK;
}

agpu_error _agpu_command_list::useIndexBuffer ( agpu_buffer* index_buffer )
{
    if(index_buffer)
        index_buffer->retain();
    if(currentIndexBuffer)
        currentIndexBuffer->release();
    currentIndexBuffer = index_buffer;
    return AGPU_OK;
}

agpu_error _agpu_command_list::useDrawIndirectBuffer ( agpu_buffer* draw_buffer )
{
    if(draw_buffer)
        draw_buffer->retain();
    if(currentIndirectBuffer)
        currentIndirectBuffer->release();
    currentIndirectBuffer = draw_buffer;
    return AGPU_OK;
}

agpu_error _agpu_command_list::useShaderResources ( agpu_shader_resource_binding* binding )
{
    CHECK_POINTER(binding);
    if(binding->elementIndex >= MaxActiveResourceBindings)
        return AGPU_UNSUPPORTED;

    binding->retain();
    if(activeShaderResourceBindings[binding->elementIndex])
        activeShaderResourceBindings[binding->elementIndex]->release();
    activeShaderResourceBindings[binding->elementIndex] = binding;
    return AGPU_OK;
}

void _agpu_command_list::updateRenderState()
{
    activateVertexBinding();
    activateShaderResourceBindings();
    uploadPushConstants();
}

void _agpu_command_list::activateVertexBinding ( )
{
    if(!currentVertexBinding)
    {
        vertexBufferCount = 0;
        return;
    }

    auto &buffers = currentVertexBinding->buffers;
    vertexBufferCount = buffers.size();
    for(size_t i = 0; i < vertexBufferCount; ++i)
    {
        auto buffer = buffers[i];
        if(!buffer)
            return;

        [renderEncoder setVertexBuffer: buffer->handle offset: 0 atIndex: i];
    }
}


void _agpu_command_list::activateShaderResourceBindings()
{
    for(size_t i = 0; i < MaxActiveResourceBindings; ++i)
    {
        auto activeBinding = activeShaderResourceBindings[i];
        if(!activeBinding)
            continue;
        activeBinding->activateOn(vertexBufferCount, renderEncoder);
    }
}

void _agpu_command_list::uploadPushConstants()
{
    if(/*!pushConstantsModified || */currentShaderSignature->pushConstantBufferSize == 0)
        return;

    [renderEncoder setVertexBytes: pushConstantsBuffer length: currentShaderSignature->pushConstantBufferSize atIndex: 1 + currentShaderSignature->pushConstantBufferIndex];
    [renderEncoder setFragmentBytes: pushConstantsBuffer length: currentShaderSignature->pushConstantBufferSize atIndex: currentShaderSignature->pushConstantBufferIndex];
    pushConstantsModified = false;
}

agpu_error _agpu_command_list::drawArrays ( agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance )
{
    if(!currentPipeline)
        return AGPU_INVALID_OPERATION;

    updateRenderState();
    [renderEncoder drawPrimitives: currentPipeline->commandState.primitiveType
                       vertexStart: first_vertex
                        vertexCount: vertex_count
                    instanceCount: instance_count
                     baseInstance: base_instance];
    return AGPU_OK;
}

agpu_error _agpu_command_list::drawElements ( agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance )
{
    if(!currentIndexBuffer || !currentPipeline)
        return AGPU_INVALID_OPERATION;

    updateRenderState();
    [renderEncoder drawIndexedPrimitives: currentPipeline->commandState.primitiveType
                   indexCount: index_count
                    indexType: mapIndexType(currentIndexBuffer->description.stride)
                  indexBuffer: currentIndexBuffer->handle
            indexBufferOffset: first_index*currentIndexBuffer->description.stride
                instanceCount: instance_count
                   baseVertex: base_vertex
                 baseInstance: base_instance];
    return AGPU_OK;
}

agpu_error _agpu_command_list::drawElementsIndirect ( agpu_size offset )
{
    if(!currentIndexBuffer || !currentIndirectBuffer || !currentPipeline)
        return AGPU_INVALID_OPERATION;
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::multiDrawElementsIndirect ( agpu_size offset, agpu_size drawcount )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setStencilReference ( agpu_uint reference )
{
    [renderEncoder setStencilReferenceValue: reference];
    return AGPU_OK;
}

agpu_error _agpu_command_list::executeBundle ( agpu_command_list* bundle )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::close (  )
{
    return AGPU_OK;
}

agpu_error _agpu_command_list::reset ( agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
{
    CHECK_POINTER(allocator);

    // Store the new allocator.
    allocator->retain();
    if(this->allocator)
        this->allocator->release();
    this->allocator = allocator;

    // Create the buffer.
    if(buffer)
        [buffer release];
    buffer = [allocator->queue->handle commandBuffer];
    used = false;

    if(currentIndirectBuffer)
        currentIndirectBuffer->release();
    currentIndirectBuffer = nullptr;

    if(currentIndexBuffer)
        currentIndexBuffer->release();
    currentIndexBuffer = nullptr;

    if(currentVertexBinding)
        currentVertexBinding->release();
    currentVertexBinding = nullptr;

    for(size_t i = 0; i < MaxActiveResourceBindings; ++i)
    {
        if(activeShaderResourceBindings[i])
            activeShaderResourceBindings[i]->release();
        activeShaderResourceBindings[i] = nullptr;
    }

    if(initial_pipeline_state)
        initial_pipeline_state->retain();
    if(currentPipeline)
        currentPipeline->release();
    currentPipeline = initial_pipeline_state;

    if(currentShaderSignature)
        currentShaderSignature->release();
    currentShaderSignature = nullptr;

    vertexBufferCount = 0;
    pushConstantsModified = true;
    memset(pushConstantsBuffer, 0, sizeof(pushConstantsBuffer));

    return AGPU_OK;
}

agpu_error _agpu_command_list::resetBundle ( agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state, agpu_inheritance_info* inheritance_info )
{
    return reset(allocator, initial_pipeline_state);
}

agpu_error _agpu_command_list::beginRenderPass ( agpu_renderpass* renderpass, agpu_framebuffer* framebuffer, agpu_bool bundle_content )
{
    CHECK_POINTER(renderpass);
    CHECK_POINTER(framebuffer);

    auto descriptor = renderpass->createDescriptor(framebuffer);
    renderEncoder = [buffer renderCommandEncoderWithDescriptor: descriptor];
    [descriptor release];
    if(currentPipeline)
        currentPipeline->applyRenderCommands(renderEncoder);

    return AGPU_OK;
}

agpu_error _agpu_command_list::endRenderPass (  )
{
    if(!renderEncoder)
        return AGPU_INVALID_OPERATION;

    [renderEncoder endEncoding];
    renderEncoder = nil;
    return AGPU_OK;
}

agpu_error _agpu_command_list::resolveFramebuffer ( agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::pushConstants ( agpu_uint offset, agpu_uint size, agpu_pointer values )
{
    if(size + offset > MaxPushConstantBufferSize)
        return AGPU_OUT_OF_BOUNDS;

    memcpy(pushConstantsBuffer + offset, values, size);
    pushConstantsModified = true;
    return AGPU_OK;
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddCommandListReference ( agpu_command_list* command_list )
{
    CHECK_POINTER(command_list);
    return command_list->retain();
}

AGPU_EXPORT agpu_error agpuReleaseCommandList ( agpu_command_list* command_list )
{
    CHECK_POINTER(command_list);
    return command_list->release();
}

AGPU_EXPORT agpu_error agpuSetShaderSignature ( agpu_command_list* command_list, agpu_shader_signature* signature )
{
    CHECK_POINTER(command_list);
    return command_list->setShaderSignature(signature);
}

AGPU_EXPORT agpu_error agpuSetViewport ( agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h )
{
    CHECK_POINTER(command_list);
    return command_list->setViewport(x, y, w, h);
}

AGPU_EXPORT agpu_error agpuSetScissor ( agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h )
{
    CHECK_POINTER(command_list);
    return command_list->setScissor(x, y, w, h);
}

AGPU_EXPORT agpu_error agpuUsePipelineState ( agpu_command_list* command_list, agpu_pipeline_state* pipeline )
{
    CHECK_POINTER(command_list);
    return command_list->usePipelineState(pipeline);
}

AGPU_EXPORT agpu_error agpuUseVertexBinding ( agpu_command_list* command_list, agpu_vertex_binding* vertex_binding )
{
    CHECK_POINTER(command_list);
    return command_list->useVertexBinding(vertex_binding);
}

AGPU_EXPORT agpu_error agpuUseIndexBuffer ( agpu_command_list* command_list, agpu_buffer* index_buffer )
{
    CHECK_POINTER(command_list);
    return command_list->useIndexBuffer(index_buffer);
}

AGPU_EXPORT agpu_error agpuUseDrawIndirectBuffer ( agpu_command_list* command_list, agpu_buffer* draw_buffer )
{
    CHECK_POINTER(command_list);
    return command_list->useDrawIndirectBuffer(draw_buffer);
}

AGPU_EXPORT agpu_error agpuUseShaderResources ( agpu_command_list* command_list, agpu_shader_resource_binding* binding )
{
    CHECK_POINTER(command_list);
    return command_list->useShaderResources(binding);
}

AGPU_EXPORT agpu_error agpuDrawArrays ( agpu_command_list* command_list, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance )
{
    CHECK_POINTER(command_list);
    return command_list->drawArrays(vertex_count, instance_count, first_vertex, base_instance);
}

AGPU_EXPORT agpu_error agpuDrawElements ( agpu_command_list* command_list, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance )
{
    CHECK_POINTER(command_list);
    return command_list->drawElements(index_count, instance_count, first_index, base_vertex, base_instance);
}

AGPU_EXPORT agpu_error agpuDrawElementsIndirect ( agpu_command_list* command_list, agpu_size offset )
{
    CHECK_POINTER(command_list);
    return command_list->drawElementsIndirect(offset);
}

AGPU_EXPORT agpu_error agpuMultiDrawElementsIndirect ( agpu_command_list* command_list, agpu_size offset, agpu_size drawcount )
{
    CHECK_POINTER(command_list);
    return command_list->multiDrawElementsIndirect(offset, drawcount);
}

AGPU_EXPORT agpu_error agpuSetStencilReference ( agpu_command_list* command_list, agpu_uint reference )
{
    CHECK_POINTER(command_list);
    return command_list->setStencilReference(reference);
}

AGPU_EXPORT agpu_error agpuExecuteBundle ( agpu_command_list* command_list, agpu_command_list* bundle )
{
    CHECK_POINTER(command_list);
    return command_list->executeBundle(bundle);
}

AGPU_EXPORT agpu_error agpuCloseCommandList ( agpu_command_list* command_list )
{
    CHECK_POINTER(command_list);
    return command_list->close();
}

AGPU_EXPORT agpu_error agpuResetCommandList ( agpu_command_list* command_list, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
{
    CHECK_POINTER(command_list);
    return command_list->reset(allocator, initial_pipeline_state);
}

AGPU_EXPORT agpu_error agpuResetBundleCommandList ( agpu_command_list* command_list, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state, agpu_inheritance_info* inheritance_info )
{
    CHECK_POINTER(command_list);
    return command_list->resetBundle(allocator, initial_pipeline_state, inheritance_info);
}

AGPU_EXPORT agpu_error agpuBeginRenderPass ( agpu_command_list* command_list, agpu_renderpass* renderpass, agpu_framebuffer* framebuffer, agpu_bool bundle_content )
{
    CHECK_POINTER(command_list);
    return command_list->beginRenderPass(renderpass, framebuffer, bundle_content);
}

AGPU_EXPORT agpu_error agpuEndRenderPass ( agpu_command_list* command_list )
{
    CHECK_POINTER(command_list);
    return command_list->endRenderPass();
}

AGPU_EXPORT agpu_error agpuResolveFramebuffer ( agpu_command_list* command_list, agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer )
{
    CHECK_POINTER(command_list);
    return command_list->resolveFramebuffer(destFramebuffer, sourceFramebuffer);
}

AGPU_EXPORT agpu_error agpuPushConstants ( agpu_command_list* command_list, agpu_uint offset, agpu_uint size, agpu_pointer values )
{
    CHECK_POINTER(command_list);
    return command_list->pushConstants(offset, size, values);
}
