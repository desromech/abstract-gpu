#include "device.hpp"

/* Methods for interface agpu_swap_chain. */
AGPU_EXPORT agpu_error agpuAddSwapChainReference ( agpu_swap_chain* swap_chain )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseSwapChain ( agpu_swap_chain* swap_chain )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSwapBuffers ( agpu_swap_chain* swap_chain )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_framebuffer* agpuGetCurrentBackBuffer ( agpu_swap_chain* swap_chain )
{
    return nullptr;
}

/* Methods for interface agpu_pipeline_builder. */
AGPU_EXPORT agpu_error agpuAddPipelineBuilderReference ( agpu_pipeline_builder* pipeline_builder )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleasePipelineBuilder ( agpu_pipeline_builder* pipeline_builder )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_pipeline_state* agpuBuildPipelineState ( agpu_pipeline_builder* pipeline_builder )
{
    return nullptr;
}

AGPU_EXPORT agpu_error agpuAttachShader ( agpu_pipeline_builder* pipeline_builder, agpu_shader* shader )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_size agpuGetPipelineBuildingLogLength ( agpu_pipeline_builder* pipeline_builder )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuGetPipelineBuildingLog ( agpu_pipeline_builder* pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetBlendState ( agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool enabled )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetBlendFunction ( agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetColorMask ( agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetDepthState ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetStencilState ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_int writeMask, agpu_int readMask )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetStencilFrontFace ( agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetStencilBackFace ( agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetRenderTargetCount ( agpu_pipeline_builder* pipeline_builder, agpu_int count )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetRenderTargetFormat ( agpu_pipeline_builder* pipeline_builder, agpu_uint index, agpu_texture_format format )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetDepthStencilFormat ( agpu_pipeline_builder* pipeline_builder, agpu_texture_format format )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetPrimitiveType ( agpu_pipeline_builder* pipeline_builder, agpu_primitive_topology type )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetVertexLayout ( agpu_pipeline_builder* pipeline_builder, agpu_vertex_layout* layout )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetPipelineShaderSignature ( agpu_pipeline_builder* pipeline_builder, agpu_shader_signature* signature )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetSampleDescription ( agpu_pipeline_builder* pipeline_builder, agpu_uint sample_count, agpu_uint sample_quality )
{
    return AGPU_UNIMPLEMENTED;
}


/* Methods for interface agpu_pipeline_state. */
AGPU_EXPORT agpu_error agpuAddPipelineStateReference ( agpu_pipeline_state* pipeline_state )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleasePipelineState ( agpu_pipeline_state* pipeline_state )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_int agpuGetUniformLocation ( agpu_pipeline_state* pipeline_state, agpu_cstring name )
{
    return AGPU_UNIMPLEMENTED;
}


/* Methods for interface agpu_command_queue. */
AGPU_EXPORT agpu_error agpuAddCommandQueueReference ( agpu_command_queue* command_queue )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseCommandQueue ( agpu_command_queue* command_queue )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuAddCommandList ( agpu_command_queue* command_queue, agpu_command_list* command_list )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuFinishQueueExecution ( agpu_command_queue* command_queue )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSignalFence ( agpu_command_queue* command_queue, agpu_fence* fence )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuWaitFence ( agpu_command_queue* command_queue, agpu_fence* fence )
{
    return AGPU_UNIMPLEMENTED;
}


/* Methods for interface agpu_command_allocator. */
AGPU_EXPORT agpu_error agpuAddCommandAllocatorReference ( agpu_command_allocator* command_allocator )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseCommandAllocator ( agpu_command_allocator* command_allocator )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuResetCommandAllocator ( agpu_command_allocator* command_allocator )
{
    return AGPU_UNIMPLEMENTED;
}


/* Methods for interface agpu_command_list. */
AGPU_EXPORT agpu_error agpuAddCommandListReference ( agpu_command_list* command_list )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseCommandList ( agpu_command_list* command_list )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetShaderSignature ( agpu_command_list* command_list, agpu_shader_signature* signature )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetViewport ( agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetScissor ( agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetClearColor ( agpu_command_list* command_list, agpu_float r, agpu_float g, agpu_float b, agpu_float a )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetClearDepth ( agpu_command_list* command_list, agpu_float depth )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetClearStencil ( agpu_command_list* command_list, agpu_int value )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuClear ( agpu_command_list* command_list, agpu_bitfield buffers )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuUsePipelineState ( agpu_command_list* command_list, agpu_pipeline_state* pipeline )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuUseVertexBinding ( agpu_command_list* command_list, agpu_vertex_binding* vertex_binding )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuUseIndexBuffer ( agpu_command_list* command_list, agpu_buffer* index_buffer )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetPrimitiveTopology ( agpu_command_list* command_list, agpu_primitive_topology topology )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuUseDrawIndirectBuffer ( agpu_command_list* command_list, agpu_buffer* draw_buffer )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuUseShaderResources ( agpu_command_list* command_list, agpu_shader_resource_binding* binding )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuDrawArrays ( agpu_command_list* command_list, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuDrawElements ( agpu_command_list* command_list, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuDrawElementsIndirect ( agpu_command_list* command_list, agpu_size offset )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuMultiDrawElementsIndirect ( agpu_command_list* command_list, agpu_size offset, agpu_size drawcount )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetStencilReference ( agpu_command_list* command_list, agpu_uint reference )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuExecuteBundle ( agpu_command_list* command_list, agpu_command_list* bundle )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuCloseCommandList ( agpu_command_list* command_list )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuResetCommandList ( agpu_command_list* command_list, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuBeginFrame ( agpu_command_list* command_list, agpu_framebuffer* framebuffer, agpu_bool bundle_content )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuEndFrame ( agpu_command_list* command_list )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuResolveFramebuffer ( agpu_command_list* command_list, agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer )
{
    return AGPU_UNIMPLEMENTED;
}


/* Methods for interface agpu_texture. */
AGPU_EXPORT agpu_error agpuAddTextureReference ( agpu_texture* texture )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseTexture ( agpu_texture* texture )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuGetTextureDescription ( agpu_texture* texture, agpu_texture_description* description )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_pointer agpuMapTextureLevel ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags )
{
    return nullptr;
}

AGPU_EXPORT agpu_error agpuUnmapTextureLevel ( agpu_texture* texture )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReadTextureData ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuUploadTextureData ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuDiscardTextureUploadBuffer ( agpu_texture* texture )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuDiscardTextureReadbackBuffer ( agpu_texture* texture )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuGetTextureFullViewDescription ( agpu_texture* texture, agpu_texture_view_description* result )
{
    return AGPU_UNIMPLEMENTED;
}


/* Methods for interface agpu_buffer. */
AGPU_EXPORT agpu_error agpuAddBufferReference ( agpu_buffer* buffer )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseBuffer ( agpu_buffer* buffer )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_pointer agpuMapBuffer ( agpu_buffer* buffer, agpu_mapping_access flags )
{
    return nullptr;
}

AGPU_EXPORT agpu_error agpuUnmapBuffer ( agpu_buffer* buffer )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuGetBufferDescription ( agpu_buffer* buffer, agpu_buffer_description* description )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuUploadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data )
{
    return AGPU_UNIMPLEMENTED;
}


/* Methods for interface agpu_vertex_binding. */
AGPU_EXPORT agpu_error agpuAddVertexBindingReference ( agpu_vertex_binding* vertex_binding )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseVertexBinding ( agpu_vertex_binding* vertex_binding )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuBindVertexBuffers ( agpu_vertex_binding* vertex_binding, agpu_uint count, agpu_buffer** vertex_buffers )
{
    return AGPU_UNIMPLEMENTED;
}


/* Methods for interface agpu_vertex_layout. */

AGPU_EXPORT agpu_error agpuAddVertexLayoutReference ( agpu_vertex_layout* vertex_layout )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseVertexLayout ( agpu_vertex_layout* vertex_layout )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuAddVertexAttributeBindings ( agpu_vertex_layout* vertex_layout, agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes )
{
    return AGPU_UNIMPLEMENTED;
}


/* Methods for interface agpu_shader. */

AGPU_EXPORT agpu_error agpuAddShaderReference ( agpu_shader* shader )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseShader ( agpu_shader* shader )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuSetShaderSource ( agpu_shader* shader, agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuCompileShader ( agpu_shader* shader, agpu_cstring options )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_size agpuGetShaderCompilationLogLength ( agpu_shader* shader )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuGetShaderCompilationLog ( agpu_shader* shader, agpu_size buffer_size, agpu_string_buffer buffer )
{
    return AGPU_UNIMPLEMENTED;
}


/* Methods for interface agpu_framebuffer. */

AGPU_EXPORT agpu_error agpuAddFramebufferReference ( agpu_framebuffer* framebuffer )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseFramebuffer ( agpu_framebuffer* framebuffer )
{
    return AGPU_UNIMPLEMENTED;
}


/* Methods for interface agpu_shader_signature_builder. */

AGPU_EXPORT agpu_error agpuAddShaderSignatureBuilderReference ( agpu_shader_signature_builder* shader_signature_builder )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseShaderSignatureBuilder ( agpu_shader_signature_builder* shader_signature_builder )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_shader_signature* agpuBuildShaderSignature ( agpu_shader_signature_builder* shader_signature_builder )
{
    return nullptr;
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingConstant ( agpu_shader_signature_builder* shader_signature_builder )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingElement ( agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint maxBindings )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingBank ( agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint bindingPointCount, agpu_uint maxBindings )
{
    return AGPU_UNIMPLEMENTED;
}


/* Methods for interface agpu_shader_signature. */

AGPU_EXPORT agpu_error agpuAddShaderSignature ( agpu_shader_signature* shader_signature )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseShaderSignature ( agpu_shader_signature* shader_signature )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_shader_resource_binding* agpuCreateShaderResourceBinding ( agpu_shader_signature* shader_signature, agpu_uint element )
{
    return nullptr;
}


/* Methods for interface agpu_shader_resource_binding. */

AGPU_EXPORT agpu_error agpuAddShaderResourceBindingReference ( agpu_shader_resource_binding* shader_resource_binding )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseShaderResourceBinding ( agpu_shader_resource_binding* shader_resource_binding )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuBindUniformBuffer ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuBindUniformBufferRange ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuBindTexture ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuBindTextureArrayRange ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuCreateSampler ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_sampler_description* description )
{
    return AGPU_UNIMPLEMENTED;
}


/* Methods for interface agpu_fence. */

AGPU_EXPORT agpu_error agpuAddFenceReference ( agpu_fence* fence )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseFenceReference ( agpu_fence* fence )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuWaitOnClient ( agpu_fence* fence )
{
    return AGPU_UNIMPLEMENTED;
}
