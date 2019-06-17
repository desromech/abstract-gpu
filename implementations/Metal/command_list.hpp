#ifndef AGPU_METAL_COMMAND_LIST_HPP
#define AGPU_METAL_COMMAND_LIST_HPP

#include "device.hpp"

namespace AgpuMetal
{

class AMtlCommandList : public agpu::command_list
{
public:
    static const size_t MaxActiveResourceBindings = 16;
    static const size_t MaxPushConstantBufferSize = 128;

    AMtlCommandList(const agpu::device_ref &device);
    ~AMtlCommandList();

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
    virtual agpu_error resetBundle(const agpu::command_allocator_ref &allocator, const agpu::pipeline_state_ref &initial_pipeline_state, agpu_inheritance_info* inheritance_info) override;
    virtual agpu_error beginRenderPass(const agpu::renderpass_ref &renderpass, const agpu::framebuffer_ref &framebuffer, agpu_bool bundle_content) override;
    virtual agpu_error endRenderPass() override;
    virtual agpu_error resolveFramebuffer(const agpu::framebuffer_ref &destFramebuffer, const agpu::framebuffer_ref &sourceFramebuffer) override;
    virtual agpu_error resolveTexture(const agpu::texture_ref &sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, const agpu::texture_ref &destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect) override;
    virtual agpu_error pushConstants(agpu_uint offset, agpu_uint size, agpu_pointer values) override;

    virtual agpu_error memoryBarrier(agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses) override;

    void updateRenderState();
    void activateVertexBinding ();
    void activateShaderResourceBindings();
    void uploadPushConstants();

    void updateComputeState();
    void activateComputeShaderResourceBindings();
    void uploadComputePushConstants();

    agpu::device_ref device;
    agpu_command_list_type type;
    agpu::command_allocator_ref allocator;
    id<MTLCommandBuffer> buffer;
    id<MTLBlitCommandEncoder> blitEncoder;
    id<MTLRenderCommandEncoder> renderEncoder;
    id<MTLComputeCommandEncoder> computeEncoder;
    agpu::buffer_ref currentIndexBuffer;
    agpu_size currentIndexBufferOffset;
    agpu_size currentIndexBufferStride;
    agpu::buffer_ref currentIndirectBuffer;
    agpu::buffer_ref currentComputeDispatchIndirectBuffer;
    agpu::vertex_binding_ref currentVertexBinding;

    agpu::pipeline_state_ref currentPipeline;
    agpu::shader_signature_ref currentShaderSignature;
    agpu_uint vertexBufferCount;
    agpu_bool used;
    agpu::shader_resource_binding_ref activeShaderResourceBindings[MaxActiveResourceBindings];
    agpu::shader_resource_binding_ref activeComputeShaderResourceBindings[MaxActiveResourceBindings];

    bool pushConstantsModified;
    uint8_t pushConstantsBuffer[MaxPushConstantBufferSize];

    bool computePushConstantsModified;
    uint8_t computePushConstantsBuffer[MaxPushConstantBufferSize];
};

} // End of namespace AgpuMetal

#endif //AGPU_METAL_COMMAND_LIST_HPP
