// This file was generated automatically. DO NOT MODIFY
#include <AGPU/agpu.h>

AGPU_EXPORT agpu_device* agpuOpenDevice ( agpu_platform* platform, agpu_device_open_info* openInfo )
{
	if (platform == nullptr)
		return (agpu_device*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (platform);
	return (*dispatchTable)->agpuOpenDevice ( platform, openInfo );
}

AGPU_EXPORT agpu_cstring agpuGetPlatformName ( agpu_platform* platform )
{
	if (platform == nullptr)
		return (agpu_cstring)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (platform);
	return (*dispatchTable)->agpuGetPlatformName ( platform );
}

AGPU_EXPORT agpu_int agpuGetPlatformVersion ( agpu_platform* platform )
{
	if (platform == nullptr)
		return (agpu_int)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (platform);
	return (*dispatchTable)->agpuGetPlatformVersion ( platform );
}

AGPU_EXPORT agpu_int agpuGetPlatformImplementationVersion ( agpu_platform* platform )
{
	if (platform == nullptr)
		return (agpu_int)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (platform);
	return (*dispatchTable)->agpuGetPlatformImplementationVersion ( platform );
}

AGPU_EXPORT agpu_bool agpuPlatformHasRealMultithreading ( agpu_platform* platform )
{
	if (platform == nullptr)
		return (agpu_bool)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (platform);
	return (*dispatchTable)->agpuPlatformHasRealMultithreading ( platform );
}

AGPU_EXPORT agpu_bool agpuIsNativePlatform ( agpu_platform* platform )
{
	if (platform == nullptr)
		return (agpu_bool)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (platform);
	return (*dispatchTable)->agpuIsNativePlatform ( platform );
}

AGPU_EXPORT agpu_error agpuAddDeviceReference ( agpu_device* device )
{
	if (device == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuAddDeviceReference ( device );
}

AGPU_EXPORT agpu_error agpuReleaseDevice ( agpu_device* device )
{
	if (device == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuReleaseDevice ( device );
}

AGPU_EXPORT agpu_command_queue* agpuGetDefaultCommandQueue ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_command_queue*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuGetDefaultCommandQueue ( device );
}

AGPU_EXPORT agpu_swap_chain* agpuCreateSwapChain ( agpu_device* device, agpu_swap_chain_create_info* swapChainInfo )
{
	if (device == nullptr)
		return (agpu_swap_chain*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateSwapChain ( device, swapChainInfo );
}

AGPU_EXPORT agpu_buffer* agpuCreateBuffer ( agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data )
{
	if (device == nullptr)
		return (agpu_buffer*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateBuffer ( device, description, initial_data );
}

AGPU_EXPORT agpu_vertex_layout* agpuCreateVertexLayout ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_vertex_layout*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateVertexLayout ( device );
}

AGPU_EXPORT agpu_vertex_binding* agpuCreateVertexBinding ( agpu_device* device, agpu_vertex_layout* layout )
{
	if (device == nullptr)
		return (agpu_vertex_binding*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateVertexBinding ( device, layout );
}

AGPU_EXPORT agpu_shader* agpuCreateShader ( agpu_device* device, agpu_shader_type type )
{
	if (device == nullptr)
		return (agpu_shader*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateShader ( device, type );
}

AGPU_EXPORT agpu_shader_resource_binding* agpuCreateShaderResourceBinding ( agpu_device* device, agpu_int bindingBank )
{
	if (device == nullptr)
		return (agpu_shader_resource_binding*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateShaderResourceBinding ( device, bindingBank );
}

AGPU_EXPORT agpu_pipeline_builder* agpuCreatePipelineBuilder ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_pipeline_builder*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreatePipelineBuilder ( device );
}

AGPU_EXPORT agpu_command_allocator* agpuCreateCommandAllocator ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_command_allocator*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateCommandAllocator ( device );
}

AGPU_EXPORT agpu_command_list* agpuCreateCommandListBundle ( agpu_device* device, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
{
	if (device == nullptr)
		return (agpu_command_list*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateCommandListBundle ( device, allocator, initial_pipeline_state );
}

AGPU_EXPORT agpu_command_list* agpuCreateCommandList ( agpu_device* device, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
{
	if (device == nullptr)
		return (agpu_command_list*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateCommandList ( device, allocator, initial_pipeline_state );
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredShaderLanguage ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_shader_language)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuGetPreferredShaderLanguage ( device );
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredHighLevelShaderLanguage ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_shader_language)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuGetPreferredHighLevelShaderLanguage ( device );
}

AGPU_EXPORT agpu_framebuffer* agpuCreateFrameBuffer ( agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint renderTargetCount, agpu_bool hasDepth, agpu_bool hasStencil )
{
	if (device == nullptr)
		return (agpu_framebuffer*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateFrameBuffer ( device, width, height, renderTargetCount, hasDepth, hasStencil );
}

AGPU_EXPORT agpu_texture* agpuCreateTexture ( agpu_device* device, agpu_texture_description* description, agpu_pointer initialData )
{
	if (device == nullptr)
		return (agpu_texture*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateTexture ( device, description, initialData );
}

AGPU_EXPORT agpu_error agpuAddSwapChainReference ( agpu_swap_chain* swap_chain )
{
	if (swap_chain == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (swap_chain);
	return (*dispatchTable)->agpuAddSwapChainReference ( swap_chain );
}

AGPU_EXPORT agpu_error agpuReleaseSwapChain ( agpu_swap_chain* swap_chain )
{
	if (swap_chain == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (swap_chain);
	return (*dispatchTable)->agpuReleaseSwapChain ( swap_chain );
}

AGPU_EXPORT agpu_error agpuSwapBuffers ( agpu_swap_chain* swap_chain )
{
	if (swap_chain == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (swap_chain);
	return (*dispatchTable)->agpuSwapBuffers ( swap_chain );
}

AGPU_EXPORT agpu_framebuffer* agpuGetCurrentBackBuffer ( agpu_swap_chain* swap_chain )
{
	if (swap_chain == nullptr)
		return (agpu_framebuffer*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (swap_chain);
	return (*dispatchTable)->agpuGetCurrentBackBuffer ( swap_chain );
}

AGPU_EXPORT agpu_error agpuAddPipelineBuilderReference ( agpu_pipeline_builder* pipeline_builder )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuAddPipelineBuilderReference ( pipeline_builder );
}

AGPU_EXPORT agpu_error agpuReleasePipelineBuilder ( agpu_pipeline_builder* pipeline_builder )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuReleasePipelineBuilder ( pipeline_builder );
}

AGPU_EXPORT agpu_pipeline_state* agpuBuildPipelineState ( agpu_pipeline_builder* pipeline_builder )
{
	if (pipeline_builder == nullptr)
		return (agpu_pipeline_state*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuBuildPipelineState ( pipeline_builder );
}

AGPU_EXPORT agpu_error agpuAttachShader ( agpu_pipeline_builder* pipeline_builder, agpu_shader* shader )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuAttachShader ( pipeline_builder, shader );
}

AGPU_EXPORT agpu_size agpuGetPipelineBuildingLogLength ( agpu_pipeline_builder* pipeline_builder )
{
	if (pipeline_builder == nullptr)
		return (agpu_size)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuGetPipelineBuildingLogLength ( pipeline_builder );
}

AGPU_EXPORT agpu_error agpuGetPipelineBuildingLog ( agpu_pipeline_builder* pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuGetPipelineBuildingLog ( pipeline_builder, buffer_size, buffer );
}

AGPU_EXPORT agpu_error agpuSetDepthState ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetDepthState ( pipeline_builder, enabled, writeMask, function );
}

AGPU_EXPORT agpu_error agpuSetStencilState ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_int writeMask, agpu_int readMask )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetStencilState ( pipeline_builder, enabled, writeMask, readMask );
}

AGPU_EXPORT agpu_error agpuSetRenderTargetCount ( agpu_pipeline_builder* pipeline_builder, agpu_int count )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetRenderTargetCount ( pipeline_builder, count );
}

AGPU_EXPORT agpu_error agpuSetPrimitiveType ( agpu_pipeline_builder* pipeline_builder, agpu_primitive_type type )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetPrimitiveType ( pipeline_builder, type );
}

AGPU_EXPORT agpu_error agpuSetVertexLayout ( agpu_pipeline_builder* pipeline_builder, agpu_vertex_layout* layout )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetVertexLayout ( pipeline_builder, layout );
}

AGPU_EXPORT agpu_error agpuAddPipelineStateReference ( agpu_pipeline_state* pipeline_state )
{
	if (pipeline_state == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_state);
	return (*dispatchTable)->agpuAddPipelineStateReference ( pipeline_state );
}

AGPU_EXPORT agpu_error agpuReleasePipelineState ( agpu_pipeline_state* pipeline_state )
{
	if (pipeline_state == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_state);
	return (*dispatchTable)->agpuReleasePipelineState ( pipeline_state );
}

AGPU_EXPORT agpu_int agpuGetUniformLocation ( agpu_pipeline_state* pipeline_state, agpu_cstring name )
{
	if (pipeline_state == nullptr)
		return (agpu_int)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_state);
	return (*dispatchTable)->agpuGetUniformLocation ( pipeline_state, name );
}

AGPU_EXPORT agpu_error agpuAddCommandQueueReference ( agpu_command_queue* command_queue )
{
	if (command_queue == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_queue);
	return (*dispatchTable)->agpuAddCommandQueueReference ( command_queue );
}

AGPU_EXPORT agpu_error agpuReleaseCommandQueue ( agpu_command_queue* command_queue )
{
	if (command_queue == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_queue);
	return (*dispatchTable)->agpuReleaseCommandQueue ( command_queue );
}

AGPU_EXPORT agpu_error agpuAddCommandList ( agpu_command_queue* command_queue, agpu_command_list* command_list )
{
	if (command_queue == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_queue);
	return (*dispatchTable)->agpuAddCommandList ( command_queue, command_list );
}

AGPU_EXPORT agpu_error agpuFinishQueueExecution ( agpu_command_queue* command_queue )
{
	if (command_queue == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_queue);
	return (*dispatchTable)->agpuFinishQueueExecution ( command_queue );
}

AGPU_EXPORT agpu_error agpuAddCommandAllocatorReference ( agpu_command_allocator* command_allocator )
{
	if (command_allocator == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_allocator);
	return (*dispatchTable)->agpuAddCommandAllocatorReference ( command_allocator );
}

AGPU_EXPORT agpu_error agpuReleaseCommandAllocator ( agpu_command_allocator* command_allocator )
{
	if (command_allocator == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_allocator);
	return (*dispatchTable)->agpuReleaseCommandAllocator ( command_allocator );
}

AGPU_EXPORT agpu_error agpuResetCommandAllocator ( agpu_command_allocator* command_allocator )
{
	if (command_allocator == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_allocator);
	return (*dispatchTable)->agpuResetCommandAllocator ( command_allocator );
}

AGPU_EXPORT agpu_error agpuAddCommandListReference ( agpu_command_list* command_list )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuAddCommandListReference ( command_list );
}

AGPU_EXPORT agpu_error agpuReleaseCommandList ( agpu_command_list* command_list )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuReleaseCommandList ( command_list );
}

AGPU_EXPORT agpu_error agpuSetViewport ( agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuSetViewport ( command_list, x, y, w, h );
}

AGPU_EXPORT agpu_error agpuSetScissor ( agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuSetScissor ( command_list, x, y, w, h );
}

AGPU_EXPORT agpu_error agpuSetClearColor ( agpu_command_list* command_list, agpu_float r, agpu_float g, agpu_float b, agpu_float a )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuSetClearColor ( command_list, r, g, b, a );
}

AGPU_EXPORT agpu_error agpuSetClearDepth ( agpu_command_list* command_list, agpu_float depth )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuSetClearDepth ( command_list, depth );
}

AGPU_EXPORT agpu_error agpuSetClearStencil ( agpu_command_list* command_list, agpu_int value )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuSetClearStencil ( command_list, value );
}

AGPU_EXPORT agpu_error agpuClear ( agpu_command_list* command_list, agpu_bitfield buffers )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuClear ( command_list, buffers );
}

AGPU_EXPORT agpu_error agpuUsePipelineState ( agpu_command_list* command_list, agpu_pipeline_state* pipeline )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuUsePipelineState ( command_list, pipeline );
}

AGPU_EXPORT agpu_error agpuUseVertexBinding ( agpu_command_list* command_list, agpu_vertex_binding* vertex_binding )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuUseVertexBinding ( command_list, vertex_binding );
}

AGPU_EXPORT agpu_error agpuUseIndexBuffer ( agpu_command_list* command_list, agpu_buffer* index_buffer )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuUseIndexBuffer ( command_list, index_buffer );
}

AGPU_EXPORT agpu_error agpuSetPrimitiveTopology ( agpu_command_list* command_list, agpu_primitive_topology topology )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuSetPrimitiveTopology ( command_list, topology );
}

AGPU_EXPORT agpu_error agpuUseDrawIndirectBuffer ( agpu_command_list* command_list, agpu_buffer* draw_buffer )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuUseDrawIndirectBuffer ( command_list, draw_buffer );
}

AGPU_EXPORT agpu_error agpuUseShaderResources ( agpu_command_list* command_list, agpu_shader_resource_binding* binding )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuUseShaderResources ( command_list, binding );
}

AGPU_EXPORT agpu_error agpuDrawArrays ( agpu_command_list* command_list, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuDrawArrays ( command_list, vertex_count, instance_count, first_vertex, base_instance );
}

AGPU_EXPORT agpu_error agpuDrawElements ( agpu_command_list* command_list, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuDrawElements ( command_list, index_count, instance_count, first_index, base_vertex, base_instance );
}

AGPU_EXPORT agpu_error agpuDrawElementsIndirect ( agpu_command_list* command_list, agpu_size offset )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuDrawElementsIndirect ( command_list, offset );
}

AGPU_EXPORT agpu_error agpuMultiDrawElementsIndirect ( agpu_command_list* command_list, agpu_size offset, agpu_size drawcount )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuMultiDrawElementsIndirect ( command_list, offset, drawcount );
}

AGPU_EXPORT agpu_error agpuSetStencilReference ( agpu_command_list* command_list, agpu_float reference )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuSetStencilReference ( command_list, reference );
}

AGPU_EXPORT agpu_error agpuSetAlphaReference ( agpu_command_list* command_list, agpu_float reference )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuSetAlphaReference ( command_list, reference );
}

AGPU_EXPORT agpu_error agpuExecuteBundle ( agpu_command_list* command_list, agpu_command_list* bundle )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuExecuteBundle ( command_list, bundle );
}

AGPU_EXPORT agpu_error agpuCloseCommandList ( agpu_command_list* command_list )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuCloseCommandList ( command_list );
}

AGPU_EXPORT agpu_error agpuResetCommandList ( agpu_command_list* command_list, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuResetCommandList ( command_list, allocator, initial_pipeline_state );
}

AGPU_EXPORT agpu_error agpuBeginFrame ( agpu_command_list* command_list, agpu_framebuffer* framebuffer )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuBeginFrame ( command_list, framebuffer );
}

AGPU_EXPORT agpu_error agpuEndFrame ( agpu_command_list* command_list )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuEndFrame ( command_list );
}

AGPU_EXPORT agpu_error agpuAddTextureReference ( agpu_texture* texture )
{
	if (texture == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture);
	return (*dispatchTable)->agpuAddTextureReference ( texture );
}

AGPU_EXPORT agpu_error agpuReleaseTexture ( agpu_texture* texture )
{
	if (texture == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture);
	return (*dispatchTable)->agpuReleaseTexture ( texture );
}

AGPU_EXPORT agpu_error agpuGetTextureDescription ( agpu_texture* texture, agpu_texture_description* description )
{
	if (texture == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture);
	return (*dispatchTable)->agpuGetTextureDescription ( texture, description );
}

AGPU_EXPORT agpu_pointer agpuMapTextureLevel ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags )
{
	if (texture == nullptr)
		return (agpu_pointer)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture);
	return (*dispatchTable)->agpuMapTextureLevel ( texture, level, arrayIndex, flags );
}

AGPU_EXPORT agpu_error agpuUnmapTextureLevel ( agpu_texture* texture )
{
	if (texture == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture);
	return (*dispatchTable)->agpuUnmapTextureLevel ( texture );
}

AGPU_EXPORT agpu_error agpuReadTextureData ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data )
{
	if (texture == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture);
	return (*dispatchTable)->agpuReadTextureData ( texture, level, arrayIndex, pitch, slicePitch, data );
}

AGPU_EXPORT agpu_error agpuUploadTextureData ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data )
{
	if (texture == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture);
	return (*dispatchTable)->agpuUploadTextureData ( texture, level, arrayIndex, pitch, slicePitch, data );
}

AGPU_EXPORT agpu_error agpuAddBufferReference ( agpu_buffer* buffer )
{
	if (buffer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (buffer);
	return (*dispatchTable)->agpuAddBufferReference ( buffer );
}

AGPU_EXPORT agpu_error agpuReleaseBuffer ( agpu_buffer* buffer )
{
	if (buffer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (buffer);
	return (*dispatchTable)->agpuReleaseBuffer ( buffer );
}

AGPU_EXPORT agpu_pointer agpuMapBuffer ( agpu_buffer* buffer, agpu_mapping_access flags )
{
	if (buffer == nullptr)
		return (agpu_pointer)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (buffer);
	return (*dispatchTable)->agpuMapBuffer ( buffer, flags );
}

AGPU_EXPORT agpu_error agpuUnmapBuffer ( agpu_buffer* buffer )
{
	if (buffer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (buffer);
	return (*dispatchTable)->agpuUnmapBuffer ( buffer );
}

AGPU_EXPORT agpu_error agpuGetBufferDescription ( agpu_buffer* buffer, agpu_buffer_description* description )
{
	if (buffer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (buffer);
	return (*dispatchTable)->agpuGetBufferDescription ( buffer, description );
}

AGPU_EXPORT agpu_error agpuUploadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data )
{
	if (buffer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (buffer);
	return (*dispatchTable)->agpuUploadBufferData ( buffer, offset, size, data );
}

AGPU_EXPORT agpu_error agpuReadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data )
{
	if (buffer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (buffer);
	return (*dispatchTable)->agpuReadBufferData ( buffer, offset, size, data );
}

AGPU_EXPORT agpu_error agpuAddVertexBindingReference ( agpu_vertex_binding* vertex_binding )
{
	if (vertex_binding == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vertex_binding);
	return (*dispatchTable)->agpuAddVertexBindingReference ( vertex_binding );
}

AGPU_EXPORT agpu_error agpuReleaseVertexBinding ( agpu_vertex_binding* vertex_binding )
{
	if (vertex_binding == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vertex_binding);
	return (*dispatchTable)->agpuReleaseVertexBinding ( vertex_binding );
}

AGPU_EXPORT agpu_error agpuBindVertexBuffers ( agpu_vertex_binding* vertex_binding, agpu_uint count, agpu_buffer** vertex_buffers )
{
	if (vertex_binding == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vertex_binding);
	return (*dispatchTable)->agpuBindVertexBuffers ( vertex_binding, count, vertex_buffers );
}

AGPU_EXPORT agpu_error agpuAddVertexLayoutReference ( agpu_vertex_layout* vertex_layout )
{
	if (vertex_layout == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vertex_layout);
	return (*dispatchTable)->agpuAddVertexLayoutReference ( vertex_layout );
}

AGPU_EXPORT agpu_error agpuReleaseVertexLayout ( agpu_vertex_layout* vertex_layout )
{
	if (vertex_layout == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vertex_layout);
	return (*dispatchTable)->agpuReleaseVertexLayout ( vertex_layout );
}

AGPU_EXPORT agpu_error agpuAddVertexAttributeBindings ( agpu_vertex_layout* vertex_layout, agpu_uint vertex_buffer_count, agpu_size attribute_count, agpu_vertex_attrib_description* attributes )
{
	if (vertex_layout == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vertex_layout);
	return (*dispatchTable)->agpuAddVertexAttributeBindings ( vertex_layout, vertex_buffer_count, attribute_count, attributes );
}

AGPU_EXPORT agpu_error agpuAddShaderReference ( agpu_shader* shader )
{
	if (shader == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader);
	return (*dispatchTable)->agpuAddShaderReference ( shader );
}

AGPU_EXPORT agpu_error agpuReleaseShader ( agpu_shader* shader )
{
	if (shader == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader);
	return (*dispatchTable)->agpuReleaseShader ( shader );
}

AGPU_EXPORT agpu_error agpuSetShaderSource ( agpu_shader* shader, agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength )
{
	if (shader == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader);
	return (*dispatchTable)->agpuSetShaderSource ( shader, language, sourceText, sourceTextLength );
}

AGPU_EXPORT agpu_error agpuCompileShader ( agpu_shader* shader, agpu_cstring options )
{
	if (shader == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader);
	return (*dispatchTable)->agpuCompileShader ( shader, options );
}

AGPU_EXPORT agpu_size agpuGetShaderCompilationLogLength ( agpu_shader* shader )
{
	if (shader == nullptr)
		return (agpu_size)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader);
	return (*dispatchTable)->agpuGetShaderCompilationLogLength ( shader );
}

AGPU_EXPORT agpu_error agpuGetShaderCompilationLog ( agpu_shader* shader, agpu_size buffer_size, agpu_string_buffer buffer )
{
	if (shader == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader);
	return (*dispatchTable)->agpuGetShaderCompilationLog ( shader, buffer_size, buffer );
}

AGPU_EXPORT agpu_error agpuBindAttributeLocation ( agpu_shader* shader, agpu_cstring name, agpu_int location )
{
	if (shader == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader);
	return (*dispatchTable)->agpuBindAttributeLocation ( shader, name, location );
}

AGPU_EXPORT agpu_error agpuAddFramebufferReference ( agpu_framebuffer* framebuffer )
{
	if (framebuffer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (framebuffer);
	return (*dispatchTable)->agpuAddFramebufferReference ( framebuffer );
}

AGPU_EXPORT agpu_error agpuReleaseFramebuffer ( agpu_framebuffer* framebuffer )
{
	if (framebuffer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (framebuffer);
	return (*dispatchTable)->agpuReleaseFramebuffer ( framebuffer );
}

AGPU_EXPORT agpu_error agpuAttachColorBuffer ( agpu_framebuffer* framebuffer, agpu_int index, agpu_texture* buffer )
{
	if (framebuffer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (framebuffer);
	return (*dispatchTable)->agpuAttachColorBuffer ( framebuffer, index, buffer );
}

AGPU_EXPORT agpu_error agpuAttachDepthStencilBuffer ( agpu_framebuffer* framebuffer, agpu_texture* buffer )
{
	if (framebuffer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (framebuffer);
	return (*dispatchTable)->agpuAttachDepthStencilBuffer ( framebuffer, buffer );
}

AGPU_EXPORT agpu_error agpuAddShaderResourceBindingReference ( agpu_shader_resource_binding* shader_resource_binding )
{
	if (shader_resource_binding == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_resource_binding);
	return (*dispatchTable)->agpuAddShaderResourceBindingReference ( shader_resource_binding );
}

AGPU_EXPORT agpu_error agpuReleaseShaderResourceBinding ( agpu_shader_resource_binding* shader_resource_binding )
{
	if (shader_resource_binding == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_resource_binding);
	return (*dispatchTable)->agpuReleaseShaderResourceBinding ( shader_resource_binding );
}

AGPU_EXPORT agpu_error agpuBindUniformBuffer ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer )
{
	if (shader_resource_binding == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_resource_binding);
	return (*dispatchTable)->agpuBindUniformBuffer ( shader_resource_binding, location, uniform_buffer );
}

AGPU_EXPORT agpu_error agpuBindUniformBufferRange ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size )
{
	if (shader_resource_binding == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_resource_binding);
	return (*dispatchTable)->agpuBindUniformBufferRange ( shader_resource_binding, location, uniform_buffer, offset, size );
}

