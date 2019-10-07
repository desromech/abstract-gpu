#include "command_list.hpp"
#include "command_allocator.hpp"
#include "framebuffer.hpp"
#include "renderpass.hpp"
#include "texture.hpp"
#include "texture_view.hpp"
#include "buffer.hpp"
#include "vertex_binding.hpp"
#include "pipeline_state.hpp"
#include "shader_signature.hpp"
#include "shader_resource_binding.hpp"

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
    if (!shaderSignature)
        return AGPU_INVALID_OPERATION;

    auto avkBindings = binding.as<AVkShaderResourceBinding> ();
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            shaderSignature.as<AVkShaderSignature> ()->layout,
            avkBindings->elementIndex, 1, &avkBindings->descriptorSet, 0, nullptr);
    return AGPU_OK;
}

agpu_error AVkCommandList::useComputeShaderResources(const agpu::shader_resource_binding_ref &binding)
{
    CHECK_POINTER(binding);
    if (!shaderSignature)
        return AGPU_INVALID_OPERATION;

    auto avkBindings = binding.as<AVkShaderResourceBinding> ();
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
            shaderSignature.as<AVkShaderSignature> ()->layout,
            avkBindings->elementIndex, 1, &avkBindings->descriptorSet, 0, nullptr);
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
    return AGPU_UNIMPLEMENTED;
}

agpu_error AVkCommandList::textureMemoryBarrier(const agpu::texture_ref & texture, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_subresource_range* subresource_range)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AVkCommandList::pushBufferTransitionBarrier(const agpu::buffer_ref & buffer, agpu_buffer_usage_mask new_usage)
{
    // TODO: Emit the appropiate buffer memory barrier.
    return AGPU_OK;
}

agpu_error AVkCommandList::pushTextureTransitionBarrier(const agpu::texture_ref & texture, agpu_texture_usage_mode_mask new_usage, agpu_subresource_range* subresource_range)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error AVkCommandList::popBufferTransitionBarrier(const agpu::buffer_ref & buffer)
{
    return AGPU_OK;
}

agpu_error AVkCommandList::popTextureTransitionBarrier(const agpu::texture_ref & texture, agpu_subresource_range* subresource_range)
{
    return AGPU_UNIMPLEMENTED;
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
    return AGPU_UNIMPLEMENTED;
}

agpu_error AVkCommandList::copyTextureToBuffer(const agpu::texture_ref & texture, const agpu::buffer_ref & buffer, agpu_buffer_image_copy_region* copy_region)
{
    return AGPU_UNIMPLEMENTED;
}

} // End of namespace AgpuVulkan
