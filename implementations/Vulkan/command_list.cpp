#include "command_list.hpp"
#include "command_allocator.hpp"
#include "framebuffer.hpp"
#include "fence.hpp"
#include "renderpass.hpp"
#include "texture.hpp"
#include "texture_view.hpp"
#include "buffer.hpp"
#include "vertex_binding.hpp"
#include "pipeline_state.hpp"
#include "shader_signature.hpp"
#include "shader_resource_binding.hpp"
#include "constants.hpp"

namespace AgpuVulkan
{

inline VkIndexType indexTypeForStride(agpu_size stride)
{
    switch (stride)
    {
    default:
    case 2: return VK_INDEX_TYPE_UINT16;
    case 4: return VK_INDEX_TYPE_UINT32;
    }
}

AVkCommandList::AVkCommandList(const agpu::device_ref &device)
    : device(device)
{
    isClosed = false;
    isSecondaryContent = false;
}

AVkCommandList::~AVkCommandList()
{
    vkFreeCommandBuffers(deviceForVk->device, allocator.as<AVkCommandAllocator> ()->commandPool, 1, &commandBuffer);
}

agpu::command_list_ref AVkCommandList::create(const agpu::device_ref &device, agpu_command_list_type type, const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initial_pipeline_state)
{
    if (!allocator)
        return agpu::command_list_ref();

    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandBufferCount = 1;
    info.commandPool = allocator.as<AVkCommandAllocator> ()->commandPool;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    if(type == AGPU_COMMAND_LIST_TYPE_BUNDLE)
        info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;

    VkCommandBuffer commandBuffer;
    auto error = vkAllocateCommandBuffers(deviceForVk->device, &info, &commandBuffer);
    if (error)
        return agpu::command_list_ref();

    VkCommandBufferInheritanceInfo commandBufferInheritance = {};
    commandBufferInheritance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

    VkCommandBufferBeginInfo bufferBeginInfo = {};
    bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bufferBeginInfo.pInheritanceInfo = &commandBufferInheritance;

    error = vkBeginCommandBuffer(commandBuffer, &bufferBeginInfo);
    if (error)
    {
        vkFreeCommandBuffers(deviceForVk->device, allocator.as<AVkCommandAllocator> ()->commandPool, 1, &commandBuffer);
        return agpu::command_list_ref();
    }

    if (initial_pipeline_state)
        vkCmdBindPipeline(commandBuffer, initial_pipeline_state.as<AVkPipelineState> ()->bindPoint, initial_pipeline_state.as<AVkPipelineState> ()->pipeline);

    auto result = agpu::makeObject<AVkCommandList> (device);
    auto avkCommandList = result.as<AVkCommandList> ();
    avkCommandList->commandBuffer = commandBuffer;
    avkCommandList->allocator = allocator;
    avkCommandList->queueFamilyIndex = allocator.as<AVkCommandAllocator> ()->queueFamilyIndex;
    avkCommandList->resetState();
    return result;
}

agpu_error AVkCommandList::close()
{
    while(!bufferTransitionStack.empty())
        popBufferTransitionBarrier();
    while(!textureTransitionStack.empty())
        popTextureTransitionBarrier();

    auto error = vkEndCommandBuffer(commandBuffer);
    CONVERT_VULKAN_ERROR(error);
    return AGPU_OK;
}

agpu_error AVkCommandList::reset(const agpu::command_allocator_ref &newAllocator, const agpu::pipeline_state_ref &initial_pipeline_state)
{
    CHECK_POINTER(newAllocator);
    if (newAllocator != allocator)
    {
        // TODO: Implement this
        return AGPU_UNIMPLEMENTED;
    }

    auto error = vkResetCommandBuffer(commandBuffer, 0);
    CONVERT_VULKAN_ERROR(error);

    VkCommandBufferInheritanceInfo commandBufferInheritance = {};
    commandBufferInheritance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

    VkCommandBufferBeginInfo bufferBeginInfo = {};
    bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bufferBeginInfo.pInheritanceInfo = &commandBufferInheritance;

    error = vkBeginCommandBuffer(commandBuffer, &bufferBeginInfo);
    CONVERT_VULKAN_ERROR(error);

    if (initial_pipeline_state)
        vkCmdBindPipeline(commandBuffer, initial_pipeline_state.as<AVkPipelineState> ()->bindPoint, initial_pipeline_state.as<AVkPipelineState> ()->pipeline);

    resetState();
    return AGPU_OK;
}

agpu_error AVkCommandList::resetBundle (const agpu::command_allocator_ref &newAllocator, const agpu::pipeline_state_ref &initial_pipeline_state, agpu_inheritance_info* inheritance_info )
{
    CHECK_POINTER(newAllocator);
    if (newAllocator != allocator)
    {
        // TODO: Implement this
        return AGPU_UNIMPLEMENTED;
    }

    auto error = vkResetCommandBuffer(commandBuffer, 0);
    CONVERT_VULKAN_ERROR(error);

    VkCommandBufferInheritanceInfo commandBufferInheritance = {};
    commandBufferInheritance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

    VkCommandBufferBeginInfo bufferBeginInfo = {};
    bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bufferBeginInfo.pInheritanceInfo = &commandBufferInheritance;
    bufferBeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    if(inheritance_info)
    {
        if(inheritance_info->renderpass)
        {
            commandBufferInheritance.renderPass = agpu::renderpass_ref::import(inheritance_info->renderpass).as<AVkRenderPass> ()->handle;
            bufferBeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
        }
    }

    error = vkBeginCommandBuffer(commandBuffer, &bufferBeginInfo);
    CONVERT_VULKAN_ERROR(error);

    if (initial_pipeline_state)
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, initial_pipeline_state.as<AVkPipelineState> ()->pipeline);

    resetState();
    return AGPU_OK;
}

void AVkCommandList::resetState()
{
    currentFramebuffer.reset();
    drawIndirectBuffer.reset();
    computeDispatchIndirectBuffer.reset();
    shaderSignature.reset();
    waitSemaphores.clear();
    signalSemaphores.clear();

    isSecondaryContent = false;

    vkCmdSetStencilReference(commandBuffer, VK_STENCIL_FRONT_AND_BACK, 0);
}

agpu_error AVkCommandList::setShaderSignature(const agpu::shader_signature_ref &signature)
{
    CHECK_POINTER(signature);
    shaderSignature = signature;
    return AGPU_OK;
}

agpu_error AVkCommandList::setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    VkViewport viewport;
    viewport.width = float(w);
    viewport.height = float(h);
    viewport.x = float(x);
    viewport.y = float(y);
    viewport.maxDepth = 1.0f;
    viewport.minDepth = 0.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    return AGPU_OK;

}

agpu_error AVkCommandList::setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    VkRect2D rect;
    rect.extent.width = w;
    rect.extent.height = h;
    rect.offset.x = x;
    rect.offset.y = y;
    vkCmdSetScissor(commandBuffer, 0, 1, &rect);
    return AGPU_OK;
}

agpu_error AVkCommandList::usePipelineState(const agpu::pipeline_state_ref &pipeline)
{
    CHECK_POINTER(pipeline);
    auto state = pipeline.as<AVkPipelineState> ();
    vkCmdBindPipeline(commandBuffer, state->bindPoint, state->pipeline);
    return AGPU_OK;
}

agpu_error AVkCommandList::useVertexBinding(const agpu::vertex_binding_ref &vertex_binding)
{
    CHECK_POINTER(vertex_binding);
    auto bindings = vertex_binding.as<AVkVertexBinding> ();
    vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)bindings->vulkanBuffers.size(), &bindings->vulkanBuffers[0], &bindings->offsets[0]);
    return AGPU_OK;
}

agpu_error AVkCommandList::useIndexBuffer(const agpu::buffer_ref &index_buffer)
{
    CHECK_POINTER(index_buffer);
    return useIndexBufferAt(index_buffer, 0, index_buffer.as<AVkBuffer> ()->description.stride);
}

agpu_error AVkCommandList::useIndexBufferAt(const agpu::buffer_ref &index_buffer, agpu_size offset, agpu_size index_size)
{
    CHECK_POINTER(index_buffer);
    if ((index_buffer.as<AVkBuffer> ()->description.usage_modes & AGPU_ELEMENT_ARRAY_BUFFER) == 0)
        return AGPU_INVALID_PARAMETER;

    vkCmdBindIndexBuffer(commandBuffer, index_buffer.as<AVkBuffer> ()->handle, offset, indexTypeForStride(index_size));
    return AGPU_OK;
}

agpu_error AVkCommandList::useDrawIndirectBuffer(const agpu::buffer_ref &draw_buffer)
{
    CHECK_POINTER(draw_buffer);
    if ((draw_buffer.as<AVkBuffer> ()->description.usage_modes & AGPU_DRAW_INDIRECT_BUFFER) == 0)
        return AGPU_INVALID_PARAMETER;

    drawIndirectBuffer = draw_buffer;
    return AGPU_OK;
}

agpu_error AVkCommandList::useComputeDispatchIndirectBuffer(const agpu::buffer_ref &dispatch_buffer)
{
    CHECK_POINTER(dispatch_buffer);
    if ((dispatch_buffer.as<AVkBuffer> ()->description.usage_modes & AGPU_COMPUTE_DISPATCH_INDIRECT_BUFFER) == 0)
        return AGPU_INVALID_PARAMETER;

    computeDispatchIndirectBuffer = dispatch_buffer;
    return AGPU_OK;
}

agpu_error AVkCommandList::useShaderResources(const agpu::shader_resource_binding_ref &binding)
{
    CHECK_POINTER(binding);
    return useShaderResourcesInSlot(binding, binding.as<AVkShaderResourceBinding> ()->elementIndex);
}


agpu_error AVkCommandList::useShaderResourcesInSlot(const agpu::shader_resource_binding_ref & binding, agpu_uint slot)
{
    if (!shaderSignature)
        return AGPU_INVALID_OPERATION;

    auto avkBindings = binding.as<AVkShaderResourceBinding> ();
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            shaderSignature.as<AVkShaderSignature> ()->layout,
            slot, 1, &avkBindings->descriptorSet, 0, nullptr);
    return AGPU_OK;
}

agpu_error AVkCommandList::useComputeShaderResources(const agpu::shader_resource_binding_ref &binding)
{
    CHECK_POINTER(binding);
    return useComputeShaderResourcesInSlot(binding, binding.as<AVkShaderResourceBinding> ()->elementIndex);
}

agpu_error AVkCommandList::useComputeShaderResourcesInSlot(const agpu::shader_resource_binding_ref & binding, agpu_uint slot)
{
    CHECK_POINTER(binding);
    if (!shaderSignature)
        return AGPU_INVALID_OPERATION;

    auto avkBindings = binding.as<AVkShaderResourceBinding> ();
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
            shaderSignature.as<AVkShaderSignature> ()->layout,
            slot, 1, &avkBindings->descriptorSet, 0, nullptr);
    return AGPU_OK;
}

agpu_error AVkCommandList::pushConstants(agpu_uint offset, agpu_uint size, agpu_pointer values)
{
    CHECK_POINTER(values);
    vkCmdPushConstants(commandBuffer, shaderSignature.as<AVkShaderSignature> ()->layout, VK_SHADER_STAGE_ALL, offset, size, values);
    return AGPU_OK;
}

agpu_error AVkCommandList::drawArrays(agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance)
{
    vkCmdDraw(commandBuffer, vertex_count, instance_count, first_vertex, base_instance);
    return AGPU_OK;
}

agpu_error AVkCommandList::drawArraysIndirect(agpu_size offset, agpu_size drawcount)
{
    if (!drawIndirectBuffer)
        return AGPU_INVALID_OPERATION;

    vkCmdDrawIndirect(commandBuffer, drawIndirectBuffer.as<AVkBuffer> ()->handle, offset, drawcount, drawIndirectBuffer.as<AVkBuffer> ()->description.stride);
    return AGPU_OK;
}

agpu_error AVkCommandList::drawElements(agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance)
{
    vkCmdDrawIndexed(commandBuffer, index_count, instance_count, first_index, base_vertex, base_instance);
    return AGPU_OK;
}

agpu_error AVkCommandList::drawElementsIndirect(agpu_size offset, agpu_size drawcount)
{
    if (!drawIndirectBuffer)
        return AGPU_INVALID_OPERATION;

    vkCmdDrawIndexedIndirect(commandBuffer, drawIndirectBuffer.as<AVkBuffer> ()->handle, offset, drawcount, drawIndirectBuffer.as<AVkBuffer> ()->description.stride);
    return AGPU_OK;
}

agpu_error AVkCommandList::dispatchCompute ( agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z )
{
    vkCmdDispatch(commandBuffer, group_count_x, group_count_y, group_count_z);
    return AGPU_OK;
}

agpu_error AVkCommandList::dispatchComputeIndirect ( agpu_size offset )
{
    if (!computeDispatchIndirectBuffer)
        return AGPU_INVALID_OPERATION;

    vkCmdDispatchIndirect(commandBuffer, computeDispatchIndirectBuffer.as<AVkBuffer> ()->handle, offset);
    return AGPU_OK;
}

agpu_error AVkCommandList::setStencilReference(agpu_uint reference)
{
    vkCmdSetStencilReference(commandBuffer, VK_STENCIL_FRONT_AND_BACK, reference);
    return AGPU_OK;
}

agpu_error AVkCommandList::executeBundle(const agpu::command_list_ref &bundle)
{
    CHECK_POINTER(bundle);
    auto avkBundle = bundle.as<AVkCommandList> ();
    if (!avkBundle->isClosed || !avkBundle->isSecondaryContent)
        return AGPU_INVALID_PARAMETER;

    vkCmdExecuteCommands(commandBuffer, 1, &avkBundle->commandBuffer);
    return AGPU_OK;
}

agpu_error AVkCommandList::transitionImageUsageMode(VkImage image, agpu_texture_usage_mode_mask allowedUsages, agpu_texture_usage_mode_mask sourceUsage, agpu_texture_usage_mode_mask destUsage, VkImageSubresourceRange range)
{
    if(sourceUsage == destUsage)
        return AGPU_OK;

    VkPipelineStageFlags srcStages = 0;
    VkPipelineStageFlags destStages = 0;
    auto barrier = barrierForImageUsageTransition(image, range, allowedUsages, sourceUsage, destUsage, srcStages, destStages);

    vkCmdPipelineBarrier(commandBuffer, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    return AGPU_OK;
}

agpu_error AVkCommandList::transitionBufferUsageMode(VkBuffer buffer, agpu_buffer_usage_mask oldUsageMode, agpu_buffer_usage_mask newUsageMode)
{
    VkBufferMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.srcAccessMask = mapBufferUsageModeToAccessFlags(oldUsageMode);
    barrier.dstAccessMask = mapBufferUsageModeToAccessFlags(newUsageMode);
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.buffer = buffer;
    barrier.offset = 0;
    barrier.size = VK_WHOLE_SIZE;
    vkCmdPipelineBarrier(commandBuffer, mapBufferUsageModeToSourceStages(oldUsageMode), mapBufferUsageModeToDestinationStages(newUsageMode),
            0, 0, nullptr, 1, &barrier, 0, nullptr);

    return AGPU_OK;
}

agpu_error AVkCommandList::beginRenderPass(const agpu::renderpass_ref &renderpass, const agpu::framebuffer_ref &framebuffer, agpu_bool secondaryContent)
{
    CHECK_POINTER(renderpass);
    CHECK_POINTER(framebuffer);

    // Store the framebuffer
    currentFramebuffer = framebuffer;
    isSecondaryContent = secondaryContent;

    auto avkCurrentFramebuffer = currentFramebuffer.as<AVkFramebuffer> ();

    // Transition the color attachments.
    for (agpu_uint i = 0; i < avkCurrentFramebuffer->colorCount; ++i)
    {
        VkImageSubresourceRange range = {};

        auto &desc = avkCurrentFramebuffer->attachmentViews[i].as<AVkTextureView> ()->description;
        range.baseArrayLayer = desc.subresource_range.base_arraylayer;
        range.baseMipLevel = desc.subresource_range.base_miplevel;
        range.layerCount = 1;
        range.levelCount = 1;
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        auto attachmentTexture = avkCurrentFramebuffer->attachmentTextures[i].as<AVkTexture> ();
        transitionImageUsageMode(attachmentTexture->image, attachmentTexture->description.usage_modes, attachmentTexture->description.main_usage_mode, AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT, range);
    }

    // Transition the depth stencil attachment, if needed.
    if(avkCurrentFramebuffer->hasDepthStencil)
    {
        auto depthStencilAttachment = avkCurrentFramebuffer->attachmentTextures.back().as<AVkTexture> ();

        auto &desc = avkCurrentFramebuffer->attachmentViews.back().as<AVkTextureView> ()->description;;
        VkImageSubresourceRange range = {};
        range.baseArrayLayer = desc.subresource_range.base_arraylayer;
        range.baseMipLevel = desc.subresource_range.base_miplevel;
        range.layerCount = 1;
        range.levelCount = 1;
        range.aspectMask = depthStencilAttachment->imageAspect;

        auto depthStencilUsageMode = depthStencilAttachment->description.usage_modes & (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT);
        transitionImageUsageMode(depthStencilAttachment->image, depthStencilAttachment->description.usage_modes, depthStencilAttachment->description.main_usage_mode, agpu_texture_usage_mode_mask(depthStencilUsageMode), range);
    }

    // Begin the render pass.
    auto avkRenderPass = renderpass.as<AVkRenderPass> ();
    VkRenderPassBeginInfo passBeginInfo = {};
    passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    passBeginInfo.renderPass = avkRenderPass->handle;
    passBeginInfo.framebuffer = avkCurrentFramebuffer->framebuffer;
    passBeginInfo.renderArea.extent.width = avkCurrentFramebuffer->width;
    passBeginInfo.renderArea.extent.height = avkCurrentFramebuffer->height;

    // Set the clear values.
    if (!avkRenderPass->clearValues.empty())
    {
        passBeginInfo.clearValueCount = (uint32_t)avkRenderPass->clearValues.size();
        passBeginInfo.pClearValues = &avkRenderPass->clearValues[0];
    }

    // Wait for the semaphore
    if(avkCurrentFramebuffer->waitSemaphore)
        addWaitSemaphore(avkCurrentFramebuffer->waitSemaphore, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

    vkCmdBeginRenderPass(commandBuffer, &passBeginInfo, secondaryContent ? VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS : VK_SUBPASS_CONTENTS_INLINE);
    return AGPU_OK;
}

agpu_error AVkCommandList::endRenderPass()
{
    if (!currentFramebuffer)
        return AGPU_INVALID_OPERATION;

    vkCmdEndRenderPass(commandBuffer);

    auto avkCurrentFramebuffer = currentFramebuffer.as<AVkFramebuffer> ();

    // Transition the color attachments.
    for (agpu_uint i = 0; i < avkCurrentFramebuffer->colorCount; ++i)
    {
        auto &desc = avkCurrentFramebuffer->attachmentViews[i].as<AVkTextureView> ()->description;
        VkImageSubresourceRange range = {};
        range.baseArrayLayer = desc.subresource_range.base_arraylayer;
        range.baseMipLevel = desc.subresource_range.base_miplevel;
        range.layerCount = 1;
        range.levelCount = 1;
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

        auto attachmentTexture = avkCurrentFramebuffer->attachmentTextures[i].as<AVkTexture> ();
        transitionImageUsageMode(attachmentTexture->image, attachmentTexture->description.usage_modes, AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT, attachmentTexture->description.main_usage_mode, range);
    }

    // Transition the depth stencil attachment if needed
    if(avkCurrentFramebuffer->hasDepthStencil)
    {
        auto depthStencilAttachment = avkCurrentFramebuffer->attachmentTextures.back().as<AVkTexture> ();

        auto &desc = avkCurrentFramebuffer->attachmentViews.back().as<AVkTextureView> ()->description;
        VkImageSubresourceRange range = {};
        range.baseArrayLayer = desc.subresource_range.base_arraylayer;
        range.baseMipLevel = desc.subresource_range.base_miplevel;
        range.layerCount = 1;
        range.levelCount = 1;
        range.aspectMask = depthStencilAttachment->imageAspect;

        auto depthStencilUsageMode = depthStencilAttachment->description.usage_modes & (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT);
        transitionImageUsageMode(depthStencilAttachment->image, depthStencilAttachment->description.usage_modes, agpu_texture_usage_mode_mask(depthStencilUsageMode), depthStencilAttachment->description.main_usage_mode, range);
    }

    // Signal the semaphore
    if(avkCurrentFramebuffer->signalSemaphore)
        addSignalSemaphore(avkCurrentFramebuffer->signalSemaphore);

    // Unset the current framebuffer
    currentFramebuffer.reset();
    return AGPU_OK;
}

agpu_error AVkCommandList::resolveFramebuffer(const agpu::framebuffer_ref &destFramebuffer, const agpu::framebuffer_ref &sourceFramebuffer)
{
    CHECK_POINTER(destFramebuffer);
    CHECK_POINTER(sourceFramebuffer);

    auto avkSourceFramebuffer = sourceFramebuffer.as<AVkFramebuffer> ();
    auto avkDestFramebuffer = destFramebuffer.as<AVkFramebuffer> ();
    if(avkSourceFramebuffer->colorCount < 1 || avkDestFramebuffer->colorCount < 1)
        return AGPU_INVALID_PARAMETER;

    if(avkSourceFramebuffer->width != avkDestFramebuffer->width ||
        avkSourceFramebuffer->height != avkDestFramebuffer->height)
        return AGPU_INVALID_PARAMETER;

    if(avkDestFramebuffer->waitSemaphore)
        addWaitSemaphore(avkDestFramebuffer->waitSemaphore, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

    if(avkDestFramebuffer->signalSemaphore)
        addSignalSemaphore(avkDestFramebuffer->signalSemaphore);

    return resolveTexture(avkSourceFramebuffer->attachmentTextures[0], 0, 0,
            avkDestFramebuffer->attachmentTextures[0], 0, 0,
            1, 1, AGPU_TEXTURE_ASPECT_COLOR);
}

agpu_error AVkCommandList::resolveTexture (const agpu::texture_ref &sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, const agpu::texture_ref &destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect )
{
    CHECK_POINTER(destTexture);
    CHECK_POINTER(sourceTexture);
    auto avkSourceTexture = sourceTexture.as<AVkTexture> ();
    auto avkDestTexture = destTexture.as<AVkTexture> ();
    if(avkSourceTexture->description.width != avkDestTexture->description.width ||
        avkSourceTexture->description.height != avkDestTexture->description.height)
        return AGPU_INVALID_PARAMETER;

    VkImageAspectFlags resolveAspects = avkSourceTexture->imageAspect & avkDestTexture->imageAspect;
    if(resolveAspects == 0)
        return AGPU_INVALID_PARAMETER;

    VkImageSubresourceRange range = {};
    range.layerCount = 1;
    range.levelCount = 1;
    range.aspectMask = avkSourceTexture->imageAspect;

    // Transition the textures into a copy layout.
    transitionImageUsageMode(avkSourceTexture->image, avkSourceTexture->description.usage_modes, avkSourceTexture->description.main_usage_mode, AGPU_TEXTURE_USAGE_COPY_SOURCE, range);
    transitionImageUsageMode(avkDestTexture->image, avkDestTexture->description.usage_modes, avkDestTexture->description.main_usage_mode, AGPU_TEXTURE_USAGE_COPY_DESTINATION, range);

    if(avkSourceTexture->description.sample_count == 1 && avkDestTexture->description.sample_count == 1)
    {
        VkImageBlit blitRegion = {};
        blitRegion.srcSubresource.aspectMask = resolveAspects;
        blitRegion.srcSubresource.baseArrayLayer = sourceLayer;
        blitRegion.srcSubresource.layerCount = layerCount;
        blitRegion.srcOffsets[1].x = avkSourceTexture->description.width;
        blitRegion.srcOffsets[1].y = avkSourceTexture->description.height;
        blitRegion.srcOffsets[1].z = 1;

        blitRegion.dstSubresource.aspectMask = resolveAspects;
        blitRegion.srcSubresource.baseArrayLayer = destLayer;
        blitRegion.dstSubresource.layerCount = layerCount;
        blitRegion.dstOffsets[1].x = avkDestTexture->description.width;
        blitRegion.dstOffsets[1].y = avkDestTexture->description.height;
        blitRegion.dstOffsets[1].z = 1;

        vkCmdBlitImage(commandBuffer, avkSourceTexture->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, avkDestTexture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitRegion, VK_FILTER_NEAREST);
    }
    else
    {
        VkImageResolve region = {};
        region.dstSubresource.aspectMask = resolveAspects;
        region.dstSubresource.baseArrayLayer = destLayer;
        region.dstSubresource.layerCount = layerCount;
        region.dstSubresource.mipLevel = destLevel;

        region.srcSubresource.aspectMask = resolveAspects;
        region.srcSubresource.baseArrayLayer = sourceLayer;
        region.srcSubresource.layerCount = layerCount;
        region.srcSubresource.mipLevel = sourceLevel;

        region.extent.width = avkSourceTexture->description.width;
        region.extent.height = avkSourceTexture->description.height;
        region.extent.depth = 1;

        vkCmdResolveImage(commandBuffer,
            avkSourceTexture->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            avkDestTexture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &region);
    }

    // Transition the textures back to their original layout
    transitionImageUsageMode(avkSourceTexture->image, avkSourceTexture->description.usage_modes, AGPU_TEXTURE_USAGE_COPY_SOURCE, avkSourceTexture->description.main_usage_mode, range);
    transitionImageUsageMode(avkDestTexture->image, avkDestTexture->description.usage_modes, AGPU_TEXTURE_USAGE_COPY_DESTINATION, avkDestTexture->description.main_usage_mode, range);

    return AGPU_OK;
}

agpu_error AVkCommandList::memoryBarrier(agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses)
{
    VkMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    barrier.srcAccessMask = VkAccessFlags(source_accesses);
    barrier.dstAccessMask = VkAccessFlags(dest_accesses);
    vkCmdPipelineBarrier(commandBuffer, VkPipelineStageFlags(source_stage), VkPipelineStageFlags(dest_stage),
            0, 1, &barrier, 0, nullptr, 0, nullptr);
    return AGPU_OK;
}

agpu_error AVkCommandList::bufferMemoryBarrier(const agpu::buffer_ref & buffer, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_size offset, agpu_size size)
{
    CHECK_POINTER(buffer);

    VkBufferMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.srcAccessMask = VkAccessFlags(source_accesses);
    barrier.dstAccessMask = VkAccessFlags(dest_accesses);
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.buffer = buffer.as<AVkBuffer> ()->handle;
    barrier.offset = offset;
    barrier.size = size;
    vkCmdPipelineBarrier(commandBuffer, VkPipelineStageFlags(source_stage), VkPipelineStageFlags(dest_stage),
            0, 0, nullptr, 1, &barrier, 0, nullptr);

    return AGPU_OK;
}

agpu_error AVkCommandList::textureMemoryBarrier(const agpu::texture_ref & texture, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_texture_usage_mode_mask old_usage, agpu_texture_usage_mode_mask new_usage, agpu_texture_subresource_range* subresource_range)
{
    CHECK_POINTER(texture);
    CHECK_POINTER(subresource_range);

    VkImageSubresourceRange range = {};

    range.aspectMask = VkImageAspectFlagBits(subresource_range->aspect);
    range.baseArrayLayer = subresource_range->base_arraylayer;
    range.baseMipLevel = subresource_range->base_miplevel;
    range.layerCount = subresource_range->layer_count;
    range.levelCount = subresource_range->level_count;

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask = VkAccessFlags(source_accesses);
    barrier.dstAccessMask = VkAccessFlags(dest_accesses);
    barrier.oldLayout = mapTextureUsageModeToLayout(old_usage);
    barrier.newLayout = mapTextureUsageModeToLayout(new_usage);
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = texture.as<AVkTexture> ()->image;
    barrier.subresourceRange = range;
    vkCmdPipelineBarrier(commandBuffer, VkPipelineStageFlags(source_stage), VkPipelineStageFlags(dest_stage),
            0, 0, nullptr, 0, nullptr, 1, &barrier);

    return AGPU_OK;
}

agpu_error AVkCommandList::pushBufferTransitionBarrier(const agpu::buffer_ref & buffer, agpu_buffer_usage_mask old_usage, agpu_buffer_usage_mask new_usage)
{
    CHECK_POINTER(buffer);

    transitionBufferUsageMode(buffer.as<AVkBuffer> ()->handle, old_usage, new_usage);
    BufferTransitionDesc transition;
    transition.buffer = buffer;
    transition.oldUsage = old_usage;
    transition.newUsage = new_usage;
    bufferTransitionStack.push_back(transition);
    return AGPU_OK;
}

agpu_error AVkCommandList::pushTextureTransitionBarrier(const agpu::texture_ref & texture, agpu_texture_usage_mode_mask old_usage, agpu_texture_usage_mode_mask new_usage, agpu_texture_subresource_range* subresource_range)
{
    CHECK_POINTER(texture);
    CHECK_POINTER(subresource_range);

    auto avkTexture = texture.as<AVkTexture> ();

    VkImageSubresourceRange range = {};

    range.baseArrayLayer = subresource_range->base_arraylayer;
    range.baseMipLevel = subresource_range->base_miplevel;
    range.layerCount = subresource_range->layer_count;
    range.levelCount = subresource_range->level_count;
    range.aspectMask = VkImageAspectFlagBits(subresource_range->aspect);

    transitionImageUsageMode(avkTexture->image, avkTexture->description.usage_modes, old_usage, new_usage, range);

    TextureTransitionDesc transition;
    transition.texture = texture;
    transition.oldUsage = old_usage;
    transition.newUsage = new_usage;
    transition.subresourceRange = range;
    textureTransitionStack.push_back(transition);
    return AGPU_OK;
}

agpu_error AVkCommandList::popBufferTransitionBarrier()
{
    if(bufferTransitionStack.empty())
        return AGPU_OUT_OF_BOUNDS;

    auto &transition = bufferTransitionStack.back();

    transitionBufferUsageMode(transition.buffer.as<AVkBuffer> ()->handle, transition.newUsage, transition.oldUsage);
    bufferTransitionStack.pop_back();
    return AGPU_OK;
}

agpu_error AVkCommandList::popTextureTransitionBarrier()
{
    if(textureTransitionStack.empty())
        return AGPU_OUT_OF_BOUNDS;

    auto &transition = textureTransitionStack.back();

    auto avkTexture = transition.texture.as<AVkTexture> ();
    transitionImageUsageMode(avkTexture->image, avkTexture->description.usage_modes, transition.newUsage, transition.oldUsage, transition.subresourceRange);
    textureTransitionStack.pop_back();
    return AGPU_OK;
}

agpu_error AVkCommandList::copyBuffer(const agpu::buffer_ref & source_buffer, agpu_size source_offset, const agpu::buffer_ref & dest_buffer, agpu_size dest_offset, agpu_size copy_size)
{
    CHECK_POINTER(source_buffer);
    CHECK_POINTER(dest_buffer);

    VkBufferCopy region = {};
    region.srcOffset = source_offset;
    region.dstOffset = dest_offset;
    region.size = copy_size;

    vkCmdCopyBuffer(commandBuffer, source_buffer.as<AVkBuffer> ()->handle, dest_buffer.as<AVkBuffer> ()->handle, 1, &region);
    return AGPU_OK;
}

agpu_error AVkCommandList::copyBufferToTexture(const agpu::buffer_ref & buffer, const agpu::texture_ref & texture, agpu_buffer_image_copy_region* copy_region)
{
    CHECK_POINTER(buffer);
    CHECK_POINTER(texture);
    CHECK_POINTER(copy_region);

    auto avkTexture = texture.as<AVkTexture>();

    VkBufferImageCopy bufferImageCopy = {};
    bufferImageCopy.bufferOffset = copy_region->buffer_offset;
    bufferImageCopy.bufferRowLength = copy_region->buffer_pitch / avkTexture->texelSize * avkTexture->texelWidth;
    bufferImageCopy.bufferImageHeight = copy_region->buffer_slice_pitch / copy_region->buffer_pitch * avkTexture->texelHeight;
    bufferImageCopy.imageSubresource.aspectMask = VkImageAspectFlags(copy_region->texture_subresource_level.aspect);
    bufferImageCopy.imageSubresource.mipLevel = copy_region->texture_subresource_level.miplevel;
    bufferImageCopy.imageSubresource.baseArrayLayer = copy_region->texture_subresource_level.base_arraylayer;
    bufferImageCopy.imageSubresource.layerCount = copy_region->texture_subresource_level.layer_count;
    bufferImageCopy.imageOffset.x = copy_region->texture_region.x;
    bufferImageCopy.imageOffset.y = copy_region->texture_region.y;
    bufferImageCopy.imageOffset.z = copy_region->texture_region.z;
    bufferImageCopy.imageExtent.width = copy_region->texture_region.width;
    bufferImageCopy.imageExtent.height = copy_region->texture_region.height;
    bufferImageCopy.imageExtent.depth = copy_region->texture_region.depth;

    vkCmdCopyBufferToImage(commandBuffer, buffer.as<AVkBuffer> ()->handle, texture.as<AVkTexture> ()->image, mapTextureUsageModeToLayout(copy_region->texture_usage_mode), 1, &bufferImageCopy);
    return AGPU_OK;
}

agpu_error AVkCommandList::copyTextureToBuffer(const agpu::texture_ref & texture, const agpu::buffer_ref & buffer, agpu_buffer_image_copy_region* copy_region)
{
    CHECK_POINTER(buffer);
    CHECK_POINTER(texture);
    CHECK_POINTER(copy_region);

    auto avkTexture = texture.as<AVkTexture>();

    VkBufferImageCopy bufferImageCopy = {};
    bufferImageCopy.bufferOffset = copy_region->buffer_offset;
    bufferImageCopy.bufferRowLength = copy_region->buffer_pitch / avkTexture->texelSize * avkTexture->texelWidth;
    bufferImageCopy.bufferImageHeight = copy_region->buffer_slice_pitch / copy_region->buffer_pitch * avkTexture->texelHeight;
    bufferImageCopy.imageSubresource.aspectMask = VkImageAspectFlags(copy_region->texture_subresource_level.aspect);
    bufferImageCopy.imageSubresource.mipLevel = copy_region->texture_subresource_level.miplevel;
    bufferImageCopy.imageSubresource.baseArrayLayer = copy_region->texture_subresource_level.base_arraylayer;
    bufferImageCopy.imageSubresource.layerCount = copy_region->texture_subresource_level.layer_count;
    bufferImageCopy.imageOffset.x = copy_region->texture_region.x;
    bufferImageCopy.imageOffset.y = copy_region->texture_region.y;
    bufferImageCopy.imageOffset.z = copy_region->texture_region.z;
    bufferImageCopy.imageExtent.width = copy_region->texture_region.width;
    bufferImageCopy.imageExtent.height = copy_region->texture_region.height;
    bufferImageCopy.imageExtent.depth = copy_region->texture_region.depth;

    vkCmdCopyImageToBuffer(commandBuffer, texture.as<AVkTexture> ()->image, mapTextureUsageModeToLayout(copy_region->texture_usage_mode), buffer.as<AVkBuffer> ()->handle, 1, &bufferImageCopy);
    return AGPU_OK;
}

agpu_error AVkCommandList::copyTexture(const agpu::texture_ref & source_texture, const agpu::texture_ref & dest_texture, agpu_image_copy_region* copy_region)
{
    CHECK_POINTER(source_texture);
    CHECK_POINTER(dest_texture);
    CHECK_POINTER(copy_region);

    VkImageCopy imageCopy = {};
    imageCopy.srcSubresource.aspectMask = VkImageAspectFlags(copy_region->source_subresource_level.aspect);
    imageCopy.srcSubresource.mipLevel = copy_region->source_subresource_level.miplevel;
    imageCopy.srcSubresource.baseArrayLayer = copy_region->source_subresource_level.base_arraylayer;
    imageCopy.srcSubresource.layerCount = copy_region->source_subresource_level.layer_count;
    imageCopy.srcOffset.x = copy_region->source_offset.x;
    imageCopy.srcOffset.y = copy_region->source_offset.y;
    imageCopy.srcOffset.z = copy_region->source_offset.z;

    imageCopy.dstSubresource.aspectMask = VkImageAspectFlags(copy_region->destination_subresource_level.aspect);
    imageCopy.dstSubresource.mipLevel = copy_region->destination_subresource_level.miplevel;
    imageCopy.dstSubresource.baseArrayLayer = copy_region->destination_subresource_level.base_arraylayer;
    imageCopy.dstSubresource.layerCount = copy_region->destination_subresource_level.layer_count;
    imageCopy.dstOffset.x = copy_region->destination_offset.x;
    imageCopy.dstOffset.y = copy_region->destination_offset.y;
    imageCopy.dstOffset.z = copy_region->destination_offset.z;

    imageCopy.extent.width = copy_region->extent.width;
    imageCopy.extent.height = copy_region->extent.height;
    imageCopy.extent.depth = copy_region->extent.depth;

    vkCmdCopyImage(commandBuffer,
            source_texture.as<AVkTexture> ()->image, mapTextureUsageModeToLayout(copy_region->source_usage_mode),
            dest_texture.as<AVkTexture> ()->image, mapTextureUsageModeToLayout(copy_region->destination_usage_mode),
            1, &imageCopy);
    return AGPU_OK;
}

void AVkCommandList::addWaitSemaphore(VkSemaphore semaphore, VkPipelineStageFlags dstStageMask)
{
    for(size_t i = 0; i < waitSemaphores.size(); ++i)
    {
        if (waitSemaphores[i] == semaphore)
        {
            waitSemaphoresDstStageMask[i] |= dstStageMask;
            return;
        }
    }

    waitSemaphores.push_back(semaphore);
    waitSemaphoresDstStageMask.push_back(dstStageMask);
}

void AVkCommandList::addSignalSemaphore(VkSemaphore semaphore)
{
    for(auto s : signalSemaphores)
    {
        if(s == semaphore)
            return;
    }

    signalSemaphores.push_back(semaphore);
}

} // End of namespace AgpuVulkan
