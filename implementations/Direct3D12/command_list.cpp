#include "command_allocator.hpp"
#include "command_list.hpp"

_agpu_command_list::_agpu_command_list()
{
    memset(clearColor, 0, sizeof(clearColor));
    clearDepth = 0;
    clearStencil = 0;
}

void _agpu_command_list::lostReferences()
{

}

_agpu_command_list *_agpu_command_list::create(agpu_device *device, _agpu_command_allocator *allocator, agpu_pipeline_state *initialState)
{
    ComPtr<ID3D12GraphicsCommandList> commandList;
    // TODO: Use the initial pipeline state.
    if (FAILED(device->d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator->allocator.Get(), nullptr, IID_PPV_ARGS(&commandList))))
        return nullptr;

    auto list = new _agpu_command_list();
    list->device = device;
    list->commandList = commandList;
    return list;
}

agpu_error _agpu_command_list::setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    return AGPU_UNIMPLEMENTED;
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
    if (buffers & AGPU_COLOR_BUFFER_BIT)
    {
        auto handle = device->renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += device->frameIndex * device->renderTargetViewDescriptorSize;
        commandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);
    }

    if (buffers & AGPU_DEPTH_BUFFER_BIT)
    {
        // TODO:
    }

    if (buffers & AGPU_STENCIL_BUFFER_BIT)
    {
        // TODO:
    }

    return AGPU_OK;
}

agpu_error _agpu_command_list::usePipelineState(agpu_pipeline_state* pipeline)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::useVertexBinding(agpu_vertex_binding* vertex_binding)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::useIndexBuffer(agpu_buffer* index_buffer)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::useDrawIndirectBuffer(agpu_buffer* draw_buffer)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::drawElementsIndirect(agpu_size offset)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::multiDrawElementsIndirect(agpu_size offset, agpu_size drawcount)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setStencilReference(agpu_float reference)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setAlphaReference(agpu_float reference)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setUniformi(agpu_int location, agpu_size count, agpu_int* data)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setUniform2i(agpu_int location, agpu_size count, agpu_int* data)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setUniform3i(agpu_int location, agpu_size count, agpu_int* data)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setUniform4i(agpu_int location, agpu_size count, agpu_int* data)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setUniformf(agpu_int location, agpu_size count, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setUniform2f(agpu_int location, agpu_size count, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setUniform3f(agpu_int location, agpu_size count, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setUniform4f(agpu_int location, agpu_size count, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setUniformMatrix2f(agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setUniformMatrix3f(agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setUniformMatrix4f(agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::close()
{
    ERROR_IF_FAILED(commandList->Close());
    return AGPU_OK;
}

agpu_error _agpu_command_list::reset(_agpu_command_allocator *allocator, agpu_pipeline_state* initial_pipeline_state)
{
    CHECK_POINTER(allocator);

    // TODO: Use the initial pipeline
    ERROR_IF_FAILED(commandList->Reset(allocator->allocator.Get(), nullptr));
    return AGPU_OK;
}

agpu_error _agpu_command_list::beginFrame()
{
    D3D12_RESOURCE_BARRIER barrier;
    memset(&barrier, 0, sizeof(barrier));
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = device->mainFrameBufferTargets[device->frameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    commandList->ResourceBarrier(1, &barrier);
    return AGPU_OK;
}

agpu_error _agpu_command_list::endFrame()
{
    D3D12_RESOURCE_BARRIER barrier;
    memset(&barrier, 0, sizeof(barrier));
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = device->mainFrameBufferTargets[device->frameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    commandList->ResourceBarrier(1, &barrier);
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
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuUseIndexBuffer(agpu_command_list* command_list, agpu_buffer* index_buffer)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuUseDrawIndirectBuffer(agpu_command_list* command_list, agpu_buffer* draw_buffer)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuDrawElementsIndirect(agpu_command_list* command_list, agpu_size offset)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuMultiDrawElementsIndirect(agpu_command_list* command_list, agpu_size offset, agpu_size drawcount)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetStencilReference(agpu_command_list* command_list, agpu_float reference)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetAlphaReference(agpu_command_list* command_list, agpu_float reference)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniformi(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_int* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniform2i(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_int* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniform3i(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_int* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniform4i(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_int* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniformf(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniform2f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniform3f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniform4f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniformMatrix2f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniformMatrix3f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetUniformMatrix4f(agpu_command_list* command_list, agpu_int location, agpu_size count, agpu_bool transpose, agpu_float* data)
{
    return AGPU_UNIMPLEMENTED;
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

AGPU_EXPORT agpu_error agpuBeginFrame(agpu_command_list* command_list)
{
    CHECK_POINTER(command_list);
    return command_list->beginFrame();
}

AGPU_EXPORT agpu_error agpuEndFrame(agpu_command_list* command_list)
{
    CHECK_POINTER(command_list);
    return command_list->endFrame();
}
