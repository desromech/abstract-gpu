#include "command_list.hpp"
#include "command_allocator.hpp"
#include "framebuffer.hpp"
#include "renderpass.hpp"
#include "texture.hpp"
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

    VkCommandBufferAllocateInfo info;
    memset(&info, 0, sizeof(info));
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

    VkCommandBufferInheritanceInfo commandBufferInheritance;
    memset(&commandBufferInheritance, 0, sizeof(commandBufferInheritance));
    commandBufferInheritance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

    VkCommandBufferBeginInfo bufferBeginInfo;
    memset(&bufferBeginInfo, 0, sizeof(bufferBeginInfo));
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

    VkCommandBufferInheritanceInfo commandBufferInheritance;
    memset(&commandBufferInheritance, 0, sizeof(commandBufferInheritance));
    commandBufferInheritance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

    VkCommandBufferBeginInfo bufferBeginInfo;
    memset(&bufferBeginInfo, 0, sizeof(bufferBeginInfo));
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

    VkCommandBufferInheritanceInfo commandBufferInheritance;
    memset(&commandBufferInheritance, 0, sizeof(commandBufferInheritance));
    commandBufferInheritance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

    VkCommandBufferBeginInfo bufferBeginInfo;
    memset(&bufferBeginInfo, 0, sizeof(bufferBeginInfo));
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
    if ((index_buffer.as<AVkBuffer> ()->description.binding & AGPU_ELEMENT_ARRAY_BUFFER) == 0)
        return AGPU_INVALID_PARAMETER;

    vkCmdBindIndexBuffer(commandBuffer, index_buffer.as<AVkBuffer> ()->getDrawBuffer(), offset, indexTypeForStride(index_size));
    return AGPU_OK;
}

agpu_error AVkCommandList::useDrawIndirectBuffer(const agpu::buffer_ref &draw_buffer)
{
    CHECK_POINTER(draw_buffer);
    if ((draw_buffer.as<AVkBuffer> ()->description.binding & AGPU_DRAW_INDIRECT_BUFFER) == 0)
        return AGPU_INVALID_PARAMETER;

    drawIndirectBuffer = draw_buffer;
    return AGPU_OK;
}

agpu_error AVkCommandList::useComputeDispatchIndirectBuffer(const agpu::buffer_ref &dispatch_buffer)
{
    CHECK_POINTER(dispatch_buffer);
    if ((dispatch_buffer.as<AVkBuffer> ()->description.binding & AGPU_COMPUTE_DISPATCH_INDIRECT_BUFFER) == 0)
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

    vkCmdDrawIndirect(commandBuffer, drawIndirectBuffer.as<AVkBuffer> ()->getDrawBuffer(), offset, drawcount, drawIndirectBuffer.as<AVkBuffer> ()->description.stride);
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

    vkCmdDrawIndexedIndirect(commandBuffer, drawIndirectBuffer.as<AVkBuffer> ()->getDrawBuffer(), offset, drawcount, drawIndirectBuffer.as<AVkBuffer> ()->description.stride);
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

    vkCmdDispatchIndirect(commandBuffer, computeDispatchIndirectBuffer.as<AVkBuffer> ()->getDrawBuffer(), offset);
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

agpu_error AVkCommandList::setImageLayout(VkImage image, VkImageSubresourceRange range, VkImageAspectFlags aspect, VkImageLayout sourceLayout, VkImageLayout destLayout, VkAccessFlags srcAccessMask)
{
    VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    auto barrier = deviceForVk->barrierForImageLayoutTransition(image, range, aspect, sourceLayout, destLayout, srcAccessMask, srcStages, destStages);

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
    auto destLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    for (agpu_uint i = 0; i < avkCurrentFramebuffer->colorCount; ++i)
    {
        VkImageSubresourceRange range;
        memset(&range, 0, sizeof(range));

        auto &desc = avkCurrentFramebuffer->attachmentDescriptions[i];
        range.baseArrayLayer = desc.subresource_range.base_arraylayer;
        range.baseMipLevel = desc.subresource_range.base_miplevel;
        range.layerCount = 1;
        range.levelCount = 1;
        auto attachmentTexture = avkCurrentFramebuffer->attachmentTextures[i].as<AVkTexture> ();
        setImageLayout(attachmentTexture->image, range, VK_IMAGE_ASPECT_COLOR_BIT, attachmentTexture->initialLayout, destLayout, attachmentTexture->initialLayoutAccessBits);
    }

    // Transition the depth stencil attachment, if needed.
    if(avkCurrentFramebuffer->hasDepthStencil)
    {
        auto depthStencilAttachment = avkCurrentFramebuffer->attachmentTextures.back().as<AVkTexture> ();
        if(depthStencilAttachment->initialLayout != VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            auto &desc = avkCurrentFramebuffer->attachmentDescriptions.back();
            VkImageSubresourceRange range;
            memset(&range, 0, sizeof(range));
            range.baseArrayLayer = desc.subresource_range.base_arraylayer;
            range.baseMipLevel = desc.subresource_range.base_miplevel;
            range.layerCount = 1;
            range.levelCount = 1;

            // TODO: Check for the stencil bit here.
            setImageLayout(depthStencilAttachment->image, range, VK_IMAGE_ASPECT_DEPTH_BIT,
                depthStencilAttachment->initialLayout, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                depthStencilAttachment->initialLayoutAccessBits);
        }
    }

    // Begin the render pass.
    auto avkRenderPass = renderpass.as<AVkRenderPass> ();
    VkRenderPassBeginInfo passBeginInfo;
    memset(&passBeginInfo, 0, sizeof(passBeginInfo));
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
    auto sourceLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    for (agpu_uint i = 0; i < avkCurrentFramebuffer->colorCount; ++i)
    {
        auto &desc = avkCurrentFramebuffer->attachmentDescriptions.back();
        VkImageSubresourceRange range;
        memset(&range, 0, sizeof(range));
        range.baseArrayLayer = desc.subresource_range.base_arraylayer;
        range.baseMipLevel = desc.subresource_range.base_miplevel;
        range.layerCount = 1;
        range.levelCount = 1;

        auto attachmentTexture = avkCurrentFramebuffer->attachmentTextures[i].as<AVkTexture> ();
        auto destLayout = attachmentTexture->initialLayout;
        setImageLayout(attachmentTexture->image, range, VK_IMAGE_ASPECT_COLOR_BIT, sourceLayout, destLayout, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    }

    // Transition the depth stencil attachment if needed
    if(avkCurrentFramebuffer->hasDepthStencil)
    {
        auto depthStencilAttachment = avkCurrentFramebuffer->attachmentTextures.back().as<AVkTexture> ();
        if(depthStencilAttachment->initialLayout != VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            auto &desc = avkCurrentFramebuffer->attachmentDescriptions.back();
            VkImageSubresourceRange range;
            memset(&range, 0, sizeof(range));
            range.baseArrayLayer = desc.subresource_range.base_arraylayer;
            range.baseMipLevel = desc.subresource_range.base_miplevel;
            range.layerCount = 1;
            range.levelCount = 1;

            // TODO: Check for the stencil bit here.
            setImageLayout(depthStencilAttachment->image, range, VK_IMAGE_ASPECT_DEPTH_BIT,
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, depthStencilAttachment->initialLayout,
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
        }
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

    VkImageSubresourceRange range;
    memset(&range, 0, sizeof(range));
    range.layerCount = 1;
    range.levelCount = 1;

    // Transition the source color attachment.
    auto sourceInitialLayout = avkSourceTexture->initialLayout;
    auto sourceResolveLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    if(sourceInitialLayout != sourceResolveLayout)
        setImageLayout(avkSourceTexture->image, range, avkSourceTexture->imageAspect, sourceInitialLayout, sourceResolveLayout, avkSourceTexture->initialLayoutAccessBits);

    // Transition the dest color attachments.
    auto destInitialLayout = avkDestTexture->initialLayout;
    auto destResolveLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    if(destInitialLayout != destResolveLayout)
        setImageLayout(avkDestTexture->image, range, avkDestTexture->imageAspect, destInitialLayout, destResolveLayout, avkDestTexture->initialLayoutAccessBits);

    if(avkSourceTexture->description.sample_count == 1 && avkDestTexture->description.sample_count == 1)
    {
        VkImageBlit blitRegion;
        memset(&blitRegion, 0, sizeof(blitRegion));
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

        vkCmdBlitImage(commandBuffer, avkSourceTexture->image, sourceResolveLayout, avkDestTexture->image, destResolveLayout, 1, &blitRegion, VK_FILTER_NEAREST);
    }
    else
    {
        VkImageResolve region;
        memset(&region, 0, sizeof(region));
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
            avkSourceTexture->image, sourceResolveLayout,
            avkDestTexture->image, destResolveLayout,
            1, &region);
    }

    // Transition the destination back to its original layout
    if(destInitialLayout != destResolveLayout)
        setImageLayout(avkDestTexture->image, range, avkDestTexture->imageAspect, destResolveLayout, destInitialLayout, VkAccessFlagBits(0));
    if(sourceInitialLayout != sourceResolveLayout)
        setImageLayout(avkSourceTexture->image, range, avkSourceTexture->imageAspect, sourceResolveLayout, sourceInitialLayout, VkAccessFlagBits(0));

    return AGPU_OK;
}

agpu_error AVkCommandList::memoryBarrier(agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses)
{
    VkMemoryBarrier barrier;
    memset(&barrier, 0, sizeof(barrier));
    barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    barrier.srcAccessMask = VkAccessFlags(source_accesses);
    barrier.dstAccessMask = VkAccessFlags(dest_accesses);
    vkCmdPipelineBarrier(commandBuffer, VkPipelineStageFlags(source_stage), VkPipelineStageFlags(dest_stage),
            0, 1, &barrier, 0, nullptr, 0, nullptr);
    return AGPU_OK;
}


} // End of namespace AgpuVulkan
