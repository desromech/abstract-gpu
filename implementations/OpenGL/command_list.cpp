#include "command_list.hpp"
#include "pipeline_state.hpp"
#include "vertex_binding.hpp"
#include "buffer.hpp"
#include "framebuffer.hpp"
#include "renderpass.hpp"
#include "shader_resource_binding.hpp"
#include <string.h>

inline GLenum mapPrimitiveTopology(agpu_primitive_topology topology)
{
    switch (topology)
    {
    case AGPU_POINTS: return GL_POINTS;
    case AGPU_LINES: return GL_LINES;
    case AGPU_LINES_ADJACENCY: return GL_LINES_ADJACENCY;
    case AGPU_LINE_STRIP: return GL_LINE_STRIP;
    case AGPU_LINE_STRIP_ADJACENCY: return GL_LINE_STRIP_ADJACENCY;
    case AGPU_TRIANGLES: return GL_TRIANGLES;
    case AGPU_TRIANGLES_ADJACENCY: return GL_TRIANGLES_ADJACENCY;
    case AGPU_TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
    case AGPU_TRIANGLE_STRIP_ADJACENCY: return GL_TRIANGLE_STRIP_ADJACENCY;
    case AGPU_PATCHES: return GL_PATCHES;
    default: abort();
    }
}

inline GLenum mapIndexType(agpu_size stride)
{
    switch (stride)
    {
    case 1: return GL_UNSIGNED_BYTE;
    case 2: return GL_UNSIGNED_SHORT;
    case 4: return GL_UNSIGNED_INT;
    default: abort();
    }
}

CommandListExecutionContext::CommandListExecutionContext()
    : currentPipeline(nullptr), currentComputePipeline(nullptr), activePipeline(nullptr)
{
    memset(shaderResourceBindings, 0, sizeof(shaderResourceBindings));
    memset(computeShaderResourceBindings, 0, sizeof(computeShaderResourceBindings));
    memset(pushConstantBuffer, 0, sizeof(pushConstantBuffer));
    reset();
}

void CommandListExecutionContext::lostReferences()
{
    if(currentPipeline)
        currentPipeline->release();
    if(currentComputePipeline)
        currentComputePipeline->release();

    if(activePipeline)
        activePipeline->release();
    for(auto binding : shaderResourceBindings)
    {
        if(binding)
            binding->release();
    }
    for(auto binding : computeShaderResourceBindings)
    {
        if(binding)
            binding->release();
    }
}

void CommandListExecutionContext::reset()
{
    if(currentPipeline)
    {
        currentPipeline->release();
        currentPipeline = nullptr;
    }

    if(currentComputePipeline)
    {
        currentComputePipeline->release();
        currentComputePipeline = nullptr;
    }

    if(activePipeline)
    {
        activePipeline->release();
        activePipeline = nullptr;
    }

    for(auto &binding : shaderResourceBindings)
    {
        if(binding)
            binding->release();
        binding = nullptr;
    }

    for(auto &binding : computeShaderResourceBindings)
    {
        if(binding)
            binding->release();
        binding = nullptr;
    }

    stencilReference = 0;
    primitiveMode = GL_POINTS;
    hasValidActivePipeline = false;
	hasValidShaderResources = false;
	hasValidComputeShaderResources = false;
    hasValidGraphicsPushConstants = false;
    hasValidComputePushConstants = false;
    memset(pushConstantBuffer, 0, sizeof(pushConstantBuffer));

    //glActiveTexture(GL_TEXTURE0);
    // TODO: Unbind the samplers
}

void CommandListExecutionContext::validateBeforeDrawCall()
{
    if(!hasValidActivePipeline || activePipeline != currentPipeline)
    {
        hasValidActivePipeline = true;
        hasValidShaderResources = false;
        hasValidComputeShaderResources = false;
        if(currentPipeline)
            currentPipeline->retain();
        if(activePipeline)
            activePipeline->release();
        activePipeline = currentPipeline;

        if(activePipeline)
        {
            activePipeline->activate();
			if(activePipeline->extraStateData)
				primitiveMode = mapPrimitiveTopology(activePipeline->extraStateData->getPrimitiveTopology());
            if (stencilReference != 0)
                activePipeline->updateStencilReference(stencilReference);
        }
        else
        {
            device->glUseProgram(0);
        }
    }

    if(activePipeline && !hasValidShaderResources)
    {
        hasValidShaderResources = true;
        hasValidComputeShaderResources = false;
        activePipeline->activateShaderResourcesOn(this, shaderResourceBindings);
    }

    if(!hasValidGraphicsPushConstants)
    {
        activePipeline->uploadPushConstants(pushConstantBuffer, sizeof(pushConstantBuffer));
        hasValidGraphicsPushConstants = true;
    }
}

void CommandListExecutionContext::setBaseInstance(agpu_uint base_instance)
{
    if(activePipeline)
        activePipeline->setBaseInstance(base_instance);
}

void CommandListExecutionContext::validateBeforeComputeDispatch()
{
    if(!hasValidActivePipeline || activePipeline != currentComputePipeline)
    {
        hasValidActivePipeline = true;
        hasValidShaderResources = false;
        hasValidComputeShaderResources = false;
        if(currentComputePipeline)
            currentComputePipeline->retain();
        if(activePipeline)
            activePipeline->release();
        activePipeline = currentComputePipeline;

        if(activePipeline)
        {
            activePipeline->activate();
        }
        else
        {
            device->glUseProgram(0);
        }
    }

    if(activePipeline && !hasValidComputeShaderResources)
    {
        hasValidComputeShaderResources = true;
        hasValidShaderResources = false;
        activePipeline->activateShaderResourcesOn(this, computeShaderResourceBindings);
    }

    if(!hasValidComputePushConstants)
    {
        activePipeline->uploadPushConstants(pushConstantBuffer, sizeof(pushConstantBuffer));
        hasValidComputePushConstants = true;
    }
}

void CommandListExecutionContext::usePipelineState(agpu_pipeline_state* pipeline)
{
    if(pipeline)
        pipeline->retain();
    if(currentPipeline)
        currentPipeline->release();
    currentPipeline = pipeline;
}

void CommandListExecutionContext::useComputePipelineState(agpu_pipeline_state* pipeline)
{
    if(pipeline)
        pipeline->retain();
    if(currentComputePipeline)
        currentComputePipeline->release();
    currentComputePipeline = pipeline;
}

void CommandListExecutionContext::setStencilReference(agpu_uint reference)
{
    if (activePipeline)
        activePipeline->updateStencilReference(reference);
    stencilReference = reference;
}

void CommandListExecutionContext::useShaderResources ( agpu_shader_resource_binding* binding )
{
    if(size_t(binding->elementIndex) >= MaxNumberOfShaderResourceBindings)
        return;

    auto &dest = shaderResourceBindings[binding->elementIndex];
    if(binding)
        binding->retain();
    if(dest)
        dest->release();
    dest = binding;
    hasValidShaderResources = false;
}

void CommandListExecutionContext::useComputeShaderResources ( agpu_shader_resource_binding* binding )
{
    if(size_t(binding->elementIndex) >= MaxNumberOfShaderResourceBindings)
        return;

    auto &dest = computeShaderResourceBindings[binding->elementIndex];
    if(binding)
        binding->retain();
    if(dest)
        dest->release();
    dest = binding;
    hasValidComputeShaderResources = false;
}

_agpu_command_list::_agpu_command_list()
{
    closed = false;
    currentPipeline = nullptr;
}

void _agpu_command_list::lostReferences()
{
    executionContext.lostReferences();
}

agpu_command_list *_agpu_command_list::create(agpu_device *device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state)
{
    auto list = new agpu_command_list();
    list->device = device;
    list->executionContext.device = device;
    list->type = type;;
    if(initial_pipeline_state)
        list->usePipelineState(initial_pipeline_state);
    return list;
}

agpu_error _agpu_command_list::setShaderSignature(agpu_shader_signature* signature)
{
    return AGPU_OK;
}

agpu_error _agpu_command_list::setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    return addCommand([=] {
        glViewport(x, y, w, h);
    });
}

agpu_error _agpu_command_list::setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    return addCommand([=] {
        glScissor(x, y, w, h);
    });
}

agpu_error _agpu_command_list::usePipelineState(agpu_pipeline_state* pipeline)
{
	switch (pipeline->type)
	{
	case AgpuPipelineStateType::Graphics:
		return addCommand([=] {
			executionContext.usePipelineState(pipeline);
		});
	case AgpuPipelineStateType::Compute:
		return addCommand([=] {
			executionContext.useComputePipelineState(pipeline);
		});
	default:
		return AGPU_UNSUPPORTED;
	}
}

agpu_error _agpu_command_list::useVertexBinding(agpu_vertex_binding* vertex_binding)
{
    return addCommand([=] {
        this->currentVertexBinding = vertex_binding;
    });
}

agpu_error _agpu_command_list::useIndexBuffer(agpu_buffer* index_buffer)
{
    return addCommand([=] {
        this->currentIndexBuffer= index_buffer;
    });
}

agpu_error _agpu_command_list::useDrawIndirectBuffer(agpu_buffer* draw_buffer)
{
    return addCommand([=] {
        this->currentDrawBuffer = draw_buffer;
    });
}

agpu_error _agpu_command_list::useComputeDispatchIndirectBuffer(agpu_buffer* buffer)
{
    return addCommand([=] {
        this->currentComputeDispatchBuffer = buffer;
    });
}

agpu_error _agpu_command_list::useShaderResources ( agpu_shader_resource_binding* binding )
{
    CHECK_POINTER(binding);
    return addCommand([=] {
        executionContext.useShaderResources(binding);
    });
}

agpu_error _agpu_command_list::useComputeShaderResources ( agpu_shader_resource_binding* binding )
{
    CHECK_POINTER(binding);
    return addCommand([=] {
        executionContext.useComputeShaderResources(binding);
    });
}

agpu_error _agpu_command_list::pushConstants ( agpu_uint offset, agpu_uint size, agpu_pointer values )
{
    CHECK_POINTER(values);

    auto rawSource = reinterpret_cast<const uint8_t*> (values);
    std::vector<uint8_t> dataCopy(rawSource, rawSource + size);
    return addCommand([=] {
        memcpy(executionContext.pushConstantBuffer + offset, &dataCopy[0], size);
        executionContext.hasValidGraphicsPushConstants = false;
        executionContext.hasValidComputePushConstants = false;
    });
}

agpu_error _agpu_command_list::drawArrays ( agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance )
{
    return addCommand([=] {
        if (!currentVertexBinding)
            return;

        currentVertexBinding->bind();
        executionContext.validateBeforeDrawCall();
        executionContext.setBaseInstance(base_instance);

        device->glDrawArraysInstancedBaseInstance(executionContext.primitiveMode, first_vertex, vertex_count, instance_count, base_instance);
    });
}

agpu_error _agpu_command_list::drawArraysIndirect(agpu_size offset, agpu_size drawcount)
{
    return addCommand([=] {
        if (!currentVertexBinding || !currentDrawBuffer)
            return;

        currentVertexBinding->bind();
        currentDrawBuffer->bind();
        executionContext.validateBeforeDrawCall();
        executionContext.setBaseInstance(0);

        if(drawcount > 1)
            device->glMultiDrawArraysIndirect(executionContext.primitiveMode, reinterpret_cast<void*> ((size_t)offset), (GLsizei)drawcount, currentDrawBuffer->description.stride);
        else
            device->glDrawArraysIndirect(executionContext.primitiveMode, reinterpret_cast<void*> ((size_t)offset));
    });
}

agpu_error _agpu_command_list::drawElements ( agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance )
{
    return addCommand([=] {
        if (!currentVertexBinding || !currentIndexBuffer)
            return;

        currentVertexBinding->bind();
        currentIndexBuffer->bind();
        executionContext.validateBeforeDrawCall();
        executionContext.setBaseInstance(base_instance);

        size_t offset = currentIndexBuffer->description.stride*first_index;
        device->glDrawElementsInstancedBaseVertexBaseInstance(executionContext.primitiveMode, index_count,
            mapIndexType(currentIndexBuffer->description.stride), reinterpret_cast<void*> (offset),
            instance_count, base_vertex, base_instance);
    });
}

agpu_error _agpu_command_list::drawElementsIndirect(agpu_size offset, agpu_size drawcount)
{
    return addCommand([=] {
        if (!currentVertexBinding || !currentIndexBuffer || !currentDrawBuffer)
            return;

        currentVertexBinding->bind();
        currentIndexBuffer->bind();
        currentDrawBuffer->bind();
        executionContext.validateBeforeDrawCall();
        executionContext.setBaseInstance(0);

        if(drawcount > 1)
            device->glMultiDrawElementsIndirect(executionContext.primitiveMode, mapIndexType(currentIndexBuffer->description.stride), reinterpret_cast<void*> ((size_t)offset), (GLsizei)drawcount, currentDrawBuffer->description.stride);
        else
            device->glDrawElementsIndirect(executionContext.primitiveMode, mapIndexType(currentIndexBuffer->description.stride), reinterpret_cast<void*> ((size_t)offset));
    });
}

agpu_error _agpu_command_list::dispatchCompute ( agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z )
{
    return addCommand([=] {
        executionContext.validateBeforeComputeDispatch();

        device->glDispatchCompute(group_count_x, group_count_y, group_count_z);
    });
}

agpu_error _agpu_command_list::dispatchComputeIndirect ( agpu_size offset )
{
    return addCommand([=] {
        if(!currentComputeDispatchBuffer)
            return;

        currentComputeDispatchBuffer->bind();
        executionContext.validateBeforeComputeDispatch();

        device->glDispatchComputeIndirect(offset);
    });
}

agpu_error _agpu_command_list::setStencilReference(agpu_uint reference)
{
    return addCommand([=] {
        executionContext.setStencilReference(reference);
    });
}

agpu_error _agpu_command_list::executeBundle ( agpu_command_list* bundle )
{
    CHECK_POINTER(bundle)
    if(bundle->type != AGPU_COMMAND_LIST_TYPE_BUNDLE)
        return AGPU_INVALID_PARAMETER;

    return addCommand([=] {
        bundle->execute();
    });
}

agpu_error _agpu_command_list::close()
{
    closed = true;
    return AGPU_OK;
}

agpu_error _agpu_command_list::reset(agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state)
{
    closed = false;
    commands.clear();
    if (initial_pipeline_state)
        usePipelineState(initial_pipeline_state);
    return AGPU_OK;
}


agpu_error _agpu_command_list::resetBundleCommandList ( agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state, agpu_inheritance_info* inheritance_info )
{
    closed = false;
    commands.clear();
    if (initial_pipeline_state)
        usePipelineState(initial_pipeline_state);
    return AGPU_OK;
}

agpu_error _agpu_command_list::beginRenderPass (agpu_renderpass *renderpass, agpu_framebuffer* framebuffer, agpu_bool bundle_content)
{
    CHECK_POINTER(framebuffer)
    return addCommand([=] {
        framebuffer->bind();
        glViewport(0, 0, framebuffer->width, framebuffer->height);
        glScissor(0, 0, framebuffer->width, framebuffer->height);
        renderpass->started();
    });
}

agpu_error _agpu_command_list::endRenderPass()
{
    return addCommand([=] {
        device->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    });
    return AGPU_OK;
}

agpu_error _agpu_command_list::addCommand(const AgpuGLCommand &command)
{
    if (closed)
        return AGPU_COMMAND_LIST_CLOSED;

    commands.push_back(command);
    return AGPU_OK;
}

void _agpu_command_list::execute()
{
    currentVertexBinding = nullptr;
    currentIndexBuffer = nullptr;
    currentDrawBuffer = nullptr;
    currentComputeDispatchBuffer = nullptr;
    for (auto &command : commands)
        command();

    executionContext.reset();
}

agpu_error _agpu_command_list::resolveFramebuffer(agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer)
{
    CHECK_POINTER(destFramebuffer);
    CHECK_POINTER(sourceFramebuffer);

    return addCommand([=] {
        destFramebuffer->bind(GL_DRAW_FRAMEBUFFER);
        sourceFramebuffer->bind(GL_READ_FRAMEBUFFER);
        device->glBlitFramebuffer(
            0, 0, sourceFramebuffer->width, sourceFramebuffer->height,
            0, 0, destFramebuffer->width, destFramebuffer->height,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
    });

    return AGPU_OK;
}

// C API
AGPU_EXPORT agpu_error agpuAddCommandListReference(agpu_command_list* command_list)
{
    CHECK_POINTER(command_list);
    return command_list->retain();
}

AGPU_EXPORT agpu_error agpuReleaseCommandList(agpu_command_list* command_list)
{
    CHECK_POINTER(command_list);
    return command_list->release();
}

AGPU_EXPORT agpu_error agpuSetShaderSignature(agpu_command_list* command_list, agpu_shader_signature* signature)
{
    CHECK_POINTER(command_list);
    return command_list->setShaderSignature(signature);
}

AGPU_EXPORT agpu_error agpuSetViewport(agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    CHECK_POINTER(command_list);
    return command_list->setViewport(x, y, w, h);
}

AGPU_EXPORT agpu_error agpuSetScissor(agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    CHECK_POINTER(command_list);
    return command_list->setScissor(x, y, w, h);
}

AGPU_EXPORT agpu_error agpuUsePipelineState(agpu_command_list* command_list, agpu_pipeline_state* pipeline)
{
    CHECK_POINTER(command_list);
    return command_list->usePipelineState(pipeline);
}

AGPU_EXPORT agpu_error agpuUseVertexBinding(agpu_command_list* command_list, agpu_vertex_binding* vertex_binding)
{
    CHECK_POINTER(command_list);
    return command_list->useVertexBinding(vertex_binding);
}

AGPU_EXPORT agpu_error agpuUseIndexBuffer(agpu_command_list* command_list, agpu_buffer* index_buffer)
{
    CHECK_POINTER(command_list);
    return command_list->useIndexBuffer(index_buffer);
}

AGPU_EXPORT agpu_error agpuUseDrawIndirectBuffer(agpu_command_list* command_list, agpu_buffer* draw_buffer)
{
    CHECK_POINTER(command_list);
    return command_list->useDrawIndirectBuffer(draw_buffer);
}

AGPU_EXPORT agpu_error agpuUseComputeDispatchIndirectBuffer ( agpu_command_list* command_list, agpu_buffer* buffer )
{
    CHECK_POINTER(command_list);
    return command_list->useComputeDispatchIndirectBuffer(buffer);
}

AGPU_EXPORT agpu_error agpuUseShaderResources ( agpu_command_list* command_list, agpu_shader_resource_binding* binding )
{
    CHECK_POINTER(command_list);
    return command_list->useShaderResources(binding);
}

AGPU_EXPORT agpu_error agpuUseComputeShaderResources ( agpu_command_list* command_list, agpu_shader_resource_binding* binding )
{
    CHECK_POINTER(command_list);
    return command_list->useComputeShaderResources(binding);
}

AGPU_EXPORT agpu_error agpuDrawArrays ( agpu_command_list* command_list, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance )
{
    CHECK_POINTER(command_list);
    return command_list->drawArrays(vertex_count, instance_count, first_vertex, base_instance);
}

AGPU_EXPORT agpu_error agpuDrawArraysIndirect ( agpu_command_list* command_list, agpu_size offset, agpu_size drawcount )
{
    CHECK_POINTER(command_list);
    return command_list->drawArraysIndirect(offset, drawcount);
}

AGPU_EXPORT agpu_error agpuDrawElements ( agpu_command_list* command_list, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance )
{
    CHECK_POINTER(command_list);
    return command_list->drawElements(index_count, instance_count, first_index, base_vertex, base_instance);
}

AGPU_EXPORT agpu_error agpuDrawElementsIndirect(agpu_command_list* command_list, agpu_size offset, agpu_size drawcount)
{
    CHECK_POINTER(command_list);
    return command_list->drawElementsIndirect(offset, drawcount);
}

AGPU_EXPORT agpu_error agpuDispatchCompute ( agpu_command_list* command_list, agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z )
{
    CHECK_POINTER(command_list);
    return command_list->dispatchCompute(group_count_x, group_count_y, group_count_z);
}

AGPU_EXPORT agpu_error agpuDispatchComputeIndirect ( agpu_command_list* command_list, agpu_size offset )
{
    CHECK_POINTER(command_list);
    return command_list->dispatchComputeIndirect(offset);
}

AGPU_EXPORT agpu_error agpuSetStencilReference(agpu_command_list* command_list, agpu_uint reference)
{
    CHECK_POINTER(command_list);
    return command_list->setStencilReference(reference);
}

AGPU_EXPORT agpu_error agpuCloseCommandList(agpu_command_list* command_list)
{
    CHECK_POINTER(command_list);
    return command_list->close();
}

AGPU_EXPORT agpu_error agpuExecuteBundle ( agpu_command_list* command_list, agpu_command_list* bundle )
{
    CHECK_POINTER(command_list);
    return command_list->executeBundle(bundle);
}

AGPU_EXPORT agpu_error agpuResetCommandList ( agpu_command_list* command_list, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
{
    CHECK_POINTER(command_list);
    return command_list->reset(allocator, initial_pipeline_state);
}

AGPU_EXPORT agpu_error agpuResetBundleCommandList ( agpu_command_list* command_list, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state, agpu_inheritance_info* inheritance_info )
{
    CHECK_POINTER(command_list);
    return command_list->resetBundleCommandList(allocator, initial_pipeline_state, inheritance_info);
}

AGPU_EXPORT agpu_error agpuBeginRenderPass ( agpu_command_list* command_list, agpu_renderpass *renderpass, agpu_framebuffer* framebuffer, agpu_bool bundle_content)
{
    CHECK_POINTER(command_list);
    return command_list->beginRenderPass(renderpass, framebuffer, bundle_content);
}

AGPU_EXPORT agpu_error agpuEndRenderPass(agpu_command_list* command_list)
{
    CHECK_POINTER(command_list);
    return command_list->endRenderPass();
}

AGPU_EXPORT agpu_error agpuResolveFramebuffer(agpu_command_list* command_list, agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer)
{
    CHECK_POINTER(command_list);
    return command_list->resolveFramebuffer(destFramebuffer, sourceFramebuffer);
}

AGPU_EXPORT agpu_error agpuPushConstants ( agpu_command_list* command_list, agpu_uint offset, agpu_uint size, agpu_pointer values )
{
    CHECK_POINTER(command_list);
    return command_list->pushConstants(offset, size, values);
}
