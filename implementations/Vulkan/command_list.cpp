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

_agpu_command_list::_agpu_command_list(agpu_device *device)
    : device(device)
{
    currentFramebuffer = nullptr;
    drawIndirectBuffer = nullptr;
    shaderSignature = nullptr;
    isClosed = false;
    isSecondaryContent = false;
}

void _agpu_command_list::lostReferences()
{
    vkFreeCommandBuffers(device->device, allocator->commandPool, 1, &commandBuffer);
    if (allocator)
        allocator->release();
    if (currentFramebuffer)
        currentFramebuffer->release();
}

_agpu_command_list* _agpu_command_list::create(agpu_device* device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state)
{
    if (!allocator)
        return nullptr;

    VkCommandBufferAllocateInfo info;
    memset(&info, 0, sizeof(info));
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandBufferCount = 1;
    info.commandPool = allocator->commandPool;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    if(type == AGPU_COMMAND_LIST_TYPE_BUNDLE)
        info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    
    VkCommandBuffer commandBuffer;
    auto error = vkAllocateCommandBuffers(device->device, &info, &commandBuffer);
    if (error)
        return nullptr;

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
        vkFreeCommandBuffers(device->device, allocator->commandPool, 1, &commandBuffer);
        return nullptr;
    }

    if (initial_pipeline_state)
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, initial_pipeline_state->pipeline);

    auto result = new _agpu_command_list(device);
    result->commandBuffer = commandBuffer;
    result->allocator = allocator;
    result->queueFamilyIndex = allocator->queueFamilyIndex;
    result->resetState();
    allocator->retain();
    return result;
}

agpu_error _agpu_command_list::close()
{
    auto error = vkEndCommandBuffer(commandBuffer);
    CONVERT_VULKAN_ERROR(error);
    return AGPU_OK;
}

agpu_error _agpu_command_list::reset(agpu_command_allocator* newAllocator, agpu_pipeline_state* initial_pipeline_state)
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
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, initial_pipeline_state->pipeline);

    resetState();
    return AGPU_OK;
}

void _agpu_command_list::resetState()
{
    if (currentFramebuffer)
    {
        currentFramebuffer->release();
        currentFramebuffer = nullptr;
    }

    if (drawIndirectBuffer)
    {
        drawIndirectBuffer->release();
        drawIndirectBuffer = nullptr;
    }

    if (shaderSignature)
    {
        shaderSignature->release();
        shaderSignature = nullptr;
    }

    isSecondaryContent = false;

    vkCmdSetStencilReference(commandBuffer, VK_STENCIL_FRONT_AND_BACK, 0);
}

agpu_error _agpu_command_list::setShaderSignature(agpu_shader_signature* signature)
{
    CHECK_POINTER(signature);
    signature->retain();
    if (shaderSignature)
        shaderSignature->release();
    shaderSignature = signature;
    return AGPU_OK;
}

agpu_error _agpu_command_list::setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    VkViewport viewport;
    viewport.width = w;
    viewport.height = h;
    viewport.x = x;
    viewport.y = y;
    viewport.maxDepth = 1.0f;
    viewport.minDepth = 0.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    return AGPU_OK;

}

agpu_error _agpu_command_list::setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    VkRect2D rect;
    rect.extent.width = w;
    rect.extent.height = h;
    rect.offset.x = x;
    rect.offset.y = y;
    vkCmdSetScissor(commandBuffer, 0, 1, &rect);
    return AGPU_OK;
}

agpu_error _agpu_command_list::usePipelineState(agpu_pipeline_state* pipeline)
{
    CHECK_POINTER(pipeline);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
    return AGPU_OK;
}

agpu_error _agpu_command_list::useVertexBinding(agpu_vertex_binding* vertex_binding)
{
    CHECK_POINTER(vertex_binding);
    vkCmdBindVertexBuffers(commandBuffer, 0, vertex_binding->vulkanBuffers.size(), &vertex_binding->vulkanBuffers[0], &vertex_binding->offsets[0]);
    return AGPU_OK;
}

agpu_error _agpu_command_list::useIndexBuffer(agpu_buffer* index_buffer)
{
    CHECK_POINTER(index_buffer);
    if (index_buffer->description.binding != AGPU_ELEMENT_ARRAY_BUFFER)
        return AGPU_INVALID_PARAMETER;

    vkCmdBindIndexBuffer(commandBuffer, index_buffer->getDrawBuffer(), 0, index_buffer->getIndexType());
    return AGPU_OK;
}

agpu_error _agpu_command_list::setPrimitiveTopology(agpu_primitive_topology topology)
{
    // Nothing to do here.
    return AGPU_OK;
}

agpu_error _agpu_command_list::useDrawIndirectBuffer(agpu_buffer* draw_buffer)
{
    CHECK_POINTER(draw_buffer);
    if (draw_buffer->description.binding != AGPU_DRAW_INDIRECT_BUFFER)
        return AGPU_INVALID_PARAMETER;

    draw_buffer->retain();
    if (drawIndirectBuffer)
        drawIndirectBuffer->release();
    drawIndirectBuffer = draw_buffer;
    return AGPU_OK;
}

agpu_error _agpu_command_list::useShaderResources(agpu_shader_resource_binding* binding)
{
    CHECK_POINTER(binding);
    if (!shaderSignature)
        return AGPU_INVALID_OPERATION;
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shaderSignature->layout, binding->elementIndex, 1, &binding->descriptorSet, 0, nullptr);
    return AGPU_OK;
}

agpu_error _agpu_command_list::drawArrays(agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance)
{
    vkCmdDraw(commandBuffer, vertex_count, instance_count, first_vertex, base_instance);
    return AGPU_OK;
}

agpu_error _agpu_command_list::drawElements(agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance)
{
    vkCmdDrawIndexed(commandBuffer, index_count, instance_count, first_index, base_vertex, base_instance);
    return AGPU_OK;
}

agpu_error _agpu_command_list::drawElementsIndirect(agpu_size offset)
{
    return multiDrawElementsIndirect(offset, 1);
}

agpu_error _agpu_command_list::multiDrawElementsIndirect(agpu_size offset, agpu_size drawcount)
{
    if (!drawIndirectBuffer)
        return AGPU_INVALID_OPERATION;

    //vkCmdDrawIndexedIndirect(commandBuffer, drawIndirectBuffer->getDrawBuffer(), offset, drawcount, );
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_command_list::setStencilReference(agpu_uint reference)
{
    vkCmdSetStencilReference(commandBuffer, VK_STENCIL_FRONT_AND_BACK, reference);
    return AGPU_OK;
}

agpu_error _agpu_command_list::executeBundle(agpu_command_list* bundle)
{
    CHECK_POINTER(bundle);
    if (!bundle->isClosed || !bundle->isSecondaryContent)
        return AGPU_INVALID_PARAMETER;

    vkCmdExecuteCommands(commandBuffer, 1, &bundle->commandBuffer);
    return AGPU_OK;
}

agpu_error _agpu_command_list::setImageLayout(VkImage image, VkImageAspectFlagBits aspect, VkImageLayout sourceLayout, VkImageLayout destLayout, VkAccessFlagBits srcAccessMask)
{
    auto barrier = device->barrierForImageLayoutTransition(image, aspect, sourceLayout, destLayout, srcAccessMask);
    VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    vkCmdPipelineBarrier(commandBuffer, srcStages, srcStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    return AGPU_OK;
}

agpu_error _agpu_command_list::beginRenderPass(agpu_renderpass *renderpass, agpu_framebuffer* framebuffer, agpu_bool secondaryContent)
{
    CHECK_POINTER(renderpass);
    CHECK_POINTER(framebuffer);

    // Store the framebuffer
    framebuffer->retain();
    if (currentFramebuffer)
        currentFramebuffer->release();
    currentFramebuffer = framebuffer;
    isSecondaryContent = secondaryContent;

    // Transition the color attachments.
    auto sourceLayout = VK_IMAGE_LAYOUT_GENERAL;
    if (currentFramebuffer->swapChainFramebuffer)
        sourceLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    auto destLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    for (agpu_uint i = 0; i < currentFramebuffer->colorCount; ++i)
        setImageLayout(currentFramebuffer->attachmentTextures[i]->image, VK_IMAGE_ASPECT_COLOR_BIT, sourceLayout, destLayout, VkAccessFlagBits(0));

    // Begin the render pass.
    VkRenderPassBeginInfo passBeginInfo;
    memset(&passBeginInfo, 0, sizeof(passBeginInfo));
    passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    passBeginInfo.renderPass = renderpass->handle;
    passBeginInfo.framebuffer = framebuffer->framebuffer;
    passBeginInfo.renderArea.extent.width = framebuffer->width;
    passBeginInfo.renderArea.extent.height = framebuffer->height;

    // Set the clear values.
    if (!renderpass->clearValues.empty())
    {
        passBeginInfo.clearValueCount = renderpass->clearValues.size();
        passBeginInfo.pClearValues = &renderpass->clearValues[0];
    }

    vkCmdBeginRenderPass(commandBuffer, &passBeginInfo, secondaryContent ? VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS : VK_SUBPASS_CONTENTS_INLINE);
    return AGPU_OK;
}

agpu_error _agpu_command_list::endRenderPass()
{
    if (!currentFramebuffer)
        return AGPU_INVALID_OPERATION;

    vkCmdEndRenderPass(commandBuffer);

    // Transition the color attachments.
    auto destLayout = VK_IMAGE_LAYOUT_GENERAL;
    if (currentFramebuffer->swapChainFramebuffer)
        destLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    auto sourceLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    for (agpu_uint i = 0; i < currentFramebuffer->colorCount; ++i)
        setImageLayout(currentFramebuffer->attachmentTextures[i]->image, VK_IMAGE_ASPECT_COLOR_BIT, sourceLayout, destLayout, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

    // Unset the current framebuffer
    currentFramebuffer->release();
    currentFramebuffer = nullptr;
    return AGPU_OK;
}

agpu_error _agpu_command_list::resolveFramebuffer(agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer)
{
    return AGPU_UNIMPLEMENTED;
}

// The exported C interface
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

AGPU_EXPORT agpu_error agpuResetCommandList(agpu_command_list* command_list, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state)
{
    CHECK_POINTER(command_list);
    return command_list->reset(allocator, initial_pipeline_state);
}

AGPU_EXPORT agpu_error agpuBeginRenderPass(agpu_command_list* command_list, agpu_renderpass *renderpass, agpu_framebuffer* framebuffer, agpu_bool secondaryContent)
{
    CHECK_POINTER(command_list);
    return command_list->beginRenderPass(renderpass, framebuffer, secondaryContent);
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
