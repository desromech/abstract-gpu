#ifndef AGPU_COMMAND_LIST_HPP_
#define AGPU_COMMAND_LIST_HPP_

#include <vector>
#include <functional>
#include "device.hpp"

namespace AgpuGL
{

typedef std::function<void()> AgpuGLCommand;

struct CommandListExecutionContext
{
    static constexpr unsigned int MaxNumberOfShaderResourceBindings = 16;

    CommandListExecutionContext();
    ~CommandListExecutionContext();

    void reset();
    void validateBeforeDrawCall();
    void validateBeforeComputeDispatch();
    void usePipelineState(const agpu::pipeline_state_ref &newPipeline);
    void useComputePipelineState(const agpu::pipeline_state_ref &newPipeline);

    void setStencilReference(agpu_uint reference);

	void useShaderResources(const agpu::shader_resource_binding_ref &binding );
	void useComputeShaderResources(const agpu::shader_resource_binding_ref &binding);
    void setBaseInstance(agpu_uint base_instance);

    agpu::device_ref device;
    agpu::pipeline_state_ref currentPipeline;
    agpu::pipeline_state_ref currentComputePipeline;
    agpu::pipeline_state_ref activePipeline;
    agpu::shader_resource_binding_ref shaderResourceBindings[MaxNumberOfShaderResourceBindings];
    agpu::shader_resource_binding_ref computeShaderResourceBindings[MaxNumberOfShaderResourceBindings];

    bool hasValidActivePipeline;
    bool hasValidShaderResources;
    bool hasValidComputeShaderResources;
    bool hasValidComputePushConstants;
    bool hasValidGraphicsPushConstants;
    GLenum primitiveMode;
    agpu_uint stencilReference;
    uint8_t pushConstantBuffer[128];
};

struct GLCommandList: public agpu::command_list
{
public:
    GLCommandList();
    ~GLCommandList();

    static agpu::command_list_ref create(const agpu::device_ref &device, agpu_command_list_type type, const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initial_pipeline_state);

    virtual agpu_error setShaderSignature(const agpu::shader_signature_ref &signature) override;
    virtual agpu_error setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h) override;
    virtual agpu_error setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h) override;
    virtual agpu_error usePipelineState(const agpu::pipeline_state_ref &pipeline) override;
    virtual agpu_error useVertexBinding(const agpu::vertex_binding_ref &vertex_binding) override;
    virtual agpu_error useIndexBuffer(const agpu::buffer_ref &index_buffer) override;
    virtual agpu_error useIndexBufferAt(const agpu::buffer_ref &index_buffer, agpu_size offset, agpu_size index_size) override;
    virtual agpu_error useDrawIndirectBuffer(const agpu::buffer_ref &draw_buffer) override;
    virtual agpu_error useComputeDispatchIndirectBuffer(const agpu::buffer_ref &draw_buffer) override;
    virtual agpu_error useShaderResources (const agpu::shader_resource_binding_ref &binding) override;
    virtual agpu_error useComputeShaderResources(const agpu::shader_resource_binding_ref &binding) override;
    virtual agpu_error pushConstants(agpu_uint offset, agpu_uint size, agpu_pointer values) override;
    virtual agpu_error drawArrays(agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance) override;
    virtual agpu_error drawArraysIndirect(agpu_size offset, agpu_size drawcount) override;
    virtual agpu_error drawElements(agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance) override;
    virtual agpu_error drawElementsIndirect(agpu_size offset, agpu_size drawcount) override;
    virtual agpu_error dispatchCompute(agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z) override;
    virtual agpu_error dispatchComputeIndirect(agpu_size offset) override;
    virtual agpu_error setStencilReference(agpu_uint reference) override;
    virtual agpu_error executeBundle(const agpu::command_list_ref &bundle) override;
    virtual agpu_error close() override;
    virtual agpu_error reset(const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initial_pipeline_state) override;
    virtual agpu_error resetBundle(const agpu::command_allocator_ref & allocator, const agpu::pipeline_state_ref & initial_pipeline_state, agpu_inheritance_info* inheritance_info) override;
    virtual agpu_error beginRenderPass(const agpu::renderpass_ref &renderpass, const agpu::framebuffer_ref &framebuffer, agpu_bool bundle_content) override;
    virtual agpu_error endRenderPass() override;
    virtual agpu_error resolveFramebuffer(const agpu::framebuffer_ref &destFramebuffer, const agpu::framebuffer_ref &sourceFramebuffer) override;
    virtual agpu_error resolveTexture(const agpu::texture_ref & sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, const agpu::texture_ref & destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect) override;

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

public:
    agpu::device_ref device;

    agpu::pipeline_state_ref currentPipeline;

    agpu::vertex_binding_ref currentVertexBinding;
    agpu::buffer_ref currentIndexBuffer;
    agpu_size currentIndexBufferOffset;
    agpu_size currentIndexBufferIndexSize;

    agpu::buffer_ref currentDrawBuffer;
    agpu::buffer_ref currentComputeDispatchBuffer;
    agpu_command_list_type type;

    void execute();

private:
    agpu_error addCommand(const AgpuGLCommand &command);

    std::vector<AgpuGLCommand> commands;
    bool closed;
    CommandListExecutionContext executionContext;
};

} // End of namespace AgpuGL

#endif //AGPU_COMMAND_LIST_HPP_
