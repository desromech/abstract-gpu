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
#include "constants.hpp"

namespace AgpuD3D12
{

ADXCommandList::ADXCommandList(const agpu::device_ref &cdevice)
    : device(cdevice)
{
}

ADXCommandList::~ADXCommandList()
{
}

agpu::command_list_ref ADXCommandList::create(const agpu::device_ref &device, agpu_command_list_type type, const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initialState)
{
    ComPtr<ID3D12GraphicsCommandList> commandList;
    ID3D12PipelineState *state = nullptr;
    if (initialState)
    {
        state = initialState.as<ADXPipelineState> ()->state.Get();
    }

    if (FAILED(deviceForDX->d3dDevice->CreateCommandList(0, mapCommandListType(type), allocator.as<ADXCommandAllocator> ()->allocator.Get(), state, IID_PPV_ARGS(&commandList))))
        return agpu::command_list_ref();

    if (initialState)
		initialState.as<ADXPipelineState> ()->activatedOnCommandList(commandList);

    auto result = agpu::makeObject<ADXCommandList> (device);
    auto adxList = result.as<ADXCommandList> ();
    adxList->type = type;
    adxList->commandList = commandList;
    if (adxList->setCommonState() < 0)
    {
        return agpu::command_list_ref();
    }

    return result;
}

agpu_error ADXCommandList::setShaderSignature(const agpu::shader_signature_ref &signature)
{
    CHECK_POINTER(signature);
    ID3D12DescriptorHeap *heaps[2];
    int heapCount = 0;
    auto adxSignature = signature.as<ADXShaderSignature> ();

	bool hasGraphicsSignature = type == AGPU_COMMAND_LIST_TYPE_DIRECT || type == AGPU_COMMAND_LIST_TYPE_BUNDLE;
	bool hasComputeSignature = type == AGPU_COMMAND_LIST_TYPE_DIRECT || type == AGPU_COMMAND_LIST_TYPE_BUNDLE || type == AGPU_COMMAND_LIST_TYPE_COMPUTE;
	if(hasGraphicsSignature)
	    commandList->SetGraphicsRootSignature(adxSignature->rootSignature.Get());
	if (hasComputeSignature)
		commandList->SetComputeRootSignature(adxSignature->rootSignature.Get());

    if (adxSignature->shaderResourceViewHeap)
        heaps[heapCount++] = adxSignature->shaderResourceViewHeap.Get();
    if (adxSignature->samplerHeap)
        heaps[heapCount++] = adxSignature->samplerHeap.Get();
	if (heapCount > 0)
	{
		commandList->SetDescriptorHeaps(heapCount, heaps);
		for (size_t i = 0; i < adxSignature->nullDescriptorTables.size(); ++i)
		{
			if(hasGraphicsSignature)
				commandList->SetGraphicsRootDescriptorTable(i, adxSignature->nullDescriptorTables[i]);
			if (hasComputeSignature)
				commandList->SetComputeRootDescriptorTable(i, adxSignature->nullDescriptorTables[i]);
		}
	}

    return AGPU_OK;
}

agpu_error ADXCommandList::setCommonState()
{
    currentFramebuffer.reset();

    return AGPU_OK;
}

agpu_buffer_usage_mask ADXCommandList::getCurrentBufferUsageMode(const agpu::buffer_ref& buffer)
{
	for (auto it = bufferTransitionStack.rbegin(); it != bufferTransitionStack.rend(); ++it)
	{
		if (it->first == buffer)
			return it->second;
	}

	return buffer.as<ADXBuffer>()->description.main_usage_mode;
}

agpu_texture_usage_mode_mask ADXCommandList::getCurrentTextureUsageMode(const agpu::texture_ref& texture)
{
	return texture.as<ADXTexture>()->description.main_usage_mode;
}

void ADXCommandList::transitionTextureUsageMode(ID3D12Resource *resource, agpu_memory_heap_type heapType, agpu_texture_usage_mode_mask sourceMode, agpu_texture_usage_mode_mask destinationMode)
{
	if (sourceMode == destinationMode)
		return;

	auto sourceState = mapTextureUsageToResourceState(heapType, sourceMode);
	auto destinationState = mapTextureUsageToResourceState(heapType, destinationMode);
	if (sourceState == destinationState)
		return;

	auto barrier = resourceTransitionBarrier(resource, sourceState, destinationState);
    commandList->ResourceBarrier(1, &barrier);
}

void ADXCommandList::transitionBufferUsageMode(ID3D12Resource* resource, agpu_memory_heap_type heapType, agpu_buffer_usage_mask sourceMode, agpu_buffer_usage_mask destinationMode)
{
	if (sourceMode == destinationMode)
		return;

	auto sourceState = mapBufferUsageToResourceState(heapType, sourceMode);
	auto destinationState = mapBufferUsageToResourceState(heapType, destinationMode);
	if (sourceState == destinationState)
		return;

	auto barrier = resourceTransitionBarrier(resource, sourceState, destinationState);
	commandList->ResourceBarrier(1, &barrier);
}

agpu_error ADXCommandList::setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
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

agpu_error ADXCommandList::setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    RECT rect;
    rect.top = x;
    rect.left = y;
    rect.right = x + w;
    rect.bottom = y + h;
    commandList->RSSetScissorRects(1, &rect);
    return AGPU_OK;
}

agpu_error ADXCommandList::usePipelineState(const agpu::pipeline_state_ref &pipeline)
{
    CHECK_POINTER(pipeline);
    auto adxPipeline = pipeline.as<ADXPipelineState> ();
    commandList->SetPipelineState(adxPipeline->state.Get());
	adxPipeline->activatedOnCommandList(commandList);

    return AGPU_OK;
}

agpu_error ADXCommandList::useVertexBinding(const agpu::vertex_binding_ref &vertex_binding)
{
    CHECK_POINTER(vertex_binding);
    auto adxVertexBinding = vertex_binding.as<ADXVertexBinding> ();
    commandList->IASetVertexBuffers(0, (UINT)adxVertexBinding->vertexBuffers.size(), &adxVertexBinding->vertexBufferViews[0]);
    return AGPU_OK;
}

agpu_error ADXCommandList::useIndexBuffer(const agpu::buffer_ref &index_buffer)
{
    CHECK_POINTER(index_buffer);
    auto adxIndexBuffer = index_buffer.as<ADXBuffer> ();
    return useIndexBufferAt(index_buffer, 0, adxIndexBuffer->description.stride);
}

agpu_error ADXCommandList::useIndexBufferAt(const agpu::buffer_ref &index_buffer, agpu_size offset, agpu_size index_size)
{
    CHECK_POINTER(index_buffer);
    auto adxIndexBuffer = index_buffer.as<ADXBuffer> ();
    if ((adxIndexBuffer->description.usage_modes & AGPU_ELEMENT_ARRAY_BUFFER) == 0)
        return AGPU_INVALID_PARAMETER;

    D3D12_INDEX_BUFFER_VIEW view = {};
    auto error = adxIndexBuffer->createIndexBufferView(&view, offset, index_size);
    if(error) return error;

    commandList->IASetIndexBuffer(&view);
    return AGPU_OK;
}

agpu_error ADXCommandList::useDrawIndirectBuffer(const agpu::buffer_ref &draw_buffer)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error ADXCommandList::useComputeDispatchIndirectBuffer(const agpu::buffer_ref & buffer)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error ADXCommandList::useShaderResources(const agpu::shader_resource_binding_ref &binding)
{
    CHECK_POINTER(binding);

    auto adxBinding = binding.as<ADXShaderResourceBinding> ();
	commandList->SetGraphicsRootDescriptorTable(adxBinding->bankIndex, adxBinding->gpuDescriptorTableHandle);
    return AGPU_OK;
}

agpu_error ADXCommandList::useComputeShaderResources(const agpu::shader_resource_binding_ref &binding)
{
	CHECK_POINTER(binding);

	auto adxBinding = binding.as<ADXShaderResourceBinding>();
	commandList->SetComputeRootDescriptorTable(adxBinding->bankIndex, adxBinding->gpuDescriptorTableHandle);
	return AGPU_OK;
}

agpu_error ADXCommandList::drawArrays(agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance)
{
    commandList->DrawInstanced(vertex_count, instance_count, first_vertex, base_instance);
    return AGPU_OK;
}

agpu_error ADXCommandList::drawArraysIndirect(agpu_size offset, agpu_size drawcount)
{
    return AGPU_OK;
}

agpu_error ADXCommandList::drawElements(agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance)
{
    commandList->DrawIndexedInstanced(index_count, instance_count, first_index, base_vertex, base_instance);
    return AGPU_OK;
}

agpu_error ADXCommandList::drawElementsIndirect(agpu_size offset, agpu_size drawcount)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error ADXCommandList::dispatchCompute(agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z)
{
	commandList->Dispatch(group_count_x, group_count_y, group_count_z);
    return AGPU_OK;
}

agpu_error ADXCommandList::dispatchComputeIndirect(agpu_size offset)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error ADXCommandList::setStencilReference(agpu_uint reference)
{
    commandList->OMSetStencilRef(reference);
    return AGPU_OK;
}

agpu_error ADXCommandList::executeBundle(const agpu::command_list_ref &bundle)
{
    CHECK_POINTER(bundle);
    auto adxBundle = bundle.as<ADXCommandList> ();
    if (adxBundle->type != AGPU_COMMAND_LIST_TYPE_BUNDLE)
        return AGPU_INVALID_PARAMETER;

    commandList->ExecuteBundle(adxBundle->commandList.Get());
    return AGPU_OK;
}

agpu_error ADXCommandList::close()
{
	while (!bufferTransitionStack.empty())
		popBufferTransitionBarrier();

    ERROR_IF_FAILED(commandList->Close());
    return AGPU_OK;
}

agpu_error ADXCommandList::reset(const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initial_pipeline_state)
{
    CHECK_POINTER(allocator);

    ID3D12PipelineState *state = nullptr;
    if (initial_pipeline_state)
        state = initial_pipeline_state.as<ADXPipelineState> ()->state.Get();

    ERROR_IF_FAILED(commandList->Reset(allocator.as<ADXCommandAllocator> ()->allocator.Get(), state));

    if (initial_pipeline_state)
		initial_pipeline_state.as<ADXPipelineState> ()->activatedOnCommandList(commandList);

    return setCommonState();
}

agpu_error ADXCommandList::resetBundle(const agpu::command_allocator_ref & allocator, const agpu::pipeline_state_ref & initial_pipeline_state, agpu_inheritance_info* inheritance_info)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error ADXCommandList::beginRenderPass(const agpu::renderpass_ref &renderpass, const agpu::framebuffer_ref &framebuffer, agpu_bool secondaryContent)
{
    CHECK_POINTER(renderpass);
    CHECK_POINTER(framebuffer);
    currentFramebuffer = framebuffer;
    if (!currentFramebuffer)
        return AGPU_OK;

    auto adxFramebuffer = framebuffer.as<ADXFramebuffer> ();
    auto adxRenderpass = renderpass.as<ADXRenderPass> ();

    // Perform the resource transitions
    for (size_t i = 0; i < adxFramebuffer->getColorBufferCount(); ++i)
    {
        auto &colorBuffer = adxFramebuffer->colorBuffers[i];
        if (!colorBuffer)
            return AGPU_ERROR;

        auto adxColorBuffer = colorBuffer.as<ADXTexture> ();
        transitionTextureUsageMode(adxColorBuffer->resource.Get(), adxColorBuffer->description.heap_type, getCurrentTextureUsageMode(colorBuffer), AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT);
    }

    if (adxFramebuffer->depthStencilView)
    {
		auto &depthStencilBuffer = adxFramebuffer->depthStencilBuffer;
        auto adxDepthStencil = depthStencilBuffer.as<ADXTexture> ();
        auto depthStencilUsage = agpu_texture_usage_mode_mask(adxDepthStencil->description.usage_modes & (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT));
        transitionTextureUsageMode(adxDepthStencil->resource.Get(), adxDepthStencil->description.heap_type, getCurrentTextureUsageMode(depthStencilBuffer), depthStencilUsage);

        auto desc = adxFramebuffer->getDepthStencilCpuHandle();
        commandList->OMSetRenderTargets((UINT)adxFramebuffer->colorBufferDescriptors.size(), &adxFramebuffer->colorBufferDescriptors[0], FALSE, &desc);
    }
    else
    {
        commandList->OMSetRenderTargets((UINT)adxFramebuffer->colorBufferDescriptors.size(), &adxFramebuffer->colorBufferDescriptors[0], FALSE, nullptr);
    }

    // Clear the color buffers
    for (size_t i = 0; i < adxFramebuffer->getColorBufferCount(); ++i)
    {
        if (!adxFramebuffer->colorBuffers[i])
            return AGPU_ERROR;

        auto handle = adxFramebuffer->getColorBufferCpuHandle(i);
        auto &attachment = adxRenderpass->colorAttachments[i];
        if(attachment.begin_action == AGPU_ATTACHMENT_CLEAR)
            commandList->ClearRenderTargetView(handle, reinterpret_cast<FLOAT*> (&attachment.clear_value), 0, nullptr);
    }

    if ((adxRenderpass->hasDepth || adxRenderpass->hasStencil) && adxRenderpass->depthStencilAttachment.begin_action == AGPU_ATTACHMENT_CLEAR)
    {
        D3D12_CLEAR_FLAGS flags = D3D12_CLEAR_FLAGS(0);
        if (adxRenderpass->hasDepth)
            flags |= D3D12_CLEAR_FLAG_DEPTH;
        if (adxRenderpass->hasStencil)
            flags |= D3D12_CLEAR_FLAG_STENCIL;

        auto clearDepth = adxRenderpass->depthStencilAttachment.clear_value.depth;
        auto clearStencil = adxRenderpass->depthStencilAttachment.clear_value.stencil;
        commandList->ClearDepthStencilView(adxFramebuffer->getDepthStencilCpuHandle(), flags, clearDepth, clearStencil, 0, nullptr);
    }

    return AGPU_OK;
}

agpu_error ADXCommandList::endRenderPass()
{
    if (!currentFramebuffer)
        return AGPU_OK;

    auto adxFramebuffer = currentFramebuffer.as<ADXFramebuffer> ();
    commandList->OMSetRenderTargets(0, nullptr, FALSE, nullptr);

    // Perform the resource transitions
    if (adxFramebuffer->depthStencilView)
    {
		auto& depthStencilBuffer = adxFramebuffer->depthStencilBuffer;
        auto adxDepthStencil = depthStencilBuffer.as<ADXTexture> ();
        auto depthStencilUsage = agpu_texture_usage_mode_mask(adxDepthStencil->description.usage_modes & (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT));
        transitionTextureUsageMode(adxDepthStencil->resource.Get(), adxDepthStencil->description.heap_type, depthStencilUsage, getCurrentTextureUsageMode(depthStencilBuffer));
    }

    for (size_t i = 0; i < adxFramebuffer->getColorBufferCount(); ++i)
    {
        auto &colorBuffer = adxFramebuffer->colorBuffers[i];
        if (!colorBuffer)
            return AGPU_ERROR;

        auto adxColorBuffer = colorBuffer.as<ADXTexture> ();
        transitionTextureUsageMode(adxColorBuffer->resource.Get(), adxColorBuffer->description.heap_type, AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT, getCurrentTextureUsageMode(colorBuffer));
    }

    currentFramebuffer.reset();
    return AGPU_OK;
}

agpu_error ADXCommandList::resolveFramebuffer(const agpu::framebuffer_ref &destFramebuffer, const agpu::framebuffer_ref &sourceFramebuffer)
{
    CHECK_POINTER(destFramebuffer);
    CHECK_POINTER(sourceFramebuffer);

    auto adxDestFramebuffer = destFramebuffer.as<ADXFramebuffer> ();
    auto adxSourceFramebuffer = sourceFramebuffer.as<ADXFramebuffer> ();
    if (destFramebuffer == sourceFramebuffer ||
        adxDestFramebuffer->getColorBufferCount() != adxSourceFramebuffer->getColorBufferCount())
        return AGPU_INVALID_PARAMETER;

    for (size_t i = 0; i < adxDestFramebuffer->getColorBufferCount(); ++i)
    {
        auto &sourceTexture = adxSourceFramebuffer->colorBuffers[i];
        auto &destTexture = adxSourceFramebuffer->colorBuffers[i];
        resolveTexture(sourceTexture, 0, 0, destTexture, 0, 0, 1, 1, AGPU_TEXTURE_ASPECT_COLOR);
    }

    return AGPU_OK;
}

agpu_error ADXCommandList::resolveTexture(const agpu::texture_ref & sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, const agpu::texture_ref & destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect)
{
    auto adxDestTexture = destTexture.as<ADXTexture> ();
    auto adxSourceTexture = sourceTexture.as<ADXTexture> ();

    UINT sourceSubresource = adxSourceTexture->subresourceIndexFor(sourceLevel, sourceLayer);
    UINT destSubresource = adxDestTexture->subresourceIndexFor(destLevel, destLayer);

    D3D12_RESOURCE_STATES destState = mapTextureUsageToResourceState(adxDestTexture->description.heap_type, getCurrentTextureUsageMode(destTexture));
    D3D12_RESOURCE_STATES sourceState = mapTextureUsageToResourceState(adxSourceTexture->description.heap_type, getCurrentTextureUsageMode(sourceTexture));

    {
        D3D12_RESOURCE_BARRIER barriers[2] = {
            resourceTransitionBarrier(adxDestTexture->resource.Get(), destState, D3D12_RESOURCE_STATE_RESOLVE_DEST, destSubresource),
            resourceTransitionBarrier(adxSourceTexture->resource.Get(), sourceState, D3D12_RESOURCE_STATE_RESOLVE_SOURCE, sourceSubresource)
        };
        commandList->ResourceBarrier(2, barriers);
    }

    commandList->ResolveSubresource(adxDestTexture->resource.Get(), destSubresource,
        adxSourceTexture->resource.Get(), sourceSubresource,
        (DXGI_FORMAT)adxSourceTexture->description.format);

    {
        D3D12_RESOURCE_BARRIER barriers[2] = {
            resourceTransitionBarrier(adxDestTexture->resource.Get(), D3D12_RESOURCE_STATE_RESOLVE_DEST, destState, destSubresource),
            resourceTransitionBarrier(adxSourceTexture->resource.Get(), D3D12_RESOURCE_STATE_RESOLVE_SOURCE, sourceState, sourceSubresource)
        };
        commandList->ResourceBarrier(2, barriers);
    }

    return AGPU_OK;
}

agpu_error ADXCommandList::pushConstants(agpu_uint offset, agpu_uint size, agpu_pointer values)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error ADXCommandList::memoryBarrier(agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses)
{
	// TODO: Check the flags for selecting the correct memory barrier type. For now, just create an UAV
	// barrier for everything, which also matches the typical use case for this kind of barrier.
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	commandList->ResourceBarrier(1, &barrier);
	return AGPU_OK;
}

agpu_error ADXCommandList::bufferMemoryBarrier(const agpu::buffer_ref & buffer, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_size offset, agpu_size size)
{
	CHECK_POINTER(buffer);
	auto adxBuffer = buffer.as<ADXBuffer>();
	if ((adxBuffer->description.usage_modes & (AGPU_STORAGE_BUFFER | AGPU_STORAGE_TEXEL_BUFFER)) == 0)
		return AGPU_OK;

	// Create a barrier for UAV.
	// TODO: Check the flags for deciding the correct barrier to use.
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = adxBuffer->resource.Get();
	commandList->ResourceBarrier(1, &barrier);
	return AGPU_OK;
}

agpu_error ADXCommandList::textureMemoryBarrier(const agpu::texture_ref & texture, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_subresource_range* subresource_range)
{
	CHECK_POINTER(texture);
	auto adxTexture= texture.as<ADXTexture>();
	if ((adxTexture->description.usage_modes & AGPU_TEXTURE_USAGE_STORAGE) == 0)
		return AGPU_OK;

	// Create a barrier for UAV.
	// TODO: Check the flags for deciding the correct barrier to use.
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = adxTexture->resource.Get();
	commandList->ResourceBarrier(1, &barrier);
	return AGPU_OK;
}

agpu_error ADXCommandList::pushBufferTransitionBarrier(const agpu::buffer_ref & buffer, agpu_buffer_usage_mask new_usage)
{
	CHECK_POINTER(buffer);

	auto currentBufferUsage = getCurrentBufferUsageMode(buffer);
	auto adxBuffer = buffer.as<ADXBuffer>();
	transitionBufferUsageMode(adxBuffer->resource.Get(), adxBuffer->description.heap_type, currentBufferUsage, new_usage);
	bufferTransitionStack.push_back(std::make_pair(buffer, new_usage));
	return AGPU_UNIMPLEMENTED;
}

agpu_error ADXCommandList::pushTextureTransitionBarrier(const agpu::texture_ref & texture, agpu_texture_usage_mode_mask new_usage, agpu_subresource_range* subresource_range)
{
	CHECK_POINTER(texture);
	return AGPU_UNIMPLEMENTED;
}

agpu_error ADXCommandList::popBufferTransitionBarrier()
{
	if (bufferTransitionStack.empty())
		return AGPU_OUT_OF_BOUNDS;

	auto buffer = bufferTransitionStack.back().first;
	auto adxBuffer = buffer.as<ADXBuffer>();
	auto oldMode = bufferTransitionStack.back().second;
	bufferTransitionStack.pop_back();
	auto newMode = getCurrentBufferUsageMode(buffer);
	transitionBufferUsageMode(adxBuffer->resource.Get(), adxBuffer->description.heap_type, oldMode, newMode);
	return AGPU_OK;
}

agpu_error ADXCommandList::popTextureTransitionBarrier()
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error ADXCommandList::copyBuffer(const agpu::buffer_ref & source_buffer, agpu_size source_offset, const agpu::buffer_ref & dest_buffer, agpu_size dest_offset, agpu_size copy_size)
{
	CHECK_POINTER(source_buffer);
	CHECK_POINTER(dest_buffer);

	auto adxSourceBuffer = source_buffer.as<ADXBuffer>();
	auto adxDestBuffer = dest_buffer.as<ADXBuffer>();
	commandList->CopyBufferRegion(adxDestBuffer->resource.Get(), dest_offset, adxSourceBuffer->resource.Get(), source_offset, copy_size);
	return AGPU_OK;
}

agpu_error ADXCommandList::copyBufferToTexture(const agpu::buffer_ref & buffer, const agpu::texture_ref & texture, agpu_buffer_image_copy_region* copy_region)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error ADXCommandList::copyTextureToBuffer(const agpu::texture_ref & texture, const agpu::buffer_ref & buffer, agpu_buffer_image_copy_region* copy_region)
{
    return AGPU_UNIMPLEMENTED;
}

} // End of namespace AgpuD3D12
