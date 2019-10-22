#include "state_tracker.hpp"

namespace AgpuCommon
{

AbstractStateTracker::AbstractStateTracker(const agpu::state_tracker_cache_ref &cache,
        const agpu::device_ref &device,
        agpu_command_list_type type,
        const agpu::command_queue_ref &commandQueue)
    : device(device), cache(cache), commandListType(type), commandQueue(commandQueue)
{
    isRecording = false;
	isGraphicsPipelineDescriptionChanged = true;
	isComputePipelineDescriptionChanged = true;
}

AbstractStateTracker::~AbstractStateTracker()
{
}

agpu_error AbstractStateTracker::beginRecordingCommands()
{
    auto error = setupCommandListForRecordingCommands();
    if(error) return error;

    return reset();
}

agpu_error AbstractStateTracker::endRecordingAndFlushCommands()
{
    auto commandList = agpu::command_list_ref(endRecordingCommands());
    if(!commandList) return AGPU_ERROR;

    return commandQueue->addCommandList(commandList);
}

agpu_error AbstractStateTracker::reset()
{
    pipelineBuildErrorLog.clear();

    auto error = resetGraphicsPipeline();
    if(error) return error;

    return resetComputePipeline();
}

// Compute pipeline methods.
agpu_error AbstractStateTracker::resetComputePipeline()
{
    computePipelineStateDescription.reset();
    invalidateComputePipelineState();
    return AGPU_OK;
}

void AbstractStateTracker::invalidateComputePipelineState()
{
    isComputePipelineDescriptionChanged = true;
}

agpu_error AbstractStateTracker::validateComputePipelineState()
{
    if(!isComputePipelineDescriptionChanged) return AGPU_OK;

    auto pipelineState = cache.as<StateTrackerCache> ()->getComputePipelineWithDescription(computePipelineStateDescription, pipelineBuildErrorLog);
    if(!pipelineState)
        return AGPU_LINKING_ERROR;

    currentCommandList->usePipelineState(pipelineState);
    isComputePipelineDescriptionChanged = false;
    invalidateGraphicsPipelineState();
    return AGPU_OK;
}

// Graphics pipeline methods.
agpu_error AbstractStateTracker::resetGraphicsPipeline()
{
    graphicsPipelineStateDescription.reset();
    invalidateGraphicsPipelineState();
    return AGPU_OK;
}

void AbstractStateTracker::invalidateGraphicsPipelineState()
{
    isGraphicsPipelineDescriptionChanged = true;
}

agpu_error AbstractStateTracker::validateGraphicsPipelineState()
{
    if(!isGraphicsPipelineDescriptionChanged) return AGPU_OK;

    auto pipelineState = cache.as<StateTrackerCache> ()->getGraphicsPipelineWithDescription(graphicsPipelineStateDescription, pipelineBuildErrorLog);
    if(!pipelineState)
        return AGPU_LINKING_ERROR;

    currentCommandList->usePipelineState(pipelineState);
    isGraphicsPipelineDescriptionChanged = false;
    invalidateComputePipelineState();
    return AGPU_OK;
}

agpu_error AbstractStateTracker::setComputeStage(const agpu::shader_ref & shader, agpu_cstring entryPoint)
{
    if(computePipelineStateDescription.computeStage.set(shader, entryPoint))
        invalidateComputePipelineState();
    return AGPU_OK;
}

agpu_error AbstractStateTracker::setVertexStage(const agpu::shader_ref & shader, agpu_cstring entryPoint)
{
    if(graphicsPipelineStateDescription.vertexStage.set(shader, entryPoint))
        invalidateGraphicsPipelineState();
    return AGPU_OK;
}

agpu_error AbstractStateTracker::setFragmentStage(const agpu::shader_ref & shader, agpu_cstring entryPoint)
{
    if(graphicsPipelineStateDescription.fragmentStage.set(shader, entryPoint))
        invalidateGraphicsPipelineState();
    return AGPU_OK;
}

agpu_error AbstractStateTracker::setGeometryStage(const agpu::shader_ref & shader, agpu_cstring entryPoint)
{
    if(graphicsPipelineStateDescription.geometryStage.set(shader, entryPoint))
        invalidateGraphicsPipelineState();
    return AGPU_OK;
}

agpu_error AbstractStateTracker::setTessellationControlStage(const agpu::shader_ref & shader, agpu_cstring entryPoint)
{
    if(graphicsPipelineStateDescription.tessellationControlStage.set(shader, entryPoint))
        invalidateGraphicsPipelineState();
    return AGPU_OK;
}

agpu_error AbstractStateTracker::setTessellationEvaluationStage(const agpu::shader_ref & shader, agpu_cstring entryPoint)
{
    if(graphicsPipelineStateDescription.tessellationEvaluationStage.set(shader, entryPoint))
        invalidateGraphicsPipelineState();
    return AGPU_OK;
}

agpu_error AbstractStateTracker::setBlendState(agpu_int renderTargetMask, agpu_bool enabled)
{
    bool changed = false;
    graphicsPipelineStateDescription.renderTargetsMatchingMaskDo(renderTargetMask, [&] (RenderTargetColorAttachmentDescription &attachment){
        changed = changed || attachment.blendingEnabled != enabled;
        attachment.blendingEnabled = enabled;
    });

    if(changed) invalidateGraphicsPipelineState();
    return AGPU_OK;
}

agpu_error AbstractStateTracker::setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation)
{
    bool changed = false;
    graphicsPipelineStateDescription.renderTargetsMatchingMaskDo(renderTargetMask, [&] (RenderTargetColorAttachmentDescription &attachment){
        changed = changed ||
            (attachment.sourceColorBlendingFactor != sourceFactor) ||
            (attachment.destColorBlendingFactor != destFactor) ||
            (attachment.colorBlendingOperation != colorOperation) ||

            (attachment.sourceAlphaBlendingFactor != sourceAlphaFactor) ||
            (attachment.destAlphaBlendingFactor != destAlphaFactor) ||
            (attachment.alphaBlendingOperation != alphaOperation);

        attachment.sourceColorBlendingFactor = sourceFactor;
        attachment.destColorBlendingFactor = destFactor;
        attachment.colorBlendingOperation = colorOperation;
        attachment.sourceAlphaBlendingFactor = sourceAlphaFactor;
        attachment.destAlphaBlendingFactor = destAlphaFactor;
        attachment.alphaBlendingOperation = alphaOperation;
    });

    if(changed) invalidateGraphicsPipelineState();
    return AGPU_OK;
}

agpu_error AbstractStateTracker::setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled)
{
    bool changed = false;
    graphicsPipelineStateDescription.renderTargetsMatchingMaskDo(renderTargetMask, [&] (RenderTargetColorAttachmentDescription &attachment){
        changed = changed ||
            (attachment.redColorMask != redEnabled) ||
            (attachment.greenColorMask != greenEnabled) ||
            (attachment.blueColorMask != blueEnabled) ||
            (attachment.alphaColorMask != alphaEnabled);

        attachment.redColorMask = redEnabled;
        attachment.greenColorMask = greenEnabled;
        attachment.blueColorMask = blueEnabled;
        attachment.alphaColorMask = alphaEnabled;
    });

    if(changed) invalidateGraphicsPipelineState();
    return AGPU_OK;
}

agpu_error AbstractStateTracker::setFrontFace(agpu_face_winding winding)
{
    if(graphicsPipelineStateDescription.frontFaceWinding != winding)
    {
        graphicsPipelineStateDescription.frontFaceWinding = winding;
        invalidateGraphicsPipelineState();
    }

    return AGPU_OK;
}

agpu_error AbstractStateTracker::setCullMode(agpu_cull_mode mode)
{
    if(graphicsPipelineStateDescription.faceCullingMode != mode)
    {
        graphicsPipelineStateDescription.faceCullingMode = mode;
        invalidateGraphicsPipelineState();
    }

    return AGPU_OK;
}

agpu_error AbstractStateTracker::setDepthBias(agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor)
{
    auto enabled = constant_factor != 0.0f || clamp != 0.0f || slope_factor != 0.0f;
    if(graphicsPipelineStateDescription.depthBiasEnabled != enabled ||
       graphicsPipelineStateDescription.depthBiasConstantFactor != constant_factor ||
       graphicsPipelineStateDescription.depthBiasClamp != clamp ||
       graphicsPipelineStateDescription.depthBiasSlopeFactor != slope_factor)
    {
        graphicsPipelineStateDescription.depthBiasEnabled = enabled;
        graphicsPipelineStateDescription.depthBiasConstantFactor = constant_factor;
        graphicsPipelineStateDescription.depthBiasClamp = clamp;
        graphicsPipelineStateDescription.depthBiasSlopeFactor = slope_factor;
        invalidateGraphicsPipelineState();
    }

    return AGPU_OK;
}

agpu_error AbstractStateTracker::setDepthState(agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function)
{
    if(graphicsPipelineStateDescription.depthTestingEnabled != enabled ||
       graphicsPipelineStateDescription.depthWriteMask != writeMask ||
       graphicsPipelineStateDescription.depthCompareFunction != function)
    {
        graphicsPipelineStateDescription.depthTestingEnabled = enabled;
        graphicsPipelineStateDescription.depthWriteMask = writeMask;
        graphicsPipelineStateDescription.depthCompareFunction = function;
        invalidateGraphicsPipelineState();
    }
    return AGPU_OK;
}

agpu_error AbstractStateTracker::setPolygonMode(agpu_polygon_mode mode)
{
    if(graphicsPipelineStateDescription.polygonMode != mode)
    {
        graphicsPipelineStateDescription.polygonMode = mode;
        invalidateGraphicsPipelineState();
    }
    return AGPU_OK;
}

agpu_error AbstractStateTracker::setStencilState(agpu_bool enabled, agpu_int writeMask, agpu_int readMask)
{
    if(graphicsPipelineStateDescription.stencilTestingEnabled != enabled ||
       graphicsPipelineStateDescription.stencilWriteMask != writeMask ||
       graphicsPipelineStateDescription.stencilReadMask != readMask)
    {
        graphicsPipelineStateDescription.stencilTestingEnabled = enabled;
        graphicsPipelineStateDescription.stencilWriteMask = writeMask;
        graphicsPipelineStateDescription.stencilReadMask = readMask;
        invalidateGraphicsPipelineState();
    }
    return AGPU_OK;
}

agpu_error AbstractStateTracker::setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    if(graphicsPipelineStateDescription.frontStencilFailOperation != stencilFailOperation ||
       graphicsPipelineStateDescription.frontStencilDepthFailOperation != depthFailOperation ||
       graphicsPipelineStateDescription.frontStencilDepthPassOperation != stencilDepthPassOperation ||
       graphicsPipelineStateDescription.frontStencilCompareFunction != stencilFunction)
    {
        graphicsPipelineStateDescription.frontStencilFailOperation = stencilFailOperation;
        graphicsPipelineStateDescription.frontStencilDepthFailOperation = depthFailOperation;
        graphicsPipelineStateDescription.frontStencilDepthPassOperation = stencilDepthPassOperation;
        graphicsPipelineStateDescription.frontStencilCompareFunction = stencilFunction;
        invalidateGraphicsPipelineState();
    }
    return AGPU_OK;
}

agpu_error AbstractStateTracker::setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    if(graphicsPipelineStateDescription.backStencilFailOperation != stencilFailOperation ||
       graphicsPipelineStateDescription.backStencilDepthFailOperation != depthFailOperation ||
       graphicsPipelineStateDescription.backStencilDepthPassOperation != stencilDepthPassOperation ||
       graphicsPipelineStateDescription.backStencilCompareFunction != stencilFunction)
    {
        graphicsPipelineStateDescription.backStencilFailOperation = stencilFailOperation;
        graphicsPipelineStateDescription.backStencilDepthFailOperation = depthFailOperation;
        graphicsPipelineStateDescription.backStencilDepthPassOperation = stencilDepthPassOperation;
        graphicsPipelineStateDescription.backStencilCompareFunction = stencilFunction;
        invalidateGraphicsPipelineState();
    }
    return AGPU_OK;
}

agpu_error AbstractStateTracker::setPrimitiveType(agpu_primitive_topology type)
{
    if(graphicsPipelineStateDescription.primitiveType != type)
    {
        graphicsPipelineStateDescription.primitiveType = type;
        invalidateGraphicsPipelineState();
    }
    return AGPU_OK;
}

agpu_error AbstractStateTracker::setVertexLayout(const agpu::vertex_layout_ref & layout)
{
    if(graphicsPipelineStateDescription.vertexLayout != layout)
    {
        graphicsPipelineStateDescription.vertexLayout = layout;
        invalidateGraphicsPipelineState();
    }

    return AGPU_OK;
}

agpu_error AbstractStateTracker::setShaderSignature(const agpu::shader_signature_ref & signature)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;

    graphicsPipelineStateDescription.shaderSignature = signature;
    computePipelineStateDescription.shaderSignature = signature;
    invalidateComputePipelineState();
    invalidateGraphicsPipelineState();
    return currentCommandList->setShaderSignature(signature);
}

agpu_error AbstractStateTracker::setSampleDescription(agpu_uint sample_count, agpu_uint sample_quality)
{
    if(graphicsPipelineStateDescription.sampleCount != sample_count ||
       graphicsPipelineStateDescription.sampleQuality != sample_quality)
    {
        graphicsPipelineStateDescription.sampleCount = sample_count;
        graphicsPipelineStateDescription.sampleQuality = sample_quality;
        invalidateGraphicsPipelineState();
    }
    return AGPU_OK;
}

// Command list methods.
agpu_error AbstractStateTracker::setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->setViewport(x, y, w, h);
}

agpu_error AbstractStateTracker::setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->setScissor(x, y, w, h);
}

agpu_error AbstractStateTracker::useVertexBinding(const agpu::vertex_binding_ref & vertex_binding)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->useVertexBinding(vertex_binding);
}

agpu_error AbstractStateTracker::useIndexBuffer(const agpu::buffer_ref & index_buffer)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->useIndexBuffer(index_buffer);
}

agpu_error AbstractStateTracker::useIndexBufferAt(const agpu::buffer_ref & index_buffer, agpu_size offset, agpu_size index_size)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->useIndexBufferAt(index_buffer, offset, index_size);
}

agpu_error AbstractStateTracker::useDrawIndirectBuffer(const agpu::buffer_ref & draw_buffer)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->useDrawIndirectBuffer(draw_buffer);
}

agpu_error AbstractStateTracker::useComputeDispatchIndirectBuffer(const agpu::buffer_ref & buffer)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->useComputeDispatchIndirectBuffer(buffer);
}

agpu_error AbstractStateTracker::useShaderResources(const agpu::shader_resource_binding_ref & binding)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->useShaderResources(binding);
}

agpu_error AbstractStateTracker::useComputeShaderResources(const agpu::shader_resource_binding_ref & binding)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->useComputeShaderResources(binding);
}

agpu_error AbstractStateTracker::drawArrays(agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;

    auto error = validateGraphicsPipelineState();
    if(error) return error;

    return currentCommandList->drawArrays(vertex_count, instance_count, first_vertex, base_instance);
}

agpu_error AbstractStateTracker::drawArraysIndirect(agpu_size offset, agpu_size drawcount)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;

    auto error = validateGraphicsPipelineState();
    if(error) return error;

    return currentCommandList->drawArraysIndirect(offset, drawcount);
}

agpu_error AbstractStateTracker::drawElements(agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;

    auto error = validateGraphicsPipelineState();
    if(error) return error;

    return currentCommandList->drawElements(index_count, instance_count, first_index, base_vertex, base_instance);
}

agpu_error AbstractStateTracker::drawElementsIndirect(agpu_size offset, agpu_size drawcount)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;

    auto error = validateGraphicsPipelineState();
    if(error) return error;

    return currentCommandList->drawElementsIndirect(offset, drawcount);
}

agpu_error AbstractStateTracker::dispatchCompute(agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;

    auto error = validateComputePipelineState();
    if(error) return error;

    return currentCommandList->dispatchCompute(group_count_x, group_count_y, group_count_z);
}

agpu_error AbstractStateTracker::dispatchComputeIndirect(agpu_size offset)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;

    auto error = validateComputePipelineState();
    if(error) return error;

    return currentCommandList->dispatchComputeIndirect(offset);
}

agpu_error AbstractStateTracker::setStencilReference(agpu_uint reference)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->setStencilReference(reference);
}

agpu_error AbstractStateTracker::executeBundle(const agpu::command_list_ref & bundle)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->executeBundle(bundle);
}

agpu_error AbstractStateTracker::beginRenderPass(const agpu::renderpass_ref & renderpass, const agpu::framebuffer_ref & framebuffer, agpu_bool bundle_content)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    if(!renderpass) return AGPU_NULL_POINTER;
    if(!framebuffer) return AGPU_NULL_POINTER;

    // Extract render target count, format
    constexpr auto MaxRenderTargetAttachmentCount = GraphicsPipelineStateDescription::MaxRenderTargetAttachmentCount ;
    std::array<agpu_texture_format, MaxRenderTargetAttachmentCount> textureFormats;
    agpu_uint renderTargetCount = MaxRenderTargetAttachmentCount;
    auto error = renderpass->getColorAttachmentFormats(&renderTargetCount, &textureFormats[0]);
    if(error) return error;

    // Extract depth stencil format.
    auto depthStencilFormat = renderpass->getDepthStencilAttachmentFormat();
    bool changed = false;
    if(graphicsPipelineStateDescription.depthStencilFormat != depthStencilFormat)
    {
        graphicsPipelineStateDescription.depthStencilFormat = depthStencilFormat;
        changed = true;
    }

    // Apply the render target count and formats.
    if(graphicsPipelineStateDescription.renderTargetColorAttachmentCount != renderTargetCount)
    {
        changed = true;
        graphicsPipelineStateDescription.renderTargetColorAttachmentCount = renderTargetCount;
    }

    for(agpu_uint i = 0; i < renderTargetCount; ++i)
    {
        auto &attachment = graphicsPipelineStateDescription.renderTargetColorAttachments[i];
        auto attachmentFormat = textureFormats[i];
        if(attachment.textureFormat != attachmentFormat)
        {
            changed = true;
            attachment.textureFormat = attachmentFormat;
        }
    }

    // Apply the sample count and quality.
    auto sampleCount = renderpass->getSampleCount();
    auto sampleQuality = renderpass->getSampleQuality();
    if(sampleCount != graphicsPipelineStateDescription.sampleCount ||
        sampleQuality != graphicsPipelineStateDescription.sampleQuality)
    {
        graphicsPipelineStateDescription.sampleCount = sampleCount;
        graphicsPipelineStateDescription.sampleQuality = sampleQuality;
        changed = true;
    }

    if(changed)
        invalidateGraphicsPipelineState();

    return currentCommandList->beginRenderPass(renderpass, framebuffer, bundle_content);
}

agpu_error AbstractStateTracker::endRenderPass()
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->endRenderPass();
}

agpu_error AbstractStateTracker::resolveFramebuffer(const agpu::framebuffer_ref & destFramebuffer, const agpu::framebuffer_ref & sourceFramebuffer)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->resolveFramebuffer(destFramebuffer, sourceFramebuffer);
}

agpu_error AbstractStateTracker::resolveTexture(const agpu::texture_ref & sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, const agpu::texture_ref & destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->resolveTexture(sourceTexture, sourceLevel, sourceLayer, destTexture, destLevel, destLayer, levelCount, layerCount, aspect);
}

agpu_error AbstractStateTracker::pushConstants(agpu_uint offset, agpu_uint size, agpu_pointer values)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->pushConstants(offset, size, values);
}

agpu_error AbstractStateTracker::memoryBarrier(agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->memoryBarrier(source_stage, dest_stage, source_accesses, dest_accesses);
}

agpu_error AbstractStateTracker::bufferMemoryBarrier(const agpu::buffer_ref & buffer, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_size offset, agpu_size size)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->bufferMemoryBarrier(buffer, source_stage, dest_stage, source_accesses, dest_accesses, offset, size);
}

agpu_error AbstractStateTracker::textureMemoryBarrier(const agpu::texture_ref & texture, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_subresource_range* subresource_range)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->textureMemoryBarrier(texture, source_stage, dest_stage, source_accesses, dest_accesses, subresource_range);
}

agpu_error AbstractStateTracker::pushBufferTransitionBarrier(const agpu::buffer_ref & buffer, agpu_buffer_usage_mask new_usage)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->pushBufferTransitionBarrier(buffer, new_usage);
}

agpu_error AbstractStateTracker::pushTextureTransitionBarrier(const agpu::texture_ref & texture, agpu_texture_usage_mode_mask new_usage, agpu_subresource_range* subresource_range)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->pushTextureTransitionBarrier(texture, new_usage, subresource_range);
}

agpu_error AbstractStateTracker::popBufferTransitionBarrier()
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->popBufferTransitionBarrier();
}

agpu_error AbstractStateTracker::popTextureTransitionBarrier()
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->popTextureTransitionBarrier();
}

agpu_error AbstractStateTracker::copyBuffer(const agpu::buffer_ref & source_buffer, agpu_size source_offset, const agpu::buffer_ref & dest_buffer, agpu_size dest_offset, agpu_size copy_size)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->copyBuffer(source_buffer, source_offset, dest_buffer, dest_offset, copy_size);
}

agpu_error AbstractStateTracker::copyBufferToTexture(const agpu::buffer_ref & buffer, const agpu::texture_ref & texture, agpu_buffer_image_copy_region* copy_region)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->copyBufferToTexture(buffer, texture, copy_region);
}

agpu_error AbstractStateTracker::copyTextureToBuffer(const agpu::texture_ref & texture, const agpu::buffer_ref & buffer, agpu_buffer_image_copy_region* copy_region)
{
    if(!currentCommandList) return AGPU_INVALID_OPERATION;
    return currentCommandList->copyTextureToBuffer(texture, buffer, copy_region);
}


//==============================================================================
// DirectStateTracker
//==============================================================================

DirectStateTracker::DirectStateTracker(const agpu::state_tracker_cache_ref &cache,
        const agpu::device_ref &device,
        agpu_command_list_type type,
        const agpu::command_queue_ref &commandQueue,
        const agpu::command_allocator_ref &commandAllocator,
        bool isCommandAllocatorOwned)
    : AbstractStateTracker(cache, device, type, commandQueue),
      commandAllocator(commandAllocator),
      isCommandAllocatorOwned(isCommandAllocatorOwned)
{
}

DirectStateTracker::~DirectStateTracker()
{
}

agpu::state_tracker_ref DirectStateTracker::create(const agpu::state_tracker_cache_ref &cache,
        const agpu::device_ref &device,
        agpu_command_list_type type,
        const agpu::command_queue_ref &commandQueue,
        const agpu::command_allocator_ref &commandAllocator,
        bool isCommandAllocatorOwned)
{
    auto result = agpu::makeObject<DirectStateTracker> (cache, device, type, commandQueue, commandAllocator, isCommandAllocatorOwned);
    if(!result.as<DirectStateTracker> ()->createCommandList())
        return agpu::state_tracker_ref();

    return result;
}

bool DirectStateTracker::createCommandList()
{
    commandList = agpu::command_list_ref(device->createCommandList(commandListType, commandAllocator, agpu::pipeline_state_ref()));
	if (!commandList)
		return false;

	commandList->close();
    return true;
}

agpu_error DirectStateTracker::setupCommandListForRecordingCommands()
{
    // Reset the command allocator, if needed.
    if(isCommandAllocatorOwned)
    {
        auto resetError = commandAllocator->reset();
        if(resetError) return resetError;
    }

    // Reset the command list.
    auto error = commandList->reset(commandAllocator, agpu::pipeline_state_ref());
    if(error) return error;

    currentCommandList = commandList;
    return AGPU_OK;
}

agpu::command_list_ptr DirectStateTracker::endRecordingCommands()
{
    auto error = currentCommandList->close();
    auto result = currentCommandList.disown();
    if(error) return nullptr;
    return result;
}

//==============================================================================
// FrameBufferredStateTracker
//==============================================================================

FrameBufferredStateTracker::FrameBufferredStateTracker(const agpu::state_tracker_cache_ref &cache,
        const agpu::device_ref &device,
        agpu_command_list_type type,
        const agpu::command_queue_ref &commandQueue,
        agpu_uint frameBufferingCount)
    : AbstractStateTracker(cache, device, type, commandQueue),
      frameBufferingCount(frameBufferingCount)
{
}

FrameBufferredStateTracker::~FrameBufferredStateTracker()
{
}

agpu::state_tracker_ref FrameBufferredStateTracker::create(const agpu::state_tracker_cache_ref &cache,
        const agpu::device_ref &device,
        agpu_command_list_type type,
        const agpu::command_queue_ref &commandQueue,
        agpu_uint frameBufferingCount)
{
    auto result = agpu::makeObject<FrameBufferredStateTracker> (cache, device, type, commandQueue, frameBufferingCount);
    if(!result.as<FrameBufferredStateTracker> ()->createCommandAllocatorsAndCommandLists())
        return agpu::state_tracker_ref();
    return result;
}

bool FrameBufferredStateTracker::createCommandAllocatorsAndCommandLists()
{
    commandAllocators.reserve(frameBufferingCount);
    commandLists.reserve(frameBufferingCount);
    for(size_t i = 0; i < frameBufferingCount; ++i)
    {
        auto allocator = agpu::command_allocator_ref(device->createCommandAllocator(commandListType, commandQueue));
        if(!allocator)
            return false;

        auto commandList = agpu::command_list_ref(device->createCommandList(commandListType, allocator, agpu::pipeline_state_ref()));
        if(!commandList)
            return false;

        auto error = commandList->close();
        if(error)
            return false;

        commandAllocators.push_back(allocator);
        commandLists.push_back(commandList);
    }

    return true;
}

agpu_error FrameBufferredStateTracker::setupCommandListForRecordingCommands()
{
    return AGPU_UNIMPLEMENTED;
}

agpu::command_list_ptr FrameBufferredStateTracker::endRecordingCommands()
{
    return nullptr;
}

} // End of namespace AgpuCommon
