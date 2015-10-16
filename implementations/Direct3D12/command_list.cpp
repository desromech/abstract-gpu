#include "command_allocator.hpp"
#include "command_list.hpp"
#include "common_commands.hpp"
#include "pipeline_state.hpp"
#include "buffer.hpp"
#include "vertex_binding.hpp"
#include "shader_signature.hpp"
#include "shader_resource_binding.hpp"
#include "framebuffer.hpp"
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

inline D3D12_COMMAND_LIST_TYPE mapCommandListType(CommandListType type)
{
    switch (type)
    {
    case CommandListType::Direct: return D3D12_COMMAND_LIST_TYPE_DIRECT;
    case CommandListType::Bundle: return D3D12_COMMAND_LIST_TYPE_BUNDLE;
    case CommandListType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
    default: abort();
    }
}

_agpu_command_list *_agpu_command_list::create(agpu_device *device, CommandListType type, _agpu_command_allocator *allocator, agpu_pipeline_state *initialState)
{
    ComPtr<ID3D12GraphicsCommandList> commandList;
    ID3D12PipelineState *state = nullptr;
    if (initialState)
        state = initialState->state.Get();

    if (FAILED(device->d3dDevice->CreateCommandList(0, mapCommandListType(type), allocator->allocator.Get(), state, IID_PPV_ARGS(&commandList))))
        return nullptr;

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
    // Reset some state
    memset(clearColor, 0, sizeof(clearColor));
    clearDepth = 1;
    clearStencil = 0;

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

agpu_error _agpu_command_list::setClearColor(agpu_float r, agpu_float g, agpu_float b, agpu_float a)
{
    clearColor[0] = r;
    clearColor[1] = g;
    clearColor[2] = b;
    clearColor[3] = a;
    return AGPU_OK;
}

agpu_error _agpu_command_list::setClearDepth(agpu_float depth)
{
    clearDepth = depth;
    return AGPU_OK;
}

agpu_error _agpu_command_list::setClearStencil(agpu_int value)
{
    clearStencil = value;
    return AGPU_OK;
}

agpu_error _agpu_command_list::clear(agpu_bitfield buffers)
{
    if (!currentFramebuffer)
        return AGPU_OK;

    // Clear the color buffers
    if (buffers & AGPU_COLOR_BUFFER_BIT)
    {
        for (size_t i = 0; i < currentFramebuffer->getColorBufferCount(); ++i)
        {
            if (!currentFramebuffer->colorBuffers[i])
                return AGPU_ERROR;

            auto handle = currentFramebuffer->getColorBufferCpuHandle(i);
            commandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);
        }
    }

    // Clear the depth stencil attachment.
    if (currentFramebuffer->depthStencil && ((buffers & AGPU_DEPTH_BUFFER_BIT) || buffers & AGPU_STENCIL_BUFFER_BIT))
    {
        D3D12_CLEAR_FLAGS flags = D3D12_CLEAR_FLAGS(0);
        if (buffers & AGPU_DEPTH_BUFFER_BIT)
            flags = D3D12_CLEAR_FLAG_DEPTH;
        if (buffers & AGPU_STENCIL_BUFFER_BIT)
            flags = D3D12_CLEAR_FLAG_STENCIL;
        commandList->ClearDepthStencilView(currentFramebuffer->getDepthStencilCpuHandle(), flags, clearDepth, clearStencil, 0, nullptr);
    }

    return AGPU_OK;
}

agpu_error _agpu_command_list::usePipelineState(agpu_pipeline_state* pipeline)
{
    CHECK_POINTER(pipeline);
    commandList->SetPipelineState(pipeline->state.Get());
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

agpu_error _agpu_command_list::setPrimitiveTopology(agpu_primitive_topology topology)
{
    commandList->IASetPrimitiveTopology(mapPrimitiveTopology(topology));
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
    if (bundle->type != CommandListType::Bundle)
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

    return setCommonState();
}

agpu_error _agpu_command_list::beginFrame(agpu_framebuffer* framebuffer)
{
    if(framebuffer)
        framebuffer->retain();
    if (currentFramebuffer)
        currentFramebuffer->release();
    currentFramebuffer = framebuffer;
    if (!currentFramebuffer)
        return AGPU_OK;

    // TODO: Use a more proper state depending if this is used as a texture or not.
    D3D12_RESOURCE_STATES prevState = D3D12_RESOURCE_STATE_PRESENT;

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

    return AGPU_OK;
}

agpu_error _agpu_command_list::endFrame()
{
    if (!currentFramebuffer)
        return AGPU_OK;

    // TODO: Use a more proper state depending if this is used as a texture or not.
    D3D12_RESOURCE_STATES newState = D3D12_RESOURCE_STATE_PRESENT;

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

AGPU_EXPORT agpu_error agpuSetClearColor(agpu_command_list* command_list, agpu_float r, agpu_float g, agpu_float b, agpu_float a)
{
    CHECK_POINTER(command_list);
    return command_list->setClearColor(r, g, b, a);
}

AGPU_EXPORT agpu_error agpuSetClearDepth(agpu_command_list* command_list, agpu_float depth)
{
    CHECK_POINTER(command_list);
    return command_list->setClearDepth(depth);
}

AGPU_EXPORT agpu_error agpuSetClearStencil(agpu_command_list* command_list, agpu_int value)
{
    CHECK_POINTER(command_list);
    return command_list->setClearStencil(value);
}

AGPU_EXPORT agpu_error agpuClear(agpu_command_list* command_list, agpu_bitfield buffers)
{
    CHECK_POINTER(command_list);
    return command_list->clear(buffers);
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

AGPU_EXPORT agpu_error agpuSetPrimitiveTopology(agpu_command_list* command_list, agpu_primitive_topology topology)
{
    CHECK_POINTER(command_list);
    return command_list->setPrimitiveTopology(topology);
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

AGPU_EXPORT agpu_error agpuBeginFrame(agpu_command_list* command_list, agpu_framebuffer* framebuffer)
{
    CHECK_POINTER(command_list);
    return command_list->beginFrame(framebuffer);
}

AGPU_EXPORT agpu_error agpuEndFrame(agpu_command_list* command_list)
{
    CHECK_POINTER(command_list);
    return command_list->endFrame();
}
