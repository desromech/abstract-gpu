#ifndef AGPU_COMMAND_LIST_HPP
#define AGPU_COMMAND_LIST_HPP

#include "device.hpp"

namespace AgpuVulkan
{

class AVkCommandList : public agpu::command_list
{
public:
    AVkCommandList(const agpu::device_ref &device);
    ~AVkCommandList();

    static agpu::command_list_ref create(const agpu::device_ref &device, agpu_command_list_type type, const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initial_pipeline_state);

    virtual agpu_error setShaderSignature(const agpu::shader_signature_ref &signature) override;
    virtual agpu_error setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h) override;
    virtual agpu_error setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h) override;
    virtual agpu_error usePipelineState(const agpu::pipeline_state_ref &pipeline) override;
    virtual agpu_error useVertexBinding(const agpu::vertex_binding_ref &vertex_binding) override;
    virtual agpu_error useIndexBuffer(const agpu::buffer_ref &index_buffer) override;
    virtual agpu_error useIndexBufferAt(const agpu::buffer_ref &index_buffer, agpu_size offset, agpu_size index_size) override;
    virtual agpu_error useDrawIndirectBuffer(const agpu::buffer_ref &draw_buffer) override;
    virtual agpu_error useComputeDispatchIndirectBuffer(const agpu::buffer_ref &dispatch_buffer) override;
    virtual agpu_error useShaderResources(const agpu::shader_resource_binding_ref &binding) override;
    virtual agpu_error useComputeShaderResources(const agpu::shader_resource_binding_ref &binding) override;
    virtual agpu_error pushConstants (agpu_uint offset, agpu_uint size, agpu_pointer values) override;
    virtual agpu_error drawArrays(agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance) override;
    virtual agpu_error drawArraysIndirect(agpu_size offset, agpu_size drawcount) override;
    virtual agpu_error drawElements(agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance) override;
    virtual agpu_error drawElementsIndirect(agpu_size offset, agpu_size drawcount) override;
    virtual agpu_error dispatchCompute ( agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z ) override;
    virtual agpu_error dispatchComputeIndirect ( agpu_size offset ) override;
    virtual agpu_error setStencilReference(agpu_uint reference) override;
    virtual agpu_error executeBundle(const agpu::command_list_ref &bundle) override;

    virtual agpu_error close() override;
    virtual agpu_error reset(const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initial_pipeline_state) override;
    virtual agpu_error resetBundle (const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initial_pipeline_state, agpu_inheritance_info* inheritance_info ) override;

    virtual agpu_error beginRenderPass(const agpu::renderpass_ref &renderpass, const agpu::framebuffer_ref &framebuffer, agpu_bool secondaryContent) override;
    virtual agpu_error endRenderPass() override;
    virtual agpu_error resolveFramebuffer(const agpu::framebuffer_ref &destFramebuffer, const agpu::framebuffer_ref &sourceFramebuffer) override;
    virtual agpu_error resolveTexture (const agpu::texture_ref &sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, const agpu::texture_ref &destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect ) override;

    virtual agpu_error memoryBarrier(agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses) override;
    virtual agpu_error bufferMemoryBarrier(const agpu::buffer_ref & buffer, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_size offset, agpu_size size) override;
    virtual agpu_error textureMemoryBarrier(const agpu::texture_ref & texture, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_subresource_range* subresource_range) override;
    virtual agpu_error pushBufferTransitionBarrier(const agpu::buffer_ref & buffer, agpu_buffer_usage_mask new_usage) override;
    virtual agpu_error pushTextureTransitionBarrier(const agpu::texture_ref & texture, agpu_texture_usage_mode_mask new_usage, agpu_subresource_range* subresource_range) override;
    virtual agpu_error popBufferTransitionBarrier() override;
    virtual agpu_error popTextureTransitionBarrier() override;
    virtual agpu_error copyBuffer(const agpu::buffer_ref & source_buffer, agpu_size source_offset, const agpu::buffer_ref & dest_buffer, agpu_size dest_offset, agpu_size copy_size) override;
    virtual agpu_error copyBufferToTexture(const agpu::buffer_ref & buffer, const agpu::texture_ref & texture, agpu_buffer_image_copy_region* copy_region) override;
    virtual agpu_error copyTextureToBuffer(const agpu::texture_ref & texture, const agpu::buffer_ref & buffer, agpu_buffer_image_copy_region* copy_region) override;

    void addWaitSemaphore(VkSemaphore semaphore, VkPipelineStageFlags dstStageMask);
    void addSignalSemaphore(VkSemaphore semaphore);

    agpu::device_ref device;
    agpu::command_allocator_ref allocator;
    agpu_command_list_type type;
    agpu_uint queueFamilyIndex;
    VkCommandBuffer commandBuffer;

    std::vector<VkSemaphore> waitSemaphores;
    std::vector<VkPipelineStageFlags> waitSemaphoresDstStageMask;
    std::vector<VkSemaphore> signalSemaphores;

private:
    agpu_buffer_usage_mask getCurrentBufferUsageMode(const agpu::buffer_ref &buffer);
    agpu_texture_usage_mode_mask getCurrentTextureUsageMode(const agpu::texture_ref &texture);

    void resetState();
    agpu_error transitionImageUsageMode(VkImage image, agpu_texture_usage_mode_mask allowedUsages, agpu_texture_usage_mode_mask sourceUsage, agpu_texture_usage_mode_mask destUsage, VkImageSubresourceRange range);
    agpu_error transitionBufferUsageMode(VkBuffer buffer, agpu_buffer_usage_mask oldUsageMode, agpu_buffer_usage_mask newUsageMode);

    agpu::framebuffer_ref currentFramebuffer;
    agpu_bool isClosed;
    agpu_bool isSecondaryContent;

    agpu::buffer_ref drawIndirectBuffer;
    agpu::buffer_ref computeDispatchIndirectBuffer;
    agpu::shader_signature_ref shaderSignature;

    std::vector<std::pair<agpu::buffer_ref, agpu_buffer_usage_mask>> bufferTransitionStack;
};

} // End of namespace AgpuVulkan

#endif //AGPU_COMMAND_LIST_HPP
