#include "command_allocator.hpp"
#include "command_list.hpp"
#include "common_commands.hpp"
#include "pipeline_state.hpp"
#include "buffer.hpp"
#include "vertex_binding.hpp"
#include "shader_signature.hpp"
#include "shader_resource_binding.hpp"
#include "framebuffer.hpp"
#include "renderpass.hpp"
#include "texture.hpp"

inline D3D_PRIMITIVE_TOPOLOGY mapPrimitiveTopology(agpu_primitive_topology topology)
{
    switch (topology)
    {
    case AGPU_POINTS: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
    case AGPU_LINES: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    case AGPU_LINES_ADJACENCY: return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
    case AGPU_LINE_STRIP: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
    case AGPU_LINE_STRIP_ADJACENCY:return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
    case AGPU_TRIANGLES: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case AGPU_TRIANGLES_ADJACENCY: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
    case AGPU_TRIANGLE_STRIP: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    case AGPU_TRIANGLE_STRIP_ADJACENCY: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
    default:
        abort();
    }
}

_agpu_command_list::_agpu_command_list()
{
    currentFramebuffer = nullptr;
}

void _agpu_command_list::lostReferences()
{
    if (currentFramebuffer)
        currentFramebuffer->release();
}

_agpu_command_list *_agpu_command_list::create(agpu_device *device, agpu_command_list_type type, _agpu_command_allocator *allocator, agpu_pipeline_state *initialState)
{
    ComPtr<ID3D12GraphicsCommandList> commandList;
    ID3D12PipelineState *state = nullptr;
    if (initialState)
        state = initialState->state.Get();

    if (FAILED(device->d3dDevice->CreateCommandList(0, mapCommandListType(type), allocator->allocator.Get(), state, IID_PPV_ARGS(&commandList))))
        return nullptr;

    if (initialState)
        commandList->IASetPrimitiveTopology(mapPrimitiveTopology(initialState->primitiveTopology));

    std::unique_ptr<agpu_command_list> list(new _agpu_command_list());
    list->type = type;
    list->device = device;
    list->commandList = commandList;
    if (list->setCommonState() < 0)
    {
        list->release();
        return nullptr;
    }

    return list.release();
}

agpu_error _agpu_command_list::setShaderSignature(agpu_shader_signature *signature)
{
    CHECK_POINTER(signature);
    ID3D12DescriptorHeap *heaps[2];
    int heapCount = 0;
    commandList->SetGraphicsRootSignature(signature->rootSignature.Get());

    if (signature->shaderResourceViewHeap)
        heaps[heapCount++] = signature->shaderResourceViewHeap.Get();
    if (signature->samplerHeap)
        heaps[heapCount++] = signature->samplerHeap.Get();
    if(heapCount)
        commandList->SetDescriptorHeaps(heapCount, heaps);

    return AGPU_OK;
}

agpu_error _agpu_command_list::setCommonState()
{
    if (currentFramebuffer)
    {
        currentFramebuffer->release();
        currentFramebuffer = nullptr;
    }

    return AGPU_OK;
}

agpu_error _agpu_command_list::setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    D3D12_VIEWPORT viewport;
    viewport.TopLeftX = (FLOAT)x;
    viewport.TopLeftY = (FLOAT)y;
    viewport.Width = (FLOAT)w;
    viewport.Height = (FLOAT)h;
    viewport.MinDepth = 0.0;
    viewport.MaxDepth = 1.0;
    commandList->RSSetViewports(1, &viewport);
    return AGPU_OK;
}

agpu_error _agpu_command_list::setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    RECT rect;
    rect.top = x;
    rect.left = y;
    rect.right = x + w;
    rect.bottom = y + h;
    commandList->RSSetScissorRects(1, &rect);
    return AGPU_OK;
}

agpu_error _agpu_command_list::usePipelineState(agpu_pipeline_state* pipeline)
{
    CHECK_POINTER(pipeline);
    commandList->SetPipelineState(pipeline->state.Get());
    commandList->IASetPrimitiveTopology(mapPrimitiveTopology(pipeline->primitiveTopology));

    return AGPU_OK;
}

agpu_error _agpu_command_list::useVertexBinding(agpu_vertex_binding* vertex_binding)
{
    CHECK_POINTER(vertex_binding);
    commandList->IASetVertexBuffers(0, (UINT)vertex_binding->vertexBuffers.size(), &vertex_binding->vertexBufferViews[0]);
    return AGPU_OK;
}

agpu_error _agpu_command_list::useIndexBuffer(agpu_buffer* index_buffer)
{
    if (index_buffer->description.binding != AGPU_ELEMENT_ARRAY_BUFFER)
        return AGPU_ERROR;

    commandList->IASetIndexBuffer(&index_buffer->view.indexBuffer);
    return AGPU_OK;
}

agpu_error _agpu_command_list::useDrawIndirectBuffer(agpu_buffer* draw_buffer)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::useShaderResources(agpu_shader_resource_binding* binding)
{
    CHECK_POINTER(binding);

    ID3D12DescriptorHeap *heap = nullptr;
    if (binding->type == AGPU_SHADER_BINDING_TYPE_SAMPLER)
        heap = binding->signature->samplerHeap.Get();
    else
        heap = binding->signature->shaderResourceViewHeap.Get();

    auto desc = heap->GetGPUDescriptorHandleForHeapStart();
    desc.ptr += binding->descriptorOffset;
    if(binding->isBank)
        commandList->SetGraphicsRootDescriptorTable(binding->element, desc);
    else
        return AGPU_UNIMPLEMENTED;
    return AGPU_OK;
}

agpu_error _agpu_command_list::drawArrays(agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance)
{
    commandList->DrawInstanced(vertex_count, instance_count, first_vertex, base_instance);
    return AGPU_OK;
}

agpu_error _agpu_command_list::drawElements(agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance)
{
    commandList->DrawIndexedInstanced(index_count, instance_count, first_index, base_vertex, base_instance);
    return AGPU_OK;
}

agpu_error _agpu_command_list::drawElementsIndirect(agpu_size offset)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::multiDrawElementsIndirect(agpu_size offset, agpu_size drawcount)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setStencilReference(agpu_uint reference)
{
    commandList->OMSetStencilRef(reference);
    return AGPU_OK;
}

agpu_error _agpu_command_list::executeBundle(agpu_command_list* bundle)
{
    CHECK_POINTER(bundle);
    if (bundle->type != AGPU_COMMAND_LIST_TYPE_BUNDLE)
        return AGPU_INVALID_PARAMETER;

    commandList->ExecuteBundle(bundle->commandList.Get());
    return AGPU_OK;
}

agpu_error _agpu_command_list::close()
{
    ERROR_IF_FAILED(commandList->Close());
    return AGPU_OK;
}

agpu_error _agpu_command_list::reset(_agpu_command_allocator *allocator, agpu_pipeline_state* initial_pipeline_state)
{
    CHECK_POINTER(allocator);

    ID3D12PipelineState *state = nullptr;
    if (initial_pipeline_state)
        state = initial_pipeline_state->state.Get();

    ERROR_IF_FAILED(commandList->Reset(allocator->allocator.Get(), state));

    if (initial_pipeline_state)
        commandList->IASetPrimitiveTopology(mapPrimitiveTopology(initial_pipeline_state->primitiveTopology));

    return setCommonState();
}

agpu_error _agpu_command_list::beginRenderPass(agpu_renderpass *renderpass, agpu_framebuffer* framebuffer, agpu_bool secondaryContent)
{
    CHECK_POINTER(renderpass);
    CHECK_POINTER(framebuffer);
    if(framebuffer)
        framebuffer->retain();
    if (currentFramebuffer)
        currentFramebuffer->release();
    currentFramebuffer = framebuffer;
    if (!currentFramebuffer)
        return AGPU_OK;

    // TODO: Use a more proper state depending if this is used as a texture or not.
    D3D12_RESOURCE_STATES prevState = framebuffer->swapChainBuffer ? D3D12_RESOURCE_STATE_PRESENT : D3D12_RESOURCE_STATE_GENERIC_READ;

    // Perform the resource transitions
    for (size_t i = 0; i < framebuffer->getColorBufferCount(); ++i)
    {
        auto colorBuffer = framebuffer->colorBuffers[i];
        if (!colorBuffer)
            return AGPU_ERROR;

        auto barrier = resourceTransitionBarrier(colorBuffer->gpuResource.Get(), prevState, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->ResourceBarrier(1, &barrier);
    }

    if (framebuffer->depthStencil)
    {
        auto desc = framebuffer->getDepthStencilCpuHandle();
        commandList->OMSetRenderTargets((UINT)framebuffer->colorBufferDescriptors.size(), &framebuffer->colorBufferDescriptors[0], FALSE, &desc);
    }
    else
    {
        commandList->OMSetRenderTargets((UINT)framebuffer->colorBufferDescriptors.size(), &framebuffer->colorBufferDescriptors[0], FALSE, nullptr);
    }

    // Clear the color buffers
    for (size_t i = 0; i < currentFramebuffer->getColorBufferCount(); ++i)
    {
        if (!currentFramebuffer->colorBuffers[i])
            return AGPU_ERROR;

        auto handle = currentFramebuffer->getColorBufferCpuHandle(i);
        auto &attachment = renderpass->colorAttachments[i];
        if(attachment.begin_action == AGPU_ATTACHMENT_CLEAR)
            commandList->ClearRenderTargetView(handle, reinterpret_cast<FLOAT*> (&attachment.clear_value), 0, nullptr);
    }

    if ((renderpass->hasDepth || renderpass->hasStencil) && renderpass->depthStencilAttachment.begin_action == AGPU_ATTACHMENT_CLEAR)
    {
        D3D12_CLEAR_FLAGS flags = D3D12_CLEAR_FLAGS(0);
        if (renderpass->hasDepth)
            flags |= D3D12_CLEAR_FLAG_DEPTH;
        if (renderpass->hasStencil)
            flags |= D3D12_CLEAR_FLAG_STENCIL;

        auto clearDepth = renderpass->depthStencilAttachment.clear_value.depth;
        auto clearStencil = renderpass->depthStencilAttachment.clear_value.stencil;
        commandList->ClearDepthStencilView(currentFramebuffer->getDepthStencilCpuHandle(), flags, clearDepth, clearStencil, 0, nullptr);
    }

    return AGPU_OK;
}

agpu_error _agpu_command_list::endRenderPass()
{
    if (!currentFramebuffer)
        return AGPU_OK;

    commandList->OMSetRenderTargets(0, nullptr, FALSE, nullptr);

    // TODO: Use a more proper state depending if this is used as a texture or not.
    D3D12_RESOURCE_STATES newState = currentFramebuffer->swapChainBuffer ? D3D12_RESOURCE_STATE_PRESENT : D3D12_RESOURCE_STATE_GENERIC_READ;

    // Perform the resource transitions
    for (size_t i = 0; i < currentFramebuffer->getColorBufferCount(); ++i)
    {
        auto &colorBuffer = currentFramebuffer->colorBuffers[i];
        if (!colorBuffer)
            return AGPU_ERROR;

        auto barrier = resourceTransitionBarrier(colorBuffer->gpuResource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, newState);
        commandList->ResourceBarrier(1, &barrier);
    }

    currentFramebuffer->release();
    currentFramebuffer = nullptr;
    return AGPU_OK;
}

agpu_error _agpu_command_list::resolveFramebuffer(agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer)
{
    CHECK_POINTER(destFramebuffer);
    CHECK_POINTER(sourceFramebuffer);
    if (destFramebuffer == sourceFramebuffer ||
        destFramebuffer->getColorBufferCount() != sourceFramebuffer->getColorBufferCount())
        return AGPU_INVALID_PARAMETER;

    D3D12_RESOURCE_STATES destState = destFramebuffer->swapChainBuffer ? D3D12_RESOURCE_STATE_PRESENT : D3D12_RESOURCE_STATE_GENERIC_READ;
    D3D12_RESOURCE_STATES sourceState = sourceFramebuffer->swapChainBuffer ? D3D12_RESOURCE_STATE_PRESENT : D3D12_RESOURCE_STATE_GENERIC_READ;

    // Go to resolve state
    for (size_t i = 0; i < destFramebuffer->getColorBufferCount(); ++i)
    {
        auto &destColorBuffer = destFramebuffer->colorBuffers[i];
        auto &sourceColorBuffer = sourceFramebuffer->colorBuffers[i];
        if (!destColorBuffer || !sourceColorBuffer)
            return AGPU_ERROR;

        {
            D3D12_RESOURCE_BARRIER barriers[2] = {
                resourceTransitionBarrier(destColorBuffer->gpuResource.Get(), destState, D3D12_RESOURCE_STATE_RESOLVE_DEST),
                resourceTransitionBarrier(sourceColorBuffer->gpuResource.Get(), sourceState, D3D12_RESOURCE_STATE_RESOLVE_SOURCE)
            };
            commandList->ResourceBarrier(2, barriers);
        }

        commandList->ResolveSubresource(destColorBuffer->gpuResource.Get(), 0, sourceColorBuffer->gpuResource.Get(), 0, (DXGI_FORMAT)sourceColorBuffer->description.format);

        {
            D3D12_RESOURCE_BARRIER barriers[2] = {
                resourceTransitionBarrier(destColorBuffer->gpuResource.Get(), D3D12_RESOURCE_STATE_RESOLVE_DEST, destState),
                resourceTransitionBarrier(sourceColorBuffer->gpuResource.Get(), D3D12_RESOURCE_STATE_RESOLVE_SOURCE, sourceState)
            };
            commandList->ResourceBarrier(2, barriers);
        }
    }

    return AGPU_OK;
}

// Exported C interface
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

AGPU_EXPORT agpu_error agpuSetShaderSignature(agpu_command_list* command_list, agpu_shader_signature *signature)
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

AGPU_EXPORT agpu_error agpuUseShaderResources(agpu_command_list* command_list, agpu_shader_resource_binding* binding)
{
    CHECK_POINTER(command_list);
    return command_list->useShaderResources(binding);
}

AGPU_EXPORT agpu_error agpuDrawArrays(agpu_command_list* command_list, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance)
{
    CHECK_POINTER(command_list);
    return command_list->drawArrays(vertex_count, instance_count, first_vertex, base_instance);
}

AGPU_EXPORT agpu_error agpuDrawElements(agpu_command_list* command_list, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance)
{
    CHECK_POINTER(command_list);
    return command_list->drawElements(index_count, instance_count, first_index, base_vertex, base_instance);
}

AGPU_EXPORT agpu_error agpuDrawElementsIndirect(agpu_command_list* command_list, agpu_size offset)
{
    CHECK_POINTER(command_list);
    return command_list->drawElementsIndirect(offset);
}

AGPU_EXPORT agpu_error agpuMultiDrawElementsIndirect(agpu_command_list* command_list, agpu_size offset, agpu_size drawcount)
{
    CHECK_POINTER(command_list);
    return command_list->multiDrawElementsIndirect(offset, drawcount);
}

AGPU_EXPORT agpu_error agpuSetStencilReference(agpu_command_list* command_list, agpu_uint reference)
{
    CHECK_POINTER(command_list);
    return command_list->setStencilReference(reference);
}

AGPU_EXPORT agpu_error agpuExecuteBundle(agpu_command_list* command_list, agpu_command_list* bundle)
{
    CHECK_POINTER(command_list);
    return command_list->executeBundle(bundle);
}

AGPU_EXPORT agpu_error agpuCloseCommandList(agpu_command_list* command_list)
{
    CHECK_POINTER(command_list);
    return command_list->close();
}

AGPU_EXPORT agpu_error agpuResetCommandList(agpu_command_list* command_list, _agpu_command_allocator *allocator, agpu_pipeline_state* initial_pipeline_state)
{
    CHECK_POINTER(command_list);
    return command_list->reset(allocator, initial_pipeline_state);
}

AGPU_EXPORT agpu_error agpuBeginRenderPass(agpu_command_list* command_list, agpu_renderpass* renderpass, agpu_framebuffer* framebuffer, agpu_bool bundle_content)
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
