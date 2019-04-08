#include "command_list.hpp"
#include "pipeline_state.hpp"
#include "vertex_binding.hpp"
#include "buffer.hpp"
#include "framebuffer.hpp"
#include "renderpass.hpp"
#include "shader_resource_binding.hpp"
#include <string.h>

namespace AgpuGL
{

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
{
    memset(pushConstantBuffer, 0, sizeof(pushConstantBuffer));
    reset();
}

CommandListExecutionContext::~CommandListExecutionContext()
{
}

void CommandListExecutionContext::reset()
{
    currentPipeline.reset();
    currentComputePipeline.reset();
    activePipeline.reset();

    for(auto &binding : shaderResourceBindings)
        binding.reset();

    for(auto &binding : computeShaderResourceBindings)
        binding.reset();

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
        activePipeline = currentPipeline;

        if(activePipeline)
        {
            auto glActivePipeline = activePipeline.as<GLPipelineState> ();
            glActivePipeline->activate();
			if(glActivePipeline->extraStateData)
				primitiveMode = mapPrimitiveTopology(glActivePipeline->extraStateData->getPrimitiveTopology());
            if (stencilReference != 0)
                glActivePipeline->updateStencilReference(stencilReference);
        }
        else
        {
            deviceForGL->glUseProgram(0);
        }
    }

    if(activePipeline && !hasValidShaderResources)
    {
        hasValidShaderResources = true;
        hasValidComputeShaderResources = false;
        activePipeline.as<GLPipelineState> ()->activateShaderResourcesOn(this, shaderResourceBindings);
    }

    if(!hasValidGraphicsPushConstants)
    {
        activePipeline.as<GLPipelineState> ()->uploadPushConstants(pushConstantBuffer, sizeof(pushConstantBuffer));
        hasValidGraphicsPushConstants = true;
    }
}

void CommandListExecutionContext::setBaseInstance(agpu_uint base_instance)
{
    if(activePipeline)
        activePipeline.as<GLPipelineState> ()->setBaseInstance(base_instance);
}

void CommandListExecutionContext::validateBeforeComputeDispatch()
{
    if(!hasValidActivePipeline || activePipeline != currentComputePipeline)
    {
        hasValidActivePipeline = true;
        hasValidShaderResources = false;
        hasValidComputeShaderResources = false;
        activePipeline = currentComputePipeline;

        if(activePipeline)
        {
            activePipeline.as<GLPipelineState> ()->activate();
        }
        else
        {
            deviceForGL->glUseProgram(0);
        }
    }

    if(activePipeline && !hasValidComputeShaderResources)
    {
        hasValidComputeShaderResources = true;
        hasValidShaderResources = false;
        activePipeline.as<GLPipelineState> ()->activateShaderResourcesOn(this, computeShaderResourceBindings);
    }

    if(!hasValidComputePushConstants)
    {
        activePipeline.as<GLPipelineState> ()->uploadPushConstants(pushConstantBuffer, sizeof(pushConstantBuffer));
        hasValidComputePushConstants = true;
    }
}

void CommandListExecutionContext::usePipelineState(const agpu::pipeline_state_ref &pipeline)
{
    currentPipeline = pipeline;
}

void CommandListExecutionContext::useComputePipelineState(const agpu::pipeline_state_ref &pipeline)
{
    currentComputePipeline = pipeline;
}

void CommandListExecutionContext::setStencilReference(agpu_uint reference)
{
    if (activePipeline)
        activePipeline.as<GLPipelineState> ()->updateStencilReference(reference);
    stencilReference = reference;
}

void CommandListExecutionContext::useShaderResources(const agpu::shader_resource_binding_ref &binding)
{
    auto elementIndex = size_t(binding.as<GLShaderResourceBinding> ()->elementIndex);
    if(elementIndex >= MaxNumberOfShaderResourceBindings)
        return;

    shaderResourceBindings[elementIndex] = binding;
    hasValidShaderResources = false;
}

void CommandListExecutionContext::useComputeShaderResources (const agpu::shader_resource_binding_ref &binding )
{
    auto elementIndex = size_t(binding.as<GLShaderResourceBinding> ()->elementIndex);
    if(elementIndex >= MaxNumberOfShaderResourceBindings)
        return;

    computeShaderResourceBindings[elementIndex] = binding;
    hasValidComputeShaderResources = false;
}

GLCommandList::GLCommandList()
{
    closed = false;
}

GLCommandList::~GLCommandList()
{
}

agpu::command_list_ref GLCommandList::create(const agpu::device_ref &device, agpu_command_list_type type, const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initial_pipeline_state)
{
    auto result = agpu::makeObject<GLCommandList> ();
    auto list = result.as<GLCommandList> ();
    list->device = device;
    list->executionContext.device = device;
    list->type = type;;
    if(initial_pipeline_state)
        list->usePipelineState(initial_pipeline_state);
    return result;
}

agpu_error GLCommandList::setShaderSignature(const agpu::shader_signature_ref &signature)
{
    return AGPU_OK;
}

agpu_error GLCommandList::setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    return addCommand([=] {
        glViewport(x, y, w, h);
    });
}

agpu_error GLCommandList::setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    return addCommand([=] {
        glScissor(x, y, w, h);
    });
}

agpu_error GLCommandList::usePipelineState(const agpu::pipeline_state_ref &pipeline)
{
	switch (pipeline.as<GLPipelineState>()->type)
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

agpu_error GLCommandList::useVertexBinding(const agpu::vertex_binding_ref &vertex_binding)
{
    return addCommand([=] {
        this->currentVertexBinding = vertex_binding;
    });
}

agpu_error GLCommandList::useIndexBuffer(const agpu::buffer_ref &index_buffer)
{
    return addCommand([=] {
        this->currentIndexBuffer= index_buffer;
    });
}

agpu_error GLCommandList::useDrawIndirectBuffer(const agpu::buffer_ref &draw_buffer)
{
    return addCommand([=] {
        this->currentDrawBuffer = draw_buffer;
    });
}

agpu_error GLCommandList::useComputeDispatchIndirectBuffer(const agpu::buffer_ref &buffer)
{
    return addCommand([=] {
        this->currentComputeDispatchBuffer = buffer;
    });
}

agpu_error GLCommandList::useShaderResources(const agpu::shader_resource_binding_ref &binding)
{
    CHECK_POINTER(binding);
    return addCommand([=] {
        executionContext.useShaderResources(binding);
    });
}

agpu_error GLCommandList::useComputeShaderResources(const agpu::shader_resource_binding_ref &binding)
{
    CHECK_POINTER(binding);
    return addCommand([=] {
        executionContext.useComputeShaderResources(binding);
    });
}

agpu_error GLCommandList::pushConstants(agpu_uint offset, agpu_uint size, agpu_pointer values)
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

agpu_error GLCommandList::drawArrays(agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance)
{
    return addCommand([=] {
        if (!currentVertexBinding)
            return;

        currentVertexBinding.as<GLVertexBinding> ()->bind();
        executionContext.validateBeforeDrawCall();
        executionContext.setBaseInstance(base_instance);

        deviceForGL->glDrawArraysInstancedBaseInstance(executionContext.primitiveMode, first_vertex, vertex_count, instance_count, base_instance);
    });
}

agpu_error GLCommandList::drawArraysIndirect(agpu_size offset, agpu_size drawcount)
{
    return addCommand([=] {
        if (!currentVertexBinding || !currentDrawBuffer)
            return;

        currentVertexBinding.as<GLVertexBinding> ()->bind();
        currentDrawBuffer.as<GLBuffer> ()->bind();
        executionContext.validateBeforeDrawCall();
        executionContext.setBaseInstance(0);

        if(drawcount > 1)
            deviceForGL->glMultiDrawArraysIndirect(executionContext.primitiveMode, reinterpret_cast<void*> ((size_t)offset), (GLsizei)drawcount, currentDrawBuffer.as<GLBuffer> ()->description.stride);
        else
            deviceForGL->glDrawArraysIndirect(executionContext.primitiveMode, reinterpret_cast<void*> ((size_t)offset));
    });
}

agpu_error GLCommandList::drawElements(agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance)
{
    return addCommand([=] {
        if (!currentVertexBinding || !currentIndexBuffer)
            return;

        currentVertexBinding.as<GLVertexBinding> ()->bind();
        currentIndexBuffer.as<GLBuffer> ()->bind();
        executionContext.validateBeforeDrawCall();
        executionContext.setBaseInstance(base_instance);

        size_t stride = currentIndexBuffer.as<GLBuffer> ()->description.stride;
        size_t offset = stride*first_index;
        deviceForGL->glDrawElementsInstancedBaseVertexBaseInstance(executionContext.primitiveMode, index_count,
            mapIndexType(stride), reinterpret_cast<void*> (offset),
            instance_count, base_vertex, base_instance);
    });
}

agpu_error GLCommandList::drawElementsIndirect(agpu_size offset, agpu_size drawcount)
{
    return addCommand([=] {
        if (!currentVertexBinding || !currentIndexBuffer || !currentDrawBuffer)
            return;

        currentVertexBinding.as<GLVertexBinding> ()->bind();
        currentIndexBuffer.as<GLBuffer> ()->bind();
        currentDrawBuffer.as<GLBuffer> ()->bind();
        executionContext.validateBeforeDrawCall();
        executionContext.setBaseInstance(0);

        if(drawcount > 1)
            deviceForGL->glMultiDrawElementsIndirect(executionContext.primitiveMode, mapIndexType(currentIndexBuffer.as<GLBuffer> ()->description.stride), reinterpret_cast<void*> ((size_t)offset), (GLsizei)drawcount, currentDrawBuffer.as<GLBuffer> ()->description.stride);
        else
            deviceForGL->glDrawElementsIndirect(executionContext.primitiveMode, mapIndexType(currentIndexBuffer.as<GLBuffer> ()->description.stride), reinterpret_cast<void*> ((size_t)offset));
    });
}

agpu_error GLCommandList::dispatchCompute(agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z)
{
    return addCommand([=] {
        executionContext.validateBeforeComputeDispatch();

        deviceForGL->glDispatchCompute(group_count_x, group_count_y, group_count_z);
    });
}

agpu_error GLCommandList::dispatchComputeIndirect(agpu_size offset)
{
    return addCommand([=] {
        if(!currentComputeDispatchBuffer)
            return;

        currentComputeDispatchBuffer.as<GLBuffer> ()->bind();
        executionContext.validateBeforeComputeDispatch();

        deviceForGL->glDispatchComputeIndirect(offset);
    });
}

agpu_error GLCommandList::setStencilReference(agpu_uint reference)
{
    return addCommand([=] {
        executionContext.setStencilReference(reference);
    });
}

agpu_error GLCommandList::executeBundle(const agpu::command_list_ref &bundle)
{
    CHECK_POINTER(bundle)
    if(bundle.as<GLCommandList> ()->type != AGPU_COMMAND_LIST_TYPE_BUNDLE)
        return AGPU_INVALID_PARAMETER;

    return addCommand([=] {
        bundle.as<GLCommandList> ()->execute();
    });
}

agpu_error GLCommandList::close()
{
    closed = true;
    return AGPU_OK;
}

agpu_error GLCommandList::reset(const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initial_pipeline_state)
{
    closed = false;
    commands.clear();
    if (initial_pipeline_state)
        usePipelineState(initial_pipeline_state);
    return AGPU_OK;
}


agpu_error GLCommandList::resetBundle(const agpu::command_allocator_ref & allocator, const agpu::pipeline_state_ref & initial_pipeline_state, agpu_inheritance_info* inheritance_info)
{
    closed = false;
    commands.clear();
    if (initial_pipeline_state)
        usePipelineState(initial_pipeline_state);
    return AGPU_OK;
}

agpu_error GLCommandList::beginRenderPass(const agpu::renderpass_ref &renderpass, const agpu::framebuffer_ref &framebuffer, agpu_bool bundle_content)
{
    CHECK_POINTER(framebuffer)
    return addCommand([=] {
        auto glFramebuffer = framebuffer.as<GLFramebuffer>();
        glFramebuffer->bind();
        glViewport(0, 0, glFramebuffer->width, glFramebuffer->height);
        glScissor(0, 0, glFramebuffer->width, glFramebuffer->height);
        renderpass.as<GLRenderPass>()->started();
    });
}

agpu_error GLCommandList::endRenderPass()
{
    return addCommand([=] {
        deviceForGL->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    });
    return AGPU_OK;
}

agpu_error GLCommandList::addCommand(const AgpuGLCommand &command)
{
    if (closed)
        return AGPU_COMMAND_LIST_CLOSED;

    commands.push_back(command);
    return AGPU_OK;
}

void GLCommandList::execute()
{
    currentVertexBinding.reset();
    currentIndexBuffer.reset();
    currentDrawBuffer.reset();
    currentComputeDispatchBuffer.reset();
    for (auto &command : commands)
        command();

    executionContext.reset();
}

agpu_error GLCommandList::resolveFramebuffer(const agpu::framebuffer_ref &destFramebuffer, const agpu::framebuffer_ref &sourceFramebuffer)
{
    CHECK_POINTER(destFramebuffer);
    CHECK_POINTER(sourceFramebuffer);

    return addCommand([=] {
        destFramebuffer.as<GLFramebuffer>()->bind(GL_DRAW_FRAMEBUFFER);
        sourceFramebuffer.as<GLFramebuffer>()->bind(GL_READ_FRAMEBUFFER);
        deviceForGL->glBlitFramebuffer(
            0, 0, sourceFramebuffer.as<GLFramebuffer>()->width, sourceFramebuffer.as<GLFramebuffer>()->height,
            0, 0, destFramebuffer.as<GLFramebuffer>()->width, destFramebuffer.as<GLFramebuffer>()->height,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
    });

    return AGPU_OK;
}

agpu_error GLCommandList::resolveTexture(const agpu::texture_ref &sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, const agpu::texture_ref & destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect)
{
    // TODO: Implement this.
    return AGPU_OK;
}

} // End of namespace AgpuGL
