#ifndef AGPU_METAL_COMMAND_LIST_HPP
#define AGPU_METAL_COMMAND_LIST_HPP

#include "device.hpp"

struct _agpu_command_list : public Object<_agpu_command_list>
{
public:
    static const size_t MaxActiveResourceBindings = 16;
    static const size_t MaxPushConstantBufferSize = 128;

    _agpu_command_list(agpu_device *device);
    void lostReferences();

    static agpu_command_list* create ( agpu_device* device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state );

    agpu_error setShaderSignature ( agpu_shader_signature* signature );
    agpu_error setViewport ( agpu_int x, agpu_int y, agpu_int w, agpu_int h );
    agpu_error setScissor ( agpu_int x, agpu_int y, agpu_int w, agpu_int h );
    agpu_error usePipelineState ( agpu_pipeline_state* pipeline );
    agpu_error useVertexBinding ( agpu_vertex_binding* vertex_binding );
    agpu_error useIndexBuffer ( agpu_buffer* index_buffer );
    agpu_error useDrawIndirectBuffer ( agpu_buffer* draw_buffer );
    agpu_error useComputeDispatchIndirectBuffer ( agpu_buffer* dispatch_buffer );
    agpu_error useShaderResources ( agpu_shader_resource_binding* binding );
    agpu_error useComputeShaderResources ( agpu_shader_resource_binding* binding );
    agpu_error drawArrays ( agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance );
    agpu_error drawArraysIndirect ( agpu_size offset, agpu_size drawcount );
    agpu_error drawElements ( agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance );
    agpu_error drawElementsIndirect ( agpu_size offset, agpu_size drawcount );
    agpu_error dispatchCompute ( agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z );
    agpu_error dispatchComputeIndirect ( agpu_size offset );
    agpu_error setStencilReference ( agpu_uint reference );
    agpu_error executeBundle ( agpu_command_list* bundle );
    agpu_error close (  );
    agpu_error reset ( agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state );
    agpu_error resetBundle ( agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state, agpu_inheritance_info* inheritance_info );
    agpu_error beginRenderPass ( agpu_renderpass* renderpass, agpu_framebuffer* framebuffer, agpu_bool bundle_content );
    agpu_error endRenderPass (  );
    agpu_error resolveFramebuffer ( agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer );
    agpu_error pushConstants ( agpu_uint offset, agpu_uint size, agpu_pointer values );

    void updateRenderState();
    void activateVertexBinding ();
    void activateShaderResourceBindings();
    void uploadPushConstants();

    void updateComputeState();
    void activateComputeShaderResourceBindings();
    void uploadComputePushConstants();

    agpu_device *device;
    agpu_command_list_type type;
    agpu_command_allocator* allocator;
    id<MTLCommandBuffer> buffer;
    id<MTLBlitCommandEncoder> blitEncoder;
    id<MTLRenderCommandEncoder> renderEncoder;
    id<MTLComputeCommandEncoder> computeEncoder;
    agpu_buffer *currentIndexBuffer;
    agpu_buffer *currentIndirectBuffer;
    agpu_buffer *currentComputeDispatchIndirectBuffer;
    agpu_vertex_binding* currentVertexBinding;

    agpu_pipeline_state *currentPipeline;
    agpu_shader_signature *currentShaderSignature;
    agpu_uint vertexBufferCount;
    agpu_bool used;
    agpu_shader_resource_binding *activeShaderResourceBindings[MaxActiveResourceBindings];
    agpu_shader_resource_binding *activeComputeShaderResourceBindings[MaxActiveResourceBindings];

    bool pushConstantsModified;
    uint8_t pushConstantsBuffer[MaxPushConstantBufferSize];
    
    bool computePushConstantsModified;
    uint8_t computePushConstantsBuffer[MaxPushConstantBufferSize];
};

#endif //AGPU_METAL_COMMAND_LIST_HPP
