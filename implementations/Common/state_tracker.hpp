#ifndef AGPU_STATE_TRACKER_HPP
#define AGPU_STATE_TRACKER_HPP

#include "state_tracker_cache.hpp"
#include <vector>

namespace AgpuCommon
{

/**
 * I am a generic state tracker implementation. I provide an OpenGL core profile
 * and D3D 11 style programming interface to generate and enqueue command lists.
 * I work by deferring rendering commands, and generating pipeline state objects
 * on the fly which are store in an state tracker cache.
 */
class AbstractStateTracker : public agpu::state_tracker
{
public:
    AbstractStateTracker(const agpu::state_tracker_cache_ref &cache,
            const agpu::device_ref &device,
            agpu_command_list_type type,
            const agpu::command_queue_ref &commandQueue);
    ~AbstractStateTracker();

    virtual agpu_error beginRecordingCommands() override;
    virtual agpu_error endRecordingAndFlushCommands() override;

    virtual agpu_error reset() override;

    // Compute pipeline methods
	virtual agpu_error resetComputePipeline() override;
	virtual agpu_error setComputeStage(const agpu::shader_ref & shader, agpu_cstring entryPoint) override;
	virtual agpu_error setComputeStageWithMain(const agpu::shader_ref & shader) override;

    // Graphics pipeline methods
    virtual agpu_error resetGraphicsPipeline() override;
	virtual agpu_error setVertexStage(const agpu::shader_ref & shader, agpu_cstring entryPoint) override;
	virtual agpu_error setVertexStageWithMain(const agpu::shader_ref & shader) override;
	virtual agpu_error setFragmentStage(const agpu::shader_ref & shader, agpu_cstring entryPoint) override;
	virtual agpu_error setFragmentStageWithMain(const agpu::shader_ref & shader) override;
    virtual agpu_error setGeometryStage(const agpu::shader_ref & shader, agpu_cstring entryPoint) override;
    virtual agpu_error setGeometryStageWithMain(const agpu::shader_ref & shader) override;
	virtual agpu_error setTessellationControlStage(const agpu::shader_ref & shader, agpu_cstring entryPoint) override;
	virtual agpu_error setTessellationControlStageWithMain(const agpu::shader_ref & shader) override;
	virtual agpu_error setTessellationEvaluationStage(const agpu::shader_ref & shader, agpu_cstring entryPoint) override;
	virtual agpu_error setTessellationEvaluationStageWithMain(const agpu::shader_ref & shader) override;
	virtual agpu_error setBlendState(agpu_int renderTargetMask, agpu_bool enabled) override;
	virtual agpu_error setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation) override;
	virtual agpu_error setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled) override;
	virtual agpu_error setFrontFace(agpu_face_winding winding) override;
	virtual agpu_error setCullMode(agpu_cull_mode mode) override;
	virtual agpu_error setDepthBias(agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor) override;
	virtual agpu_error setDepthState(agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function) override;
	virtual agpu_error setPolygonMode(agpu_polygon_mode mode) override;
	virtual agpu_error setStencilState(agpu_bool enabled, agpu_int writeMask, agpu_int readMask) override;
	virtual agpu_error setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction) override;
	virtual agpu_error setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction) override;
	virtual agpu_error setPrimitiveType(agpu_primitive_topology type) override;
	virtual agpu_error setVertexLayout(const agpu::vertex_layout_ref & layout) override;
	virtual agpu_error setShaderSignature(const agpu::shader_signature_ref & signature) override;
	virtual agpu_error setSampleDescription(agpu_uint sample_count, agpu_uint sample_quality) override;

    // Command list methods.
	virtual agpu_error setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h) override;
	virtual agpu_error setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h) override;
	virtual agpu_error useVertexBinding(const agpu::vertex_binding_ref & vertex_binding) override;
	virtual agpu_error useIndexBuffer(const agpu::buffer_ref & index_buffer) override;
	virtual agpu_error useIndexBufferAt(const agpu::buffer_ref & index_buffer, agpu_size offset, agpu_size index_size) override;
	virtual agpu_error useDrawIndirectBuffer(const agpu::buffer_ref & draw_buffer) override;
	virtual agpu_error useComputeDispatchIndirectBuffer(const agpu::buffer_ref & buffer) override;
	virtual agpu_error useShaderResources(const agpu::shader_resource_binding_ref & binding) override;
	virtual agpu_error useShaderResourcesInSlot(const agpu::shader_resource_binding_ref & binding, agpu_uint slot) override;
	virtual agpu_error useComputeShaderResources(const agpu::shader_resource_binding_ref & binding) override;
	virtual agpu_error useComputeShaderResourcesInSlot(const agpu::shader_resource_binding_ref & binding, agpu_uint slot) override;
	virtual agpu_error drawArrays(agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance) override;
	virtual agpu_error drawArraysIndirect(agpu_size offset, agpu_size drawcount) override;
	virtual agpu_error drawElements(agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance) override;
	virtual agpu_error drawElementsIndirect(agpu_size offset, agpu_size drawcount) override;
	virtual agpu_error dispatchCompute(agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z) override;
	virtual agpu_error dispatchComputeIndirect(agpu_size offset) override;
	virtual agpu_error setStencilReference(agpu_uint reference) override;
	virtual agpu_error executeBundle(const agpu::command_list_ref & bundle) override;
	virtual agpu_error beginRenderPass(const agpu::renderpass_ref & renderpass, const agpu::framebuffer_ref & framebuffer, agpu_bool bundle_content) override;
	virtual agpu_error endRenderPass() override;
	virtual agpu_error resolveFramebuffer(const agpu::framebuffer_ref & destFramebuffer, const agpu::framebuffer_ref & sourceFramebuffer) override;
	virtual agpu_error resolveTexture(const agpu::texture_ref & sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, const agpu::texture_ref & destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect) override;
	virtual agpu_error pushConstants(agpu_uint offset, agpu_uint size, agpu_pointer values) override;
    virtual agpu_error memoryBarrier(agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses) override;
    virtual agpu_error bufferMemoryBarrier(const agpu::buffer_ref & buffer, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_size offset, agpu_size size) override;
    virtual agpu_error textureMemoryBarrier(const agpu::texture_ref & texture, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_texture_usage_mode_mask old_usage, agpu_texture_usage_mode_mask new_usage, agpu_texture_subresource_range* subresource_range) override;
    virtual agpu_error pushBufferTransitionBarrier(const agpu::buffer_ref & buffer, agpu_buffer_usage_mask old_usage, agpu_buffer_usage_mask new_usage) override;
	virtual agpu_error pushTextureTransitionBarrier(const agpu::texture_ref & texture, agpu_texture_usage_mode_mask old_usage, agpu_texture_usage_mode_mask new_usage, agpu_texture_subresource_range* subresource_range) override;
    virtual agpu_error popBufferTransitionBarrier() override;
    virtual agpu_error popTextureTransitionBarrier() override;
    virtual agpu_error copyBuffer(const agpu::buffer_ref & source_buffer, agpu_size source_offset, const agpu::buffer_ref & dest_buffer, agpu_size dest_offset, agpu_size copy_size) override;
    virtual agpu_error copyBufferToTexture(const agpu::buffer_ref & buffer, const agpu::texture_ref & texture, agpu_buffer_image_copy_region* copy_region) override;
    virtual agpu_error copyTextureToBuffer(const agpu::texture_ref & texture, const agpu::buffer_ref & buffer, agpu_buffer_image_copy_region* copy_region) override;
	virtual agpu_error copyTexture(const agpu::texture_ref & source_texture, const agpu::texture_ref & dest_texture, agpu_image_copy_region* copy_region) override;

protected:
    void invalidateGraphicsPipelineState();
    agpu_error validateGraphicsPipelineState();

    void invalidateComputePipelineState();
    agpu_error validateComputePipelineState();

    virtual agpu_error setupCommandListForRecordingCommands() = 0;

    agpu::device_ref device;
    agpu::state_tracker_cache_ref cache;
    agpu_command_list_type commandListType;
    agpu::command_queue_ref commandQueue;
    agpu::command_allocator_ref currentCommandAllocator;
    agpu::command_list_ref currentCommandList;

    GraphicsPipelineStateDescription graphicsPipelineStateDescription;
    bool isGraphicsPipelineDescriptionChanged;

    ComputePipelineStateDescription computePipelineStateDescription;
    bool isComputePipelineDescriptionChanged;

    bool isRecording;

    std::string pipelineBuildErrorLog;
};

/**
 * I am an state tracker without frame buffering
 */
class DirectStateTracker : public AbstractStateTracker
{
public:
    DirectStateTracker(const agpu::state_tracker_cache_ref &cache,
            const agpu::device_ref &device,
            agpu_command_list_type type,
            const agpu::command_queue_ref &commandQueue,
            const agpu::command_allocator_ref &commandAllocator,
            bool isCommandAllocatorOwned);
    ~DirectStateTracker();

    static agpu::state_tracker_ref create(const agpu::state_tracker_cache_ref &cache,
            const agpu::device_ref &device,
            agpu_command_list_type type,
            const agpu::command_queue_ref &commandQueue,
            const agpu::command_allocator_ref &commandAllocator,
            bool isCommandAllocatorOwned);

    virtual agpu::command_list_ptr endRecordingCommands() override;

protected:
    virtual agpu_error setupCommandListForRecordingCommands() override;
    bool createCommandList();

    agpu::command_allocator_ref commandAllocator;
    agpu::command_list_ref commandList;
    bool isCommandAllocatorOwned;

};

/**
 * I am an state tracker with support for implicit frame buffering.
 */
class FrameBufferredStateTracker : public AbstractStateTracker
{
public:
    FrameBufferredStateTracker(const agpu::state_tracker_cache_ref &cache,
            const agpu::device_ref &device,
            agpu_command_list_type type,
            const agpu::command_queue_ref &commandQueue,
            agpu_uint frameBufferingCount);
    ~FrameBufferredStateTracker();

    static agpu::state_tracker_ref create(const agpu::state_tracker_cache_ref &cache,
            const agpu::device_ref &device,
            agpu_command_list_type type,
            const agpu::command_queue_ref &commandQueue,
            agpu_uint frameBufferingCount);

    virtual agpu::command_list_ptr endRecordingCommands() override;

protected:
    virtual agpu_error setupCommandListForRecordingCommands() override;

    bool createCommandAllocatorsAndCommandLists();

    agpu_uint frameBufferingCount;
    std::vector<agpu::command_allocator_ref> commandAllocators;
    std::vector<agpu::command_list_ref> commandLists;
};


}
#endif //AGPU_STATE_TRACKER_HPP
