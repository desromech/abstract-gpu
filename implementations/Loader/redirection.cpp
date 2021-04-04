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

AGPU_EXPORT agpu_size agpuGetPlatformGpuCount ( agpu_platform* platform )
{
	if (platform == nullptr)
		return (agpu_size)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (platform);
	return (*dispatchTable)->agpuGetPlatformGpuCount ( platform );
}

AGPU_EXPORT agpu_cstring agpuGetPlatformGpuName ( agpu_platform* platform, agpu_size gpu_index )
{
	if (platform == nullptr)
		return (agpu_cstring)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (platform);
	return (*dispatchTable)->agpuGetPlatformGpuName ( platform, gpu_index );
}

AGPU_EXPORT agpu_device_type agpuGetPlatformGpuDeviceType ( agpu_platform* platform, agpu_size gpu_index )
{
	if (platform == nullptr)
		return (agpu_device_type)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (platform);
	return (*dispatchTable)->agpuGetPlatformGpuDeviceType ( platform, gpu_index );
}

AGPU_EXPORT agpu_bool agpuIsFeatureSupportedOnGPU ( agpu_platform* platform, agpu_size gpu_index, agpu_feature feature )
{
	if (platform == nullptr)
		return (agpu_bool)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (platform);
	return (*dispatchTable)->agpuIsFeatureSupportedOnGPU ( platform, gpu_index, feature );
}

AGPU_EXPORT agpu_uint agpuGetLimitValueOnGPU ( agpu_platform* platform, agpu_size gpu_index, agpu_limit limit )
{
	if (platform == nullptr)
		return (agpu_uint)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (platform);
	return (*dispatchTable)->agpuGetLimitValueOnGPU ( platform, gpu_index, limit );
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

AGPU_EXPORT agpu_bool agpuIsCrossPlatform ( agpu_platform* platform )
{
	if (platform == nullptr)
		return (agpu_bool)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (platform);
	return (*dispatchTable)->agpuIsCrossPlatform ( platform );
}

AGPU_EXPORT agpu_offline_shader_compiler* agpuCreateOfflineShaderCompiler ( agpu_platform* platform )
{
	if (platform == nullptr)
		return (agpu_offline_shader_compiler*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (platform);
	return (*dispatchTable)->agpuCreateOfflineShaderCompiler ( platform );
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

AGPU_EXPORT agpu_swap_chain* agpuCreateSwapChain ( agpu_device* device, agpu_command_queue* commandQueue, agpu_swap_chain_create_info* swapChainInfo )
{
	if (device == nullptr)
		return (agpu_swap_chain*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateSwapChain ( device, commandQueue, swapChainInfo );
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

AGPU_EXPORT agpu_shader_signature_builder* agpuCreateShaderSignatureBuilder ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_shader_signature_builder*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateShaderSignatureBuilder ( device );
}

AGPU_EXPORT agpu_pipeline_builder* agpuCreatePipelineBuilder ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_pipeline_builder*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreatePipelineBuilder ( device );
}

AGPU_EXPORT agpu_compute_pipeline_builder* agpuCreateComputePipelineBuilder ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_compute_pipeline_builder*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateComputePipelineBuilder ( device );
}

AGPU_EXPORT agpu_command_allocator* agpuCreateCommandAllocator ( agpu_device* device, agpu_command_list_type type, agpu_command_queue* queue )
{
	if (device == nullptr)
		return (agpu_command_allocator*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateCommandAllocator ( device, type, queue );
}

AGPU_EXPORT agpu_command_list* agpuCreateCommandList ( agpu_device* device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
{
	if (device == nullptr)
		return (agpu_command_list*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateCommandList ( device, type, allocator, initial_pipeline_state );
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredShaderLanguage ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_shader_language)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuGetPreferredShaderLanguage ( device );
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredIntermediateShaderLanguage ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_shader_language)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuGetPreferredIntermediateShaderLanguage ( device );
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredHighLevelShaderLanguage ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_shader_language)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuGetPreferredHighLevelShaderLanguage ( device );
}

AGPU_EXPORT agpu_framebuffer* agpuCreateFrameBuffer ( agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view** colorViews, agpu_texture_view* depthStencilView )
{
	if (device == nullptr)
		return (agpu_framebuffer*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateFrameBuffer ( device, width, height, colorCount, colorViews, depthStencilView );
}

AGPU_EXPORT agpu_renderpass* agpuCreateRenderPass ( agpu_device* device, agpu_renderpass_description* description )
{
	if (device == nullptr)
		return (agpu_renderpass*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateRenderPass ( device, description );
}

AGPU_EXPORT agpu_texture* agpuCreateTexture ( agpu_device* device, agpu_texture_description* description )
{
	if (device == nullptr)
		return (agpu_texture*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateTexture ( device, description );
}

AGPU_EXPORT agpu_sampler* agpuCreateSampler ( agpu_device* device, agpu_sampler_description* description )
{
	if (device == nullptr)
		return (agpu_sampler*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateSampler ( device, description );
}

AGPU_EXPORT agpu_fence* agpuCreateFence ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_fence*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateFence ( device );
}

AGPU_EXPORT agpu_int agpuGetMultiSampleQualityLevels ( agpu_device* device, agpu_texture_format format, agpu_uint sample_count )
{
	if (device == nullptr)
		return (agpu_int)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuGetMultiSampleQualityLevels ( device, format, sample_count );
}

AGPU_EXPORT agpu_bool agpuHasTopLeftNdcOrigin ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_bool)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuHasTopLeftNdcOrigin ( device );
}

AGPU_EXPORT agpu_bool agpuHasBottomLeftTextureCoordinates ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_bool)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuHasBottomLeftTextureCoordinates ( device );
}

AGPU_EXPORT agpu_cstring agpuGetDeviceName ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_cstring)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuGetDeviceName ( device );
}

AGPU_EXPORT agpu_device_type agpuGetDeviceType ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_device_type)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuGetDeviceType ( device );
}

AGPU_EXPORT agpu_bool agpuIsFeatureSupportedOnDevice ( agpu_device* device, agpu_feature feature )
{
	if (device == nullptr)
		return (agpu_bool)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuIsFeatureSupportedOnDevice ( device, feature );
}

AGPU_EXPORT agpu_uint agpuGetLimitValue ( agpu_device* device, agpu_limit limit )
{
	if (device == nullptr)
		return (agpu_uint)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuGetLimitValue ( device, limit );
}

AGPU_EXPORT agpu_vr_system* agpuGetVRSystem ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_vr_system*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuGetVRSystem ( device );
}

AGPU_EXPORT agpu_offline_shader_compiler* agpuCreateOfflineShaderCompilerForDevice ( agpu_device* device )
{
	if (device == nullptr)
		return (agpu_offline_shader_compiler*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateOfflineShaderCompilerForDevice ( device );
}

AGPU_EXPORT agpu_state_tracker_cache* agpuCreateStateTrackerCache ( agpu_device* device, agpu_command_queue* command_queue_family )
{
	if (device == nullptr)
		return (agpu_state_tracker_cache*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuCreateStateTrackerCache ( device, command_queue_family );
}

AGPU_EXPORT agpu_error agpuFinishDeviceExecution ( agpu_device* device )
{
	if (device == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (device);
	return (*dispatchTable)->agpuFinishDeviceExecution ( device );
}

AGPU_EXPORT agpu_error agpuAddVRSystemReference ( agpu_vr_system* vr_system )
{
	if (vr_system == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vr_system);
	return (*dispatchTable)->agpuAddVRSystemReference ( vr_system );
}

AGPU_EXPORT agpu_error agpuReleaseVRSystem ( agpu_vr_system* vr_system )
{
	if (vr_system == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vr_system);
	return (*dispatchTable)->agpuReleaseVRSystem ( vr_system );
}

AGPU_EXPORT agpu_cstring agpuGetVRSystemName ( agpu_vr_system* vr_system )
{
	if (vr_system == nullptr)
		return (agpu_cstring)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vr_system);
	return (*dispatchTable)->agpuGetVRSystemName ( vr_system );
}

AGPU_EXPORT agpu_pointer agpuGetVRSystemNativeHandle ( agpu_vr_system* vr_system )
{
	if (vr_system == nullptr)
		return (agpu_pointer)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vr_system);
	return (*dispatchTable)->agpuGetVRSystemNativeHandle ( vr_system );
}

AGPU_EXPORT agpu_error agpuGetVRRecommendedRenderTargetSize ( agpu_vr_system* vr_system, agpu_size2d* size )
{
	if (vr_system == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vr_system);
	return (*dispatchTable)->agpuGetVRRecommendedRenderTargetSize ( vr_system, size );
}

AGPU_EXPORT agpu_error agpuGetVREyeToHeadTransformInto ( agpu_vr_system* vr_system, agpu_vr_eye eye, agpu_matrix4x4f* transform )
{
	if (vr_system == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vr_system);
	return (*dispatchTable)->agpuGetVREyeToHeadTransformInto ( vr_system, eye, transform );
}

AGPU_EXPORT agpu_error agpuGetVRProjectionMatrix ( agpu_vr_system* vr_system, agpu_vr_eye eye, agpu_float near_distance, agpu_float far_distance, agpu_matrix4x4f* projection_matrix )
{
	if (vr_system == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vr_system);
	return (*dispatchTable)->agpuGetVRProjectionMatrix ( vr_system, eye, near_distance, far_distance, projection_matrix );
}

AGPU_EXPORT agpu_error agpuGetVRProjectionFrustumTangents ( agpu_vr_system* vr_system, agpu_vr_eye eye, agpu_frustum_tangents* frustum )
{
	if (vr_system == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vr_system);
	return (*dispatchTable)->agpuGetVRProjectionFrustumTangents ( vr_system, eye, frustum );
}

AGPU_EXPORT agpu_error agpuSubmitVREyeRenderTargets ( agpu_vr_system* vr_system, agpu_texture* left_eye, agpu_texture* right_eye )
{
	if (vr_system == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vr_system);
	return (*dispatchTable)->agpuSubmitVREyeRenderTargets ( vr_system, left_eye, right_eye );
}

AGPU_EXPORT agpu_error agpuWaitAndFetchVRPoses ( agpu_vr_system* vr_system )
{
	if (vr_system == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vr_system);
	return (*dispatchTable)->agpuWaitAndFetchVRPoses ( vr_system );
}

AGPU_EXPORT agpu_size agpuGetMaxVRTrackedDevicePoseCount ( agpu_vr_system* vr_system )
{
	if (vr_system == nullptr)
		return (agpu_size)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vr_system);
	return (*dispatchTable)->agpuGetMaxVRTrackedDevicePoseCount ( vr_system );
}

AGPU_EXPORT agpu_size agpuGetCurrentVRTrackedDevicePoseCount ( agpu_vr_system* vr_system )
{
	if (vr_system == nullptr)
		return (agpu_size)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vr_system);
	return (*dispatchTable)->agpuGetCurrentVRTrackedDevicePoseCount ( vr_system );
}

AGPU_EXPORT agpu_error agpuGetCurrentVRTrackedDevicePoseInto ( agpu_vr_system* vr_system, agpu_size index, agpu_vr_tracked_device_pose* dest )
{
	if (vr_system == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vr_system);
	return (*dispatchTable)->agpuGetCurrentVRTrackedDevicePoseInto ( vr_system, index, dest );
}

AGPU_EXPORT agpu_size agpuGetMaxVRRenderTrackedDevicePoseCount ( agpu_vr_system* vr_system )
{
	if (vr_system == nullptr)
		return (agpu_size)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vr_system);
	return (*dispatchTable)->agpuGetMaxVRRenderTrackedDevicePoseCount ( vr_system );
}

AGPU_EXPORT agpu_size agpuGetCurrentVRRenderTrackedDevicePoseCount ( agpu_vr_system* vr_system )
{
	if (vr_system == nullptr)
		return (agpu_size)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vr_system);
	return (*dispatchTable)->agpuGetCurrentVRRenderTrackedDevicePoseCount ( vr_system );
}

AGPU_EXPORT agpu_error agpuGetCurrentVRRenderTrackedDevicePoseInto ( agpu_vr_system* vr_system, agpu_size index, agpu_vr_tracked_device_pose* dest )
{
	if (vr_system == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vr_system);
	return (*dispatchTable)->agpuGetCurrentVRRenderTrackedDevicePoseInto ( vr_system, index, dest );
}

AGPU_EXPORT agpu_bool agpuPollVREvent ( agpu_vr_system* vr_system, agpu_vr_event* event )
{
	if (vr_system == nullptr)
		return (agpu_bool)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vr_system);
	return (*dispatchTable)->agpuPollVREvent ( vr_system, event );
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

AGPU_EXPORT agpu_framebuffer* agpuGetCurrentBackBufferForLayer ( agpu_swap_chain* swap_chain, agpu_uint layer )
{
	if (swap_chain == nullptr)
		return (agpu_framebuffer*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (swap_chain);
	return (*dispatchTable)->agpuGetCurrentBackBufferForLayer ( swap_chain, layer );
}

AGPU_EXPORT agpu_size agpuGetCurrentBackBufferIndex ( agpu_swap_chain* swap_chain )
{
	if (swap_chain == nullptr)
		return (agpu_size)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (swap_chain);
	return (*dispatchTable)->agpuGetCurrentBackBufferIndex ( swap_chain );
}

AGPU_EXPORT agpu_size agpuGetFramebufferCount ( agpu_swap_chain* swap_chain )
{
	if (swap_chain == nullptr)
		return (agpu_size)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (swap_chain);
	return (*dispatchTable)->agpuGetFramebufferCount ( swap_chain );
}

AGPU_EXPORT agpu_uint agpuGetSwapChainWidth ( agpu_swap_chain* swap_chain )
{
	if (swap_chain == nullptr)
		return (agpu_uint)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (swap_chain);
	return (*dispatchTable)->agpuGetSwapChainWidth ( swap_chain );
}

AGPU_EXPORT agpu_uint agpuGetSwapChainHeight ( agpu_swap_chain* swap_chain )
{
	if (swap_chain == nullptr)
		return (agpu_uint)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (swap_chain);
	return (*dispatchTable)->agpuGetSwapChainHeight ( swap_chain );
}

AGPU_EXPORT agpu_uint agpuGetSwapChainLayerCount ( agpu_swap_chain* swap_chain )
{
	if (swap_chain == nullptr)
		return (agpu_uint)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (swap_chain);
	return (*dispatchTable)->agpuGetSwapChainLayerCount ( swap_chain );
}

AGPU_EXPORT agpu_error agpuSetSwapChainOverlayPosition ( agpu_swap_chain* swap_chain, agpu_int x, agpu_int y )
{
	if (swap_chain == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (swap_chain);
	return (*dispatchTable)->agpuSetSwapChainOverlayPosition ( swap_chain, x, y );
}

AGPU_EXPORT agpu_error agpuAddComputePipelineBuilderReference ( agpu_compute_pipeline_builder* compute_pipeline_builder )
{
	if (compute_pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (compute_pipeline_builder);
	return (*dispatchTable)->agpuAddComputePipelineBuilderReference ( compute_pipeline_builder );
}

AGPU_EXPORT agpu_error agpuReleaseComputePipelineBuilder ( agpu_compute_pipeline_builder* compute_pipeline_builder )
{
	if (compute_pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (compute_pipeline_builder);
	return (*dispatchTable)->agpuReleaseComputePipelineBuilder ( compute_pipeline_builder );
}

AGPU_EXPORT agpu_pipeline_state* agpuBuildComputePipelineState ( agpu_compute_pipeline_builder* compute_pipeline_builder )
{
	if (compute_pipeline_builder == nullptr)
		return (agpu_pipeline_state*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (compute_pipeline_builder);
	return (*dispatchTable)->agpuBuildComputePipelineState ( compute_pipeline_builder );
}

AGPU_EXPORT agpu_error agpuAttachComputeShader ( agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader* shader )
{
	if (compute_pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (compute_pipeline_builder);
	return (*dispatchTable)->agpuAttachComputeShader ( compute_pipeline_builder, shader );
}

AGPU_EXPORT agpu_error agpuAttachComputeShaderWithEntryPoint ( agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader* shader, agpu_shader_type type, agpu_cstring entry_point )
{
	if (compute_pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (compute_pipeline_builder);
	return (*dispatchTable)->agpuAttachComputeShaderWithEntryPoint ( compute_pipeline_builder, shader, type, entry_point );
}

AGPU_EXPORT agpu_size agpuGetComputePipelineBuildingLogLength ( agpu_compute_pipeline_builder* compute_pipeline_builder )
{
	if (compute_pipeline_builder == nullptr)
		return (agpu_size)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (compute_pipeline_builder);
	return (*dispatchTable)->agpuGetComputePipelineBuildingLogLength ( compute_pipeline_builder );
}

AGPU_EXPORT agpu_error agpuGetComputePipelineBuildingLog ( agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer )
{
	if (compute_pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (compute_pipeline_builder);
	return (*dispatchTable)->agpuGetComputePipelineBuildingLog ( compute_pipeline_builder, buffer_size, buffer );
}

AGPU_EXPORT agpu_error agpuSetComputePipelineShaderSignature ( agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader_signature* signature )
{
	if (compute_pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (compute_pipeline_builder);
	return (*dispatchTable)->agpuSetComputePipelineShaderSignature ( compute_pipeline_builder, signature );
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

AGPU_EXPORT agpu_error agpuAttachShaderWithEntryPoint ( agpu_pipeline_builder* pipeline_builder, agpu_shader* shader, agpu_shader_type type, agpu_cstring entry_point )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuAttachShaderWithEntryPoint ( pipeline_builder, shader, type, entry_point );
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

AGPU_EXPORT agpu_error agpuSetBlendState ( agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool enabled )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetBlendState ( pipeline_builder, renderTargetMask, enabled );
}

AGPU_EXPORT agpu_error agpuSetBlendFunction ( agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetBlendFunction ( pipeline_builder, renderTargetMask, sourceFactor, destFactor, colorOperation, sourceAlphaFactor, destAlphaFactor, alphaOperation );
}

AGPU_EXPORT agpu_error agpuSetColorMask ( agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetColorMask ( pipeline_builder, renderTargetMask, redEnabled, greenEnabled, blueEnabled, alphaEnabled );
}

AGPU_EXPORT agpu_error agpuSetFrontFace ( agpu_pipeline_builder* pipeline_builder, agpu_face_winding winding )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetFrontFace ( pipeline_builder, winding );
}

AGPU_EXPORT agpu_error agpuSetCullMode ( agpu_pipeline_builder* pipeline_builder, agpu_cull_mode mode )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetCullMode ( pipeline_builder, mode );
}

AGPU_EXPORT agpu_error agpuSetDepthBias ( agpu_pipeline_builder* pipeline_builder, agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetDepthBias ( pipeline_builder, constant_factor, clamp, slope_factor );
}

AGPU_EXPORT agpu_error agpuSetDepthState ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetDepthState ( pipeline_builder, enabled, writeMask, function );
}

AGPU_EXPORT agpu_error agpuSetPolygonMode ( agpu_pipeline_builder* pipeline_builder, agpu_polygon_mode mode )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetPolygonMode ( pipeline_builder, mode );
}

AGPU_EXPORT agpu_error agpuSetStencilState ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_int writeMask, agpu_int readMask )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetStencilState ( pipeline_builder, enabled, writeMask, readMask );
}

AGPU_EXPORT agpu_error agpuSetStencilFrontFace ( agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetStencilFrontFace ( pipeline_builder, stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction );
}

AGPU_EXPORT agpu_error agpuSetStencilBackFace ( agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetStencilBackFace ( pipeline_builder, stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction );
}

AGPU_EXPORT agpu_error agpuSetRenderTargetCount ( agpu_pipeline_builder* pipeline_builder, agpu_int count )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetRenderTargetCount ( pipeline_builder, count );
}

AGPU_EXPORT agpu_error agpuSetRenderTargetFormat ( agpu_pipeline_builder* pipeline_builder, agpu_uint index, agpu_texture_format format )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetRenderTargetFormat ( pipeline_builder, index, format );
}

AGPU_EXPORT agpu_error agpuSetDepthStencilFormat ( agpu_pipeline_builder* pipeline_builder, agpu_texture_format format )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetDepthStencilFormat ( pipeline_builder, format );
}

AGPU_EXPORT agpu_error agpuSetPrimitiveType ( agpu_pipeline_builder* pipeline_builder, agpu_primitive_topology type )
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

AGPU_EXPORT agpu_error agpuSetPipelineShaderSignature ( agpu_pipeline_builder* pipeline_builder, agpu_shader_signature* signature )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetPipelineShaderSignature ( pipeline_builder, signature );
}

AGPU_EXPORT agpu_error agpuSetSampleDescription ( agpu_pipeline_builder* pipeline_builder, agpu_uint sample_count, agpu_uint sample_quality )
{
	if (pipeline_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (pipeline_builder);
	return (*dispatchTable)->agpuSetSampleDescription ( pipeline_builder, sample_count, sample_quality );
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

AGPU_EXPORT agpu_error agpuSignalFence ( agpu_command_queue* command_queue, agpu_fence* fence )
{
	if (command_queue == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_queue);
	return (*dispatchTable)->agpuSignalFence ( command_queue, fence );
}

AGPU_EXPORT agpu_error agpuWaitFence ( agpu_command_queue* command_queue, agpu_fence* fence )
{
	if (command_queue == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_queue);
	return (*dispatchTable)->agpuWaitFence ( command_queue, fence );
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

AGPU_EXPORT agpu_error agpuSetShaderSignature ( agpu_command_list* command_list, agpu_shader_signature* signature )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuSetShaderSignature ( command_list, signature );
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

AGPU_EXPORT agpu_error agpuUseIndexBufferAt ( agpu_command_list* command_list, agpu_buffer* index_buffer, agpu_size offset, agpu_size index_size )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuUseIndexBufferAt ( command_list, index_buffer, offset, index_size );
}

AGPU_EXPORT agpu_error agpuUseDrawIndirectBuffer ( agpu_command_list* command_list, agpu_buffer* draw_buffer )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuUseDrawIndirectBuffer ( command_list, draw_buffer );
}

AGPU_EXPORT agpu_error agpuUseComputeDispatchIndirectBuffer ( agpu_command_list* command_list, agpu_buffer* buffer )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuUseComputeDispatchIndirectBuffer ( command_list, buffer );
}

AGPU_EXPORT agpu_error agpuUseShaderResources ( agpu_command_list* command_list, agpu_shader_resource_binding* binding )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuUseShaderResources ( command_list, binding );
}

AGPU_EXPORT agpu_error agpuUseComputeShaderResources ( agpu_command_list* command_list, agpu_shader_resource_binding* binding )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuUseComputeShaderResources ( command_list, binding );
}

AGPU_EXPORT agpu_error agpuDrawArrays ( agpu_command_list* command_list, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuDrawArrays ( command_list, vertex_count, instance_count, first_vertex, base_instance );
}

AGPU_EXPORT agpu_error agpuDrawArraysIndirect ( agpu_command_list* command_list, agpu_size offset, agpu_size drawcount )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuDrawArraysIndirect ( command_list, offset, drawcount );
}

AGPU_EXPORT agpu_error agpuDrawElements ( agpu_command_list* command_list, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuDrawElements ( command_list, index_count, instance_count, first_index, base_vertex, base_instance );
}

AGPU_EXPORT agpu_error agpuDrawElementsIndirect ( agpu_command_list* command_list, agpu_size offset, agpu_size drawcount )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuDrawElementsIndirect ( command_list, offset, drawcount );
}

AGPU_EXPORT agpu_error agpuDispatchCompute ( agpu_command_list* command_list, agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuDispatchCompute ( command_list, group_count_x, group_count_y, group_count_z );
}

AGPU_EXPORT agpu_error agpuDispatchComputeIndirect ( agpu_command_list* command_list, agpu_size offset )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuDispatchComputeIndirect ( command_list, offset );
}

AGPU_EXPORT agpu_error agpuSetStencilReference ( agpu_command_list* command_list, agpu_uint reference )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuSetStencilReference ( command_list, reference );
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

AGPU_EXPORT agpu_error agpuResetBundleCommandList ( agpu_command_list* command_list, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state, agpu_inheritance_info* inheritance_info )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuResetBundleCommandList ( command_list, allocator, initial_pipeline_state, inheritance_info );
}

AGPU_EXPORT agpu_error agpuBeginRenderPass ( agpu_command_list* command_list, agpu_renderpass* renderpass, agpu_framebuffer* framebuffer, agpu_bool bundle_content )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuBeginRenderPass ( command_list, renderpass, framebuffer, bundle_content );
}

AGPU_EXPORT agpu_error agpuEndRenderPass ( agpu_command_list* command_list )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuEndRenderPass ( command_list );
}

AGPU_EXPORT agpu_error agpuResolveFramebuffer ( agpu_command_list* command_list, agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuResolveFramebuffer ( command_list, destFramebuffer, sourceFramebuffer );
}

AGPU_EXPORT agpu_error agpuResolveTexture ( agpu_command_list* command_list, agpu_texture* sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, agpu_texture* destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuResolveTexture ( command_list, sourceTexture, sourceLevel, sourceLayer, destTexture, destLevel, destLayer, levelCount, layerCount, aspect );
}

AGPU_EXPORT agpu_error agpuPushConstants ( agpu_command_list* command_list, agpu_uint offset, agpu_uint size, agpu_pointer values )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuPushConstants ( command_list, offset, size, values );
}

AGPU_EXPORT agpu_error agpuMemoryBarrier ( agpu_command_list* command_list, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuMemoryBarrier ( command_list, source_stage, dest_stage, source_accesses, dest_accesses );
}

AGPU_EXPORT agpu_error agpuBufferMemoryBarrier ( agpu_command_list* command_list, agpu_buffer* buffer, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_size offset, agpu_size size )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuBufferMemoryBarrier ( command_list, buffer, source_stage, dest_stage, source_accesses, dest_accesses, offset, size );
}

AGPU_EXPORT agpu_error agpuTextureMemoryBarrier ( agpu_command_list* command_list, agpu_texture* texture, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_subresource_range* subresource_range )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuTextureMemoryBarrier ( command_list, texture, source_stage, dest_stage, source_accesses, dest_accesses, subresource_range );
}

AGPU_EXPORT agpu_error agpuPushBufferTransitionBarrier ( agpu_command_list* command_list, agpu_buffer* buffer, agpu_buffer_usage_mask new_usage )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuPushBufferTransitionBarrier ( command_list, buffer, new_usage );
}

AGPU_EXPORT agpu_error agpuPushTextureTransitionBarrier ( agpu_command_list* command_list, agpu_texture* texture, agpu_texture_usage_mode_mask new_usage, agpu_subresource_range* subresource_range )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuPushTextureTransitionBarrier ( command_list, texture, new_usage, subresource_range );
}

AGPU_EXPORT agpu_error agpuPopBufferTransitionBarrier ( agpu_command_list* command_list )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuPopBufferTransitionBarrier ( command_list );
}

AGPU_EXPORT agpu_error agpuPopTextureTransitionBarrier ( agpu_command_list* command_list )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuPopTextureTransitionBarrier ( command_list );
}

AGPU_EXPORT agpu_error agpuCopyBuffer ( agpu_command_list* command_list, agpu_buffer* source_buffer, agpu_size source_offset, agpu_buffer* dest_buffer, agpu_size dest_offset, agpu_size copy_size )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuCopyBuffer ( command_list, source_buffer, source_offset, dest_buffer, dest_offset, copy_size );
}

AGPU_EXPORT agpu_error agpuCopyBufferToTexture ( agpu_command_list* command_list, agpu_buffer* buffer, agpu_texture* texture, agpu_buffer_image_copy_region* copy_region )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuCopyBufferToTexture ( command_list, buffer, texture, copy_region );
}

AGPU_EXPORT agpu_error agpuCopyTextureToBuffer ( agpu_command_list* command_list, agpu_texture* texture, agpu_buffer* buffer, agpu_buffer_image_copy_region* copy_region )
{
	if (command_list == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (command_list);
	return (*dispatchTable)->agpuCopyTextureToBuffer ( command_list, texture, buffer, copy_region );
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

AGPU_EXPORT agpu_pointer agpuMapTextureLevel ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags, agpu_region3d* region )
{
	if (texture == nullptr)
		return (agpu_pointer)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture);
	return (*dispatchTable)->agpuMapTextureLevel ( texture, level, arrayIndex, flags, region );
}

AGPU_EXPORT agpu_error agpuUnmapTextureLevel ( agpu_texture* texture )
{
	if (texture == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture);
	return (*dispatchTable)->agpuUnmapTextureLevel ( texture );
}

AGPU_EXPORT agpu_error agpuReadTextureData ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer )
{
	if (texture == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture);
	return (*dispatchTable)->agpuReadTextureData ( texture, level, arrayIndex, pitch, slicePitch, buffer );
}

AGPU_EXPORT agpu_error agpuReadTextureSubData ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_region3d* sourceRegion, agpu_size3d* destSize, agpu_pointer buffer )
{
	if (texture == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture);
	return (*dispatchTable)->agpuReadTextureSubData ( texture, level, arrayIndex, pitch, slicePitch, sourceRegion, destSize, buffer );
}

AGPU_EXPORT agpu_error agpuUploadTextureData ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data )
{
	if (texture == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture);
	return (*dispatchTable)->agpuUploadTextureData ( texture, level, arrayIndex, pitch, slicePitch, data );
}

AGPU_EXPORT agpu_error agpuUploadTextureSubData ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_size3d* sourceSize, agpu_region3d* destRegion, agpu_pointer data )
{
	if (texture == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture);
	return (*dispatchTable)->agpuUploadTextureSubData ( texture, level, arrayIndex, pitch, slicePitch, sourceSize, destRegion, data );
}

AGPU_EXPORT agpu_error agpuGetTextureFullViewDescription ( agpu_texture* texture, agpu_texture_view_description* result )
{
	if (texture == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture);
	return (*dispatchTable)->agpuGetTextureFullViewDescription ( texture, result );
}

AGPU_EXPORT agpu_texture_view* agpuCreateTextureView ( agpu_texture* texture, agpu_texture_view_description* description )
{
	if (texture == nullptr)
		return (agpu_texture_view*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture);
	return (*dispatchTable)->agpuCreateTextureView ( texture, description );
}

AGPU_EXPORT agpu_texture_view* agpuGetOrCreateFullTextureView ( agpu_texture* texture )
{
	if (texture == nullptr)
		return (agpu_texture_view*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture);
	return (*dispatchTable)->agpuGetOrCreateFullTextureView ( texture );
}

AGPU_EXPORT agpu_error agpuAddTextureViewReference ( agpu_texture_view* texture_view )
{
	if (texture_view == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture_view);
	return (*dispatchTable)->agpuAddTextureViewReference ( texture_view );
}

AGPU_EXPORT agpu_error agpuReleaseTextureView ( agpu_texture_view* texture_view )
{
	if (texture_view == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture_view);
	return (*dispatchTable)->agpuReleaseTextureView ( texture_view );
}

AGPU_EXPORT agpu_texture* agpuGetTextureFromView ( agpu_texture_view* texture_view )
{
	if (texture_view == nullptr)
		return (agpu_texture*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (texture_view);
	return (*dispatchTable)->agpuGetTextureFromView ( texture_view );
}

AGPU_EXPORT agpu_error agpuAddSamplerReference ( agpu_sampler* sampler )
{
	if (sampler == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (sampler);
	return (*dispatchTable)->agpuAddSamplerReference ( sampler );
}

AGPU_EXPORT agpu_error agpuReleaseSampler ( agpu_sampler* sampler )
{
	if (sampler == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (sampler);
	return (*dispatchTable)->agpuReleaseSampler ( sampler );
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

AGPU_EXPORT agpu_error agpuFlushWholeBuffer ( agpu_buffer* buffer )
{
	if (buffer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (buffer);
	return (*dispatchTable)->agpuFlushWholeBuffer ( buffer );
}

AGPU_EXPORT agpu_error agpuInvalidateWholeBuffer ( agpu_buffer* buffer )
{
	if (buffer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (buffer);
	return (*dispatchTable)->agpuInvalidateWholeBuffer ( buffer );
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

AGPU_EXPORT agpu_error agpuBindVertexBuffersWithOffsets ( agpu_vertex_binding* vertex_binding, agpu_uint count, agpu_buffer** vertex_buffers, agpu_size* offsets )
{
	if (vertex_binding == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vertex_binding);
	return (*dispatchTable)->agpuBindVertexBuffersWithOffsets ( vertex_binding, count, vertex_buffers, offsets );
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

AGPU_EXPORT agpu_error agpuAddVertexAttributeBindings ( agpu_vertex_layout* vertex_layout, agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes )
{
	if (vertex_layout == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (vertex_layout);
	return (*dispatchTable)->agpuAddVertexAttributeBindings ( vertex_layout, vertex_buffer_count, vertex_strides, attribute_count, attributes );
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

AGPU_EXPORT agpu_uint agpuGetFramebufferWidth ( agpu_framebuffer* framebuffer )
{
	if (framebuffer == nullptr)
		return (agpu_uint)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (framebuffer);
	return (*dispatchTable)->agpuGetFramebufferWidth ( framebuffer );
}

AGPU_EXPORT agpu_uint agpuGetFramebufferHeight ( agpu_framebuffer* framebuffer )
{
	if (framebuffer == nullptr)
		return (agpu_uint)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (framebuffer);
	return (*dispatchTable)->agpuGetFramebufferHeight ( framebuffer );
}

AGPU_EXPORT agpu_error agpuAddRenderPassReference ( agpu_renderpass* renderpass )
{
	if (renderpass == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (renderpass);
	return (*dispatchTable)->agpuAddRenderPassReference ( renderpass );
}

AGPU_EXPORT agpu_error agpuReleaseRenderPass ( agpu_renderpass* renderpass )
{
	if (renderpass == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (renderpass);
	return (*dispatchTable)->agpuReleaseRenderPass ( renderpass );
}

AGPU_EXPORT agpu_error agpuSetDepthStencilClearValue ( agpu_renderpass* renderpass, agpu_depth_stencil_value value )
{
	if (renderpass == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (renderpass);
	return (*dispatchTable)->agpuSetDepthStencilClearValue ( renderpass, value );
}

AGPU_EXPORT agpu_error agpuSetColorClearValue ( agpu_renderpass* renderpass, agpu_uint attachment_index, agpu_color4f value )
{
	if (renderpass == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (renderpass);
	return (*dispatchTable)->agpuSetColorClearValue ( renderpass, attachment_index, value );
}

AGPU_EXPORT agpu_error agpuSetColorClearValueFrom ( agpu_renderpass* renderpass, agpu_uint attachment_index, agpu_color4f* value )
{
	if (renderpass == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (renderpass);
	return (*dispatchTable)->agpuSetColorClearValueFrom ( renderpass, attachment_index, value );
}

AGPU_EXPORT agpu_error agpuGetRenderPassColorAttachmentFormats ( agpu_renderpass* renderpass, agpu_uint* color_attachment_count, agpu_texture_format* formats )
{
	if (renderpass == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (renderpass);
	return (*dispatchTable)->agpuGetRenderPassColorAttachmentFormats ( renderpass, color_attachment_count, formats );
}

AGPU_EXPORT agpu_texture_format agpuGetRenderPassDepthStencilAttachmentFormat ( agpu_renderpass* renderpass )
{
	if (renderpass == nullptr)
		return (agpu_texture_format)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (renderpass);
	return (*dispatchTable)->agpuGetRenderPassDepthStencilAttachmentFormat ( renderpass );
}

AGPU_EXPORT agpu_uint agpuGetRenderPassSampleCount ( agpu_renderpass* renderpass )
{
	if (renderpass == nullptr)
		return (agpu_uint)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (renderpass);
	return (*dispatchTable)->agpuGetRenderPassSampleCount ( renderpass );
}

AGPU_EXPORT agpu_uint agpuGetRenderPassSampleQuality ( agpu_renderpass* renderpass )
{
	if (renderpass == nullptr)
		return (agpu_uint)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (renderpass);
	return (*dispatchTable)->agpuGetRenderPassSampleQuality ( renderpass );
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBuilderReference ( agpu_shader_signature_builder* shader_signature_builder )
{
	if (shader_signature_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_signature_builder);
	return (*dispatchTable)->agpuAddShaderSignatureBuilderReference ( shader_signature_builder );
}

AGPU_EXPORT agpu_error agpuReleaseShaderSignatureBuilder ( agpu_shader_signature_builder* shader_signature_builder )
{
	if (shader_signature_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_signature_builder);
	return (*dispatchTable)->agpuReleaseShaderSignatureBuilder ( shader_signature_builder );
}

AGPU_EXPORT agpu_shader_signature* agpuBuildShaderSignature ( agpu_shader_signature_builder* shader_signature_builder )
{
	if (shader_signature_builder == nullptr)
		return (agpu_shader_signature*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_signature_builder);
	return (*dispatchTable)->agpuBuildShaderSignature ( shader_signature_builder );
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingConstant ( agpu_shader_signature_builder* shader_signature_builder )
{
	if (shader_signature_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_signature_builder);
	return (*dispatchTable)->agpuAddShaderSignatureBindingConstant ( shader_signature_builder );
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingElement ( agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint maxBindings )
{
	if (shader_signature_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_signature_builder);
	return (*dispatchTable)->agpuAddShaderSignatureBindingElement ( shader_signature_builder, type, maxBindings );
}

AGPU_EXPORT agpu_error agpuBeginShaderSignatureBindingBank ( agpu_shader_signature_builder* shader_signature_builder, agpu_uint maxBindings )
{
	if (shader_signature_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_signature_builder);
	return (*dispatchTable)->agpuBeginShaderSignatureBindingBank ( shader_signature_builder, maxBindings );
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingBankElement ( agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint bindingPointCount )
{
	if (shader_signature_builder == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_signature_builder);
	return (*dispatchTable)->agpuAddShaderSignatureBindingBankElement ( shader_signature_builder, type, bindingPointCount );
}

AGPU_EXPORT agpu_error agpuAddShaderSignature ( agpu_shader_signature* shader_signature )
{
	if (shader_signature == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_signature);
	return (*dispatchTable)->agpuAddShaderSignature ( shader_signature );
}

AGPU_EXPORT agpu_error agpuReleaseShaderSignature ( agpu_shader_signature* shader_signature )
{
	if (shader_signature == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_signature);
	return (*dispatchTable)->agpuReleaseShaderSignature ( shader_signature );
}

AGPU_EXPORT agpu_shader_resource_binding* agpuCreateShaderResourceBinding ( agpu_shader_signature* shader_signature, agpu_uint element )
{
	if (shader_signature == nullptr)
		return (agpu_shader_resource_binding*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_signature);
	return (*dispatchTable)->agpuCreateShaderResourceBinding ( shader_signature, element );
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

AGPU_EXPORT agpu_error agpuBindStorageBuffer ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* storage_buffer )
{
	if (shader_resource_binding == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_resource_binding);
	return (*dispatchTable)->agpuBindStorageBuffer ( shader_resource_binding, location, storage_buffer );
}

AGPU_EXPORT agpu_error agpuBindStorageBufferRange ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* storage_buffer, agpu_size offset, agpu_size size )
{
	if (shader_resource_binding == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_resource_binding);
	return (*dispatchTable)->agpuBindStorageBufferRange ( shader_resource_binding, location, storage_buffer, offset, size );
}

AGPU_EXPORT agpu_error agpuBindSampledTextureView ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture_view* view )
{
	if (shader_resource_binding == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_resource_binding);
	return (*dispatchTable)->agpuBindSampledTextureView ( shader_resource_binding, location, view );
}

AGPU_EXPORT agpu_error agpuBindStorageImageView ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture_view* view )
{
	if (shader_resource_binding == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_resource_binding);
	return (*dispatchTable)->agpuBindStorageImageView ( shader_resource_binding, location, view );
}

AGPU_EXPORT agpu_error agpuBindSampler ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_sampler* sampler )
{
	if (shader_resource_binding == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (shader_resource_binding);
	return (*dispatchTable)->agpuBindSampler ( shader_resource_binding, location, sampler );
}

AGPU_EXPORT agpu_error agpuAddFenceReference ( agpu_fence* fence )
{
	if (fence == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (fence);
	return (*dispatchTable)->agpuAddFenceReference ( fence );
}

AGPU_EXPORT agpu_error agpuReleaseFenceReference ( agpu_fence* fence )
{
	if (fence == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (fence);
	return (*dispatchTable)->agpuReleaseFenceReference ( fence );
}

AGPU_EXPORT agpu_error agpuWaitOnClient ( agpu_fence* fence )
{
	if (fence == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (fence);
	return (*dispatchTable)->agpuWaitOnClient ( fence );
}

AGPU_EXPORT agpu_error agpuAddOfflineShaderCompilerReference ( agpu_offline_shader_compiler* offline_shader_compiler )
{
	if (offline_shader_compiler == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (offline_shader_compiler);
	return (*dispatchTable)->agpuAddOfflineShaderCompilerReference ( offline_shader_compiler );
}

AGPU_EXPORT agpu_error agpuReleaseOfflineShaderCompiler ( agpu_offline_shader_compiler* offline_shader_compiler )
{
	if (offline_shader_compiler == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (offline_shader_compiler);
	return (*dispatchTable)->agpuReleaseOfflineShaderCompiler ( offline_shader_compiler );
}

AGPU_EXPORT agpu_bool agpuIsShaderLanguageSupportedByOfflineCompiler ( agpu_offline_shader_compiler* offline_shader_compiler, agpu_shader_language language )
{
	if (offline_shader_compiler == nullptr)
		return (agpu_bool)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (offline_shader_compiler);
	return (*dispatchTable)->agpuIsShaderLanguageSupportedByOfflineCompiler ( offline_shader_compiler, language );
}

AGPU_EXPORT agpu_bool agpuIsTargetShaderLanguageSupportedByOfflineCompiler ( agpu_offline_shader_compiler* offline_shader_compiler, agpu_shader_language language )
{
	if (offline_shader_compiler == nullptr)
		return (agpu_bool)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (offline_shader_compiler);
	return (*dispatchTable)->agpuIsTargetShaderLanguageSupportedByOfflineCompiler ( offline_shader_compiler, language );
}

AGPU_EXPORT agpu_error agpuSetOfflineShaderCompilerSource ( agpu_offline_shader_compiler* offline_shader_compiler, agpu_shader_language language, agpu_shader_type stage, agpu_string sourceText, agpu_string_length sourceTextLength )
{
	if (offline_shader_compiler == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (offline_shader_compiler);
	return (*dispatchTable)->agpuSetOfflineShaderCompilerSource ( offline_shader_compiler, language, stage, sourceText, sourceTextLength );
}

AGPU_EXPORT agpu_error agpuCompileOfflineShader ( agpu_offline_shader_compiler* offline_shader_compiler, agpu_shader_language target_language, agpu_cstring options )
{
	if (offline_shader_compiler == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (offline_shader_compiler);
	return (*dispatchTable)->agpuCompileOfflineShader ( offline_shader_compiler, target_language, options );
}

AGPU_EXPORT agpu_size agpuGetOfflineShaderCompilationLogLength ( agpu_offline_shader_compiler* offline_shader_compiler )
{
	if (offline_shader_compiler == nullptr)
		return (agpu_size)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (offline_shader_compiler);
	return (*dispatchTable)->agpuGetOfflineShaderCompilationLogLength ( offline_shader_compiler );
}

AGPU_EXPORT agpu_error agpuGetOfflineShaderCompilationLog ( agpu_offline_shader_compiler* offline_shader_compiler, agpu_size buffer_size, agpu_string_buffer buffer )
{
	if (offline_shader_compiler == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (offline_shader_compiler);
	return (*dispatchTable)->agpuGetOfflineShaderCompilationLog ( offline_shader_compiler, buffer_size, buffer );
}

AGPU_EXPORT agpu_size agpuGetOfflineShaderCompilationResultLength ( agpu_offline_shader_compiler* offline_shader_compiler )
{
	if (offline_shader_compiler == nullptr)
		return (agpu_size)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (offline_shader_compiler);
	return (*dispatchTable)->agpuGetOfflineShaderCompilationResultLength ( offline_shader_compiler );
}

AGPU_EXPORT agpu_error agpuGetOfflineShaderCompilationResult ( agpu_offline_shader_compiler* offline_shader_compiler, agpu_size buffer_size, agpu_string_buffer buffer )
{
	if (offline_shader_compiler == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (offline_shader_compiler);
	return (*dispatchTable)->agpuGetOfflineShaderCompilationResult ( offline_shader_compiler, buffer_size, buffer );
}

AGPU_EXPORT agpu_shader* agpuGetOfflineShaderCompilerResultAsShader ( agpu_offline_shader_compiler* offline_shader_compiler )
{
	if (offline_shader_compiler == nullptr)
		return (agpu_shader*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (offline_shader_compiler);
	return (*dispatchTable)->agpuGetOfflineShaderCompilerResultAsShader ( offline_shader_compiler );
}

AGPU_EXPORT agpu_error agpuAddStateTrackerCacheReference ( agpu_state_tracker_cache* state_tracker_cache )
{
	if (state_tracker_cache == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker_cache);
	return (*dispatchTable)->agpuAddStateTrackerCacheReference ( state_tracker_cache );
}

AGPU_EXPORT agpu_error agpuReleaseStateTrackerCacheReference ( agpu_state_tracker_cache* state_tracker_cache )
{
	if (state_tracker_cache == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker_cache);
	return (*dispatchTable)->agpuReleaseStateTrackerCacheReference ( state_tracker_cache );
}

AGPU_EXPORT agpu_state_tracker* agpuCreateStateTracker ( agpu_state_tracker_cache* state_tracker_cache, agpu_command_list_type type, agpu_command_queue* command_queue )
{
	if (state_tracker_cache == nullptr)
		return (agpu_state_tracker*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker_cache);
	return (*dispatchTable)->agpuCreateStateTracker ( state_tracker_cache, type, command_queue );
}

AGPU_EXPORT agpu_state_tracker* agpuCreateStateTrackerWithCommandAllocator ( agpu_state_tracker_cache* state_tracker_cache, agpu_command_list_type type, agpu_command_queue* command_queue, agpu_command_allocator* command_allocator )
{
	if (state_tracker_cache == nullptr)
		return (agpu_state_tracker*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker_cache);
	return (*dispatchTable)->agpuCreateStateTrackerWithCommandAllocator ( state_tracker_cache, type, command_queue, command_allocator );
}

AGPU_EXPORT agpu_state_tracker* agpuCreateStateTrackerWithFrameBuffering ( agpu_state_tracker_cache* state_tracker_cache, agpu_command_list_type type, agpu_command_queue* command_queue, agpu_uint framebuffering_count )
{
	if (state_tracker_cache == nullptr)
		return (agpu_state_tracker*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker_cache);
	return (*dispatchTable)->agpuCreateStateTrackerWithFrameBuffering ( state_tracker_cache, type, command_queue, framebuffering_count );
}

AGPU_EXPORT agpu_immediate_renderer* agpuCreateImmediateRenderer ( agpu_state_tracker_cache* state_tracker_cache )
{
	if (state_tracker_cache == nullptr)
		return (agpu_immediate_renderer*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker_cache);
	return (*dispatchTable)->agpuCreateImmediateRenderer ( state_tracker_cache );
}

AGPU_EXPORT agpu_error agpuAddStateTrackerReference ( agpu_state_tracker* state_tracker )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuAddStateTrackerReference ( state_tracker );
}

AGPU_EXPORT agpu_error agpuReleaseStateTrackerReference ( agpu_state_tracker* state_tracker )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuReleaseStateTrackerReference ( state_tracker );
}

AGPU_EXPORT agpu_error agpuStateTrackerBeginRecordingCommands ( agpu_state_tracker* state_tracker )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerBeginRecordingCommands ( state_tracker );
}

AGPU_EXPORT agpu_command_list* agpuStateTrackerEndRecordingCommands ( agpu_state_tracker* state_tracker )
{
	if (state_tracker == nullptr)
		return (agpu_command_list*)0;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerEndRecordingCommands ( state_tracker );
}

AGPU_EXPORT agpu_error agpuStateTrackerEndRecordingAndFlushCommands ( agpu_state_tracker* state_tracker )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerEndRecordingAndFlushCommands ( state_tracker );
}

AGPU_EXPORT agpu_error agpuStateTrackerReset ( agpu_state_tracker* state_tracker )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerReset ( state_tracker );
}

AGPU_EXPORT agpu_error agpuStateTrackerResetGraphicsPipeline ( agpu_state_tracker* state_tracker )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerResetGraphicsPipeline ( state_tracker );
}

AGPU_EXPORT agpu_error agpuStateTrackerResetComputePipeline ( agpu_state_tracker* state_tracker )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerResetComputePipeline ( state_tracker );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetComputeStage ( agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetComputeStage ( state_tracker, shader, entryPoint );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetVertexStage ( agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetVertexStage ( state_tracker, shader, entryPoint );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetFragmentStage ( agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetFragmentStage ( state_tracker, shader, entryPoint );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetGeometryStage ( agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetGeometryStage ( state_tracker, shader, entryPoint );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetTessellationControlStage ( agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetTessellationControlStage ( state_tracker, shader, entryPoint );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetTessellationEvaluationStage ( agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetTessellationEvaluationStage ( state_tracker, shader, entryPoint );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetBlendState ( agpu_state_tracker* state_tracker, agpu_int renderTargetMask, agpu_bool enabled )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetBlendState ( state_tracker, renderTargetMask, enabled );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetBlendFunction ( agpu_state_tracker* state_tracker, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetBlendFunction ( state_tracker, renderTargetMask, sourceFactor, destFactor, colorOperation, sourceAlphaFactor, destAlphaFactor, alphaOperation );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetColorMask ( agpu_state_tracker* state_tracker, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetColorMask ( state_tracker, renderTargetMask, redEnabled, greenEnabled, blueEnabled, alphaEnabled );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetFrontFace ( agpu_state_tracker* state_tracker, agpu_face_winding winding )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetFrontFace ( state_tracker, winding );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetCullMode ( agpu_state_tracker* state_tracker, agpu_cull_mode mode )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetCullMode ( state_tracker, mode );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetDepthBias ( agpu_state_tracker* state_tracker, agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetDepthBias ( state_tracker, constant_factor, clamp, slope_factor );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetDepthState ( agpu_state_tracker* state_tracker, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetDepthState ( state_tracker, enabled, writeMask, function );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetPolygonMode ( agpu_state_tracker* state_tracker, agpu_polygon_mode mode )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetPolygonMode ( state_tracker, mode );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetStencilState ( agpu_state_tracker* state_tracker, agpu_bool enabled, agpu_int writeMask, agpu_int readMask )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetStencilState ( state_tracker, enabled, writeMask, readMask );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetStencilFrontFace ( agpu_state_tracker* state_tracker, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetStencilFrontFace ( state_tracker, stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetStencilBackFace ( agpu_state_tracker* state_tracker, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetStencilBackFace ( state_tracker, stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetPrimitiveType ( agpu_state_tracker* state_tracker, agpu_primitive_topology type )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetPrimitiveType ( state_tracker, type );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetVertexLayout ( agpu_state_tracker* state_tracker, agpu_vertex_layout* layout )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetVertexLayout ( state_tracker, layout );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetShaderSignature ( agpu_state_tracker* state_tracker, agpu_shader_signature* signature )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetShaderSignature ( state_tracker, signature );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetSampleDescription ( agpu_state_tracker* state_tracker, agpu_uint sample_count, agpu_uint sample_quality )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetSampleDescription ( state_tracker, sample_count, sample_quality );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetViewport ( agpu_state_tracker* state_tracker, agpu_int x, agpu_int y, agpu_int w, agpu_int h )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetViewport ( state_tracker, x, y, w, h );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetScissor ( agpu_state_tracker* state_tracker, agpu_int x, agpu_int y, agpu_int w, agpu_int h )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetScissor ( state_tracker, x, y, w, h );
}

AGPU_EXPORT agpu_error agpuStateTrackerUseVertexBinding ( agpu_state_tracker* state_tracker, agpu_vertex_binding* vertex_binding )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerUseVertexBinding ( state_tracker, vertex_binding );
}

AGPU_EXPORT agpu_error agpuStateTrackerUseIndexBuffer ( agpu_state_tracker* state_tracker, agpu_buffer* index_buffer )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerUseIndexBuffer ( state_tracker, index_buffer );
}

AGPU_EXPORT agpu_error agpuStateTrackerUseIndexBufferAt ( agpu_state_tracker* state_tracker, agpu_buffer* index_buffer, agpu_size offset, agpu_size index_size )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerUseIndexBufferAt ( state_tracker, index_buffer, offset, index_size );
}

AGPU_EXPORT agpu_error agpuStateTrackerUseDrawIndirectBuffer ( agpu_state_tracker* state_tracker, agpu_buffer* draw_buffer )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerUseDrawIndirectBuffer ( state_tracker, draw_buffer );
}

AGPU_EXPORT agpu_error agpuStateTrackerUseComputeDispatchIndirectBuffer ( agpu_state_tracker* state_tracker, agpu_buffer* buffer )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerUseComputeDispatchIndirectBuffer ( state_tracker, buffer );
}

AGPU_EXPORT agpu_error agpuStateTrackerUseShaderResources ( agpu_state_tracker* state_tracker, agpu_shader_resource_binding* binding )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerUseShaderResources ( state_tracker, binding );
}

AGPU_EXPORT agpu_error agpuStateTrackerUseComputeShaderResources ( agpu_state_tracker* state_tracker, agpu_shader_resource_binding* binding )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerUseComputeShaderResources ( state_tracker, binding );
}

AGPU_EXPORT agpu_error agpuStateTrackerDrawArrays ( agpu_state_tracker* state_tracker, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerDrawArrays ( state_tracker, vertex_count, instance_count, first_vertex, base_instance );
}

AGPU_EXPORT agpu_error agpuStateTrackerDrawArraysIndirect ( agpu_state_tracker* state_tracker, agpu_size offset, agpu_size drawcount )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerDrawArraysIndirect ( state_tracker, offset, drawcount );
}

AGPU_EXPORT agpu_error agpuStateTrackerDrawElements ( agpu_state_tracker* state_tracker, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerDrawElements ( state_tracker, index_count, instance_count, first_index, base_vertex, base_instance );
}

AGPU_EXPORT agpu_error agpuStateTrackerDrawElementsIndirect ( agpu_state_tracker* state_tracker, agpu_size offset, agpu_size drawcount )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerDrawElementsIndirect ( state_tracker, offset, drawcount );
}

AGPU_EXPORT agpu_error agpuStateTrackerDispatchCompute ( agpu_state_tracker* state_tracker, agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerDispatchCompute ( state_tracker, group_count_x, group_count_y, group_count_z );
}

AGPU_EXPORT agpu_error agpuStateTrackerDispatchComputeIndirect ( agpu_state_tracker* state_tracker, agpu_size offset )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerDispatchComputeIndirect ( state_tracker, offset );
}

AGPU_EXPORT agpu_error agpuStateTrackerSetStencilReference ( agpu_state_tracker* state_tracker, agpu_uint reference )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerSetStencilReference ( state_tracker, reference );
}

AGPU_EXPORT agpu_error agpuStateTrackerExecuteBundle ( agpu_state_tracker* state_tracker, agpu_command_list* bundle )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerExecuteBundle ( state_tracker, bundle );
}

AGPU_EXPORT agpu_error agpuStateTrackerBeginRenderPass ( agpu_state_tracker* state_tracker, agpu_renderpass* renderpass, agpu_framebuffer* framebuffer, agpu_bool bundle_content )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerBeginRenderPass ( state_tracker, renderpass, framebuffer, bundle_content );
}

AGPU_EXPORT agpu_error agpuStateTrackerEndRenderPass ( agpu_state_tracker* state_tracker )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerEndRenderPass ( state_tracker );
}

AGPU_EXPORT agpu_error agpuStateTrackerResolveFramebuffer ( agpu_state_tracker* state_tracker, agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerResolveFramebuffer ( state_tracker, destFramebuffer, sourceFramebuffer );
}

AGPU_EXPORT agpu_error agpuStateTrackerResolveTexture ( agpu_state_tracker* state_tracker, agpu_texture* sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, agpu_texture* destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerResolveTexture ( state_tracker, sourceTexture, sourceLevel, sourceLayer, destTexture, destLevel, destLayer, levelCount, layerCount, aspect );
}

AGPU_EXPORT agpu_error agpuStateTrackerPushConstants ( agpu_state_tracker* state_tracker, agpu_uint offset, agpu_uint size, agpu_pointer values )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerPushConstants ( state_tracker, offset, size, values );
}

AGPU_EXPORT agpu_error agpuStateTrackerMemoryBarrier ( agpu_state_tracker* state_tracker, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerMemoryBarrier ( state_tracker, source_stage, dest_stage, source_accesses, dest_accesses );
}

AGPU_EXPORT agpu_error agpuStateTrackerBufferMemoryBarrier ( agpu_state_tracker* state_tracker, agpu_buffer* buffer, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_size offset, agpu_size size )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerBufferMemoryBarrier ( state_tracker, buffer, source_stage, dest_stage, source_accesses, dest_accesses, offset, size );
}

AGPU_EXPORT agpu_error agpuStateTrackerTextureMemoryBarrier ( agpu_state_tracker* state_tracker, agpu_texture* texture, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_subresource_range* subresource_range )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerTextureMemoryBarrier ( state_tracker, texture, source_stage, dest_stage, source_accesses, dest_accesses, subresource_range );
}

AGPU_EXPORT agpu_error agpuStateTrackerPushBufferTransitionBarrier ( agpu_state_tracker* state_tracker, agpu_buffer* buffer, agpu_buffer_usage_mask new_usage )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerPushBufferTransitionBarrier ( state_tracker, buffer, new_usage );
}

AGPU_EXPORT agpu_error agpuStateTrackerPushTextureTransitionBarrier ( agpu_state_tracker* state_tracker, agpu_texture* texture, agpu_texture_usage_mode_mask new_usage, agpu_subresource_range* subresource_range )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerPushTextureTransitionBarrier ( state_tracker, texture, new_usage, subresource_range );
}

AGPU_EXPORT agpu_error agpuStateTrackerPopBufferTransitionBarrier ( agpu_state_tracker* state_tracker )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerPopBufferTransitionBarrier ( state_tracker );
}

AGPU_EXPORT agpu_error agpuStateTrackerPopTextureTransitionBarrier ( agpu_state_tracker* state_tracker )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerPopTextureTransitionBarrier ( state_tracker );
}

AGPU_EXPORT agpu_error agpuStateTrackerCopyBuffer ( agpu_state_tracker* state_tracker, agpu_buffer* source_buffer, agpu_size source_offset, agpu_buffer* dest_buffer, agpu_size dest_offset, agpu_size copy_size )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerCopyBuffer ( state_tracker, source_buffer, source_offset, dest_buffer, dest_offset, copy_size );
}

AGPU_EXPORT agpu_error agpuStateTrackerCopyBufferToTexture ( agpu_state_tracker* state_tracker, agpu_buffer* buffer, agpu_texture* texture, agpu_buffer_image_copy_region* copy_region )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerCopyBufferToTexture ( state_tracker, buffer, texture, copy_region );
}

AGPU_EXPORT agpu_error agpuStateTrackerCopyTextureToBuffer ( agpu_state_tracker* state_tracker, agpu_texture* texture, agpu_buffer* buffer, agpu_buffer_image_copy_region* copy_region )
{
	if (state_tracker == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (state_tracker);
	return (*dispatchTable)->agpuStateTrackerCopyTextureToBuffer ( state_tracker, texture, buffer, copy_region );
}

AGPU_EXPORT agpu_error agpuAddImmediateRendererReference ( agpu_immediate_renderer* immediate_renderer )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuAddImmediateRendererReference ( immediate_renderer );
}

AGPU_EXPORT agpu_error agpuReleaseImmediateRendererReference ( agpu_immediate_renderer* immediate_renderer )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuReleaseImmediateRendererReference ( immediate_renderer );
}

AGPU_EXPORT agpu_error agpuBeginImmediateRendering ( agpu_immediate_renderer* immediate_renderer, agpu_state_tracker* state_tracker )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuBeginImmediateRendering ( immediate_renderer, state_tracker );
}

AGPU_EXPORT agpu_error agpuEndImmediateRendering ( agpu_immediate_renderer* immediate_renderer )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuEndImmediateRendering ( immediate_renderer );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetBlendState ( agpu_immediate_renderer* immediate_renderer, agpu_int renderTargetMask, agpu_bool enabled )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetBlendState ( immediate_renderer, renderTargetMask, enabled );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetBlendFunction ( agpu_immediate_renderer* immediate_renderer, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetBlendFunction ( immediate_renderer, renderTargetMask, sourceFactor, destFactor, colorOperation, sourceAlphaFactor, destAlphaFactor, alphaOperation );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetColorMask ( agpu_immediate_renderer* immediate_renderer, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetColorMask ( immediate_renderer, renderTargetMask, redEnabled, greenEnabled, blueEnabled, alphaEnabled );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetFrontFace ( agpu_immediate_renderer* immediate_renderer, agpu_face_winding winding )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetFrontFace ( immediate_renderer, winding );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetCullMode ( agpu_immediate_renderer* immediate_renderer, agpu_cull_mode mode )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetCullMode ( immediate_renderer, mode );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetDepthBias ( agpu_immediate_renderer* immediate_renderer, agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetDepthBias ( immediate_renderer, constant_factor, clamp, slope_factor );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetDepthState ( agpu_immediate_renderer* immediate_renderer, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetDepthState ( immediate_renderer, enabled, writeMask, function );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetPolygonMode ( agpu_immediate_renderer* immediate_renderer, agpu_polygon_mode mode )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetPolygonMode ( immediate_renderer, mode );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetStencilState ( agpu_immediate_renderer* immediate_renderer, agpu_bool enabled, agpu_int writeMask, agpu_int readMask )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetStencilState ( immediate_renderer, enabled, writeMask, readMask );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetStencilFrontFace ( agpu_immediate_renderer* immediate_renderer, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetStencilFrontFace ( immediate_renderer, stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetStencilBackFace ( agpu_immediate_renderer* immediate_renderer, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetStencilBackFace ( immediate_renderer, stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction );
}

AGPU_EXPORT agpu_error agpuImmediateSetSamplingMode ( agpu_immediate_renderer* immediate_renderer, agpu_filter filter, agpu_float maxAnisotropy, agpu_texture_address_mode addressU, agpu_texture_address_mode addressV, agpu_texture_address_mode addressW )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateSetSamplingMode ( immediate_renderer, filter, maxAnisotropy, addressU, addressV, addressW );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetViewport ( agpu_immediate_renderer* immediate_renderer, agpu_int x, agpu_int y, agpu_int w, agpu_int h )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetViewport ( immediate_renderer, x, y, w, h );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetScissor ( agpu_immediate_renderer* immediate_renderer, agpu_int x, agpu_int y, agpu_int w, agpu_int h )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetScissor ( immediate_renderer, x, y, w, h );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetStencilReference ( agpu_immediate_renderer* immediate_renderer, agpu_uint reference )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetStencilReference ( immediate_renderer, reference );
}

AGPU_EXPORT agpu_error agpuImmediateRendererProjectionMatrixMode ( agpu_immediate_renderer* immediate_renderer )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererProjectionMatrixMode ( immediate_renderer );
}

AGPU_EXPORT agpu_error agpuImmediateRendererModelViewMatrixMode ( agpu_immediate_renderer* immediate_renderer )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererModelViewMatrixMode ( immediate_renderer );
}

AGPU_EXPORT agpu_error agpuImmediateRendererTextureMatrixMode ( agpu_immediate_renderer* immediate_renderer )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererTextureMatrixMode ( immediate_renderer );
}

AGPU_EXPORT agpu_error agpuImmediateRendererIdentity ( agpu_immediate_renderer* immediate_renderer )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererIdentity ( immediate_renderer );
}

AGPU_EXPORT agpu_error agpuImmediateRendererPushMatrix ( agpu_immediate_renderer* immediate_renderer )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererPushMatrix ( immediate_renderer );
}

AGPU_EXPORT agpu_error agpuImmediateRendererPopMatrix ( agpu_immediate_renderer* immediate_renderer )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererPopMatrix ( immediate_renderer );
}

AGPU_EXPORT agpu_error agpuImmediateRendererLoadMatrix ( agpu_immediate_renderer* immediate_renderer, agpu_float* elements )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererLoadMatrix ( immediate_renderer, elements );
}

AGPU_EXPORT agpu_error agpuImmediateRendererLoadTransposeMatrix ( agpu_immediate_renderer* immediate_renderer, agpu_float* elements )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererLoadTransposeMatrix ( immediate_renderer, elements );
}

AGPU_EXPORT agpu_error agpuImmediateRendererMultiplyMatrix ( agpu_immediate_renderer* immediate_renderer, agpu_float* elements )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererMultiplyMatrix ( immediate_renderer, elements );
}

AGPU_EXPORT agpu_error agpuImmediateRendererMultiplyTransposeMatrix ( agpu_immediate_renderer* immediate_renderer, agpu_float* elements )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererMultiplyTransposeMatrix ( immediate_renderer, elements );
}

AGPU_EXPORT agpu_error agpuImmediateRendererOrtho ( agpu_immediate_renderer* immediate_renderer, agpu_float left, agpu_float right, agpu_float bottom, agpu_float top, agpu_float near, agpu_float far )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererOrtho ( immediate_renderer, left, right, bottom, top, near, far );
}

AGPU_EXPORT agpu_error agpuImmediateRendererFrustum ( agpu_immediate_renderer* immediate_renderer, agpu_float left, agpu_float right, agpu_float bottom, agpu_float top, agpu_float near, agpu_float far )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererFrustum ( immediate_renderer, left, right, bottom, top, near, far );
}

AGPU_EXPORT agpu_error agpuImmediateRendererPerspective ( agpu_immediate_renderer* immediate_renderer, agpu_float fovy, agpu_float aspect, agpu_float near, agpu_float far )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererPerspective ( immediate_renderer, fovy, aspect, near, far );
}

AGPU_EXPORT agpu_error agpuImmediateRendererRotate ( agpu_immediate_renderer* immediate_renderer, agpu_float angle, agpu_float x, agpu_float y, agpu_float z )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererRotate ( immediate_renderer, angle, x, y, z );
}

AGPU_EXPORT agpu_error agpuImmediateRendererTranslate ( agpu_immediate_renderer* immediate_renderer, agpu_float x, agpu_float y, agpu_float z )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererTranslate ( immediate_renderer, x, y, z );
}

AGPU_EXPORT agpu_error agpuImmediateRendererScale ( agpu_immediate_renderer* immediate_renderer, agpu_float x, agpu_float y, agpu_float z )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererScale ( immediate_renderer, x, y, z );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetFlatShading ( agpu_immediate_renderer* immediate_renderer, agpu_bool enabled )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetFlatShading ( immediate_renderer, enabled );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetLightingEnabled ( agpu_immediate_renderer* immediate_renderer, agpu_bool enabled )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetLightingEnabled ( immediate_renderer, enabled );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetLightingModel ( agpu_immediate_renderer* immediate_renderer, agpu_immediate_renderer_lighting_model model )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetLightingModel ( immediate_renderer, model );
}

AGPU_EXPORT agpu_error agpuImmediateRendererClearLights ( agpu_immediate_renderer* immediate_renderer )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererClearLights ( immediate_renderer );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetAmbientLighting ( agpu_immediate_renderer* immediate_renderer, agpu_float r, agpu_float g, agpu_float b, agpu_float a )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetAmbientLighting ( immediate_renderer, r, g, b, a );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetLight ( agpu_immediate_renderer* immediate_renderer, agpu_uint index, agpu_bool enabled, agpu_immediate_renderer_light* state )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetLight ( immediate_renderer, index, enabled, state );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetMaterial ( agpu_immediate_renderer* immediate_renderer, agpu_immediate_renderer_material* state )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetMaterial ( immediate_renderer, state );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetSkinningEnabled ( agpu_immediate_renderer* immediate_renderer, agpu_bool enabled )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetSkinningEnabled ( immediate_renderer, enabled );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetSkinBones ( agpu_immediate_renderer* immediate_renderer, agpu_uint count, agpu_float* matrices, agpu_bool transpose )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetSkinBones ( immediate_renderer, count, matrices, transpose );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetTextureEnabled ( agpu_immediate_renderer* immediate_renderer, agpu_bool enabled )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetTextureEnabled ( immediate_renderer, enabled );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetTangentSpaceEnabled ( agpu_immediate_renderer* immediate_renderer, agpu_bool enabled )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetTangentSpaceEnabled ( immediate_renderer, enabled );
}

AGPU_EXPORT agpu_error agpuImmediateRendererBindTexture ( agpu_immediate_renderer* immediate_renderer, agpu_texture* texture )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererBindTexture ( immediate_renderer, texture );
}

AGPU_EXPORT agpu_error agpuImmediateRendererBindTextureIn ( agpu_immediate_renderer* immediate_renderer, agpu_texture* texture, agpu_immediate_renderer_texture_binding binding )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererBindTextureIn ( immediate_renderer, texture, binding );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetClipPlane ( agpu_immediate_renderer* immediate_renderer, agpu_uint index, agpu_bool enabled, agpu_float p1, agpu_float p2, agpu_float p3, agpu_float p4 )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetClipPlane ( immediate_renderer, index, enabled, p1, p2, p3, p4 );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetFogMode ( agpu_immediate_renderer* immediate_renderer, agpu_immediate_renderer_fog_mode mode )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetFogMode ( immediate_renderer, mode );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetFogColor ( agpu_immediate_renderer* immediate_renderer, agpu_float r, agpu_float g, agpu_float b, agpu_float a )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetFogColor ( immediate_renderer, r, g, b, a );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetFogDistances ( agpu_immediate_renderer* immediate_renderer, agpu_float start, agpu_float end )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetFogDistances ( immediate_renderer, start, end );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetFogDensity ( agpu_immediate_renderer* immediate_renderer, agpu_float density )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetFogDensity ( immediate_renderer, density );
}

AGPU_EXPORT agpu_error agpuBeginImmediateRendererPrimitives ( agpu_immediate_renderer* immediate_renderer, agpu_primitive_topology type )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuBeginImmediateRendererPrimitives ( immediate_renderer, type );
}

AGPU_EXPORT agpu_error agpuEndImmediateRendererPrimitives ( agpu_immediate_renderer* immediate_renderer )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuEndImmediateRendererPrimitives ( immediate_renderer );
}

AGPU_EXPORT agpu_error agpuSetImmediateRendererColor ( agpu_immediate_renderer* immediate_renderer, agpu_float r, agpu_float g, agpu_float b, agpu_float a )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuSetImmediateRendererColor ( immediate_renderer, r, g, b, a );
}

AGPU_EXPORT agpu_error agpuSetImmediateRendererTexcoord ( agpu_immediate_renderer* immediate_renderer, agpu_float x, agpu_float y )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuSetImmediateRendererTexcoord ( immediate_renderer, x, y );
}

AGPU_EXPORT agpu_error agpuSetImmediateRendererNormal ( agpu_immediate_renderer* immediate_renderer, agpu_float x, agpu_float y, agpu_float z )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuSetImmediateRendererNormal ( immediate_renderer, x, y, z );
}

AGPU_EXPORT agpu_error agpuAddImmediateRendererVertex ( agpu_immediate_renderer* immediate_renderer, agpu_float x, agpu_float y, agpu_float z )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuAddImmediateRendererVertex ( immediate_renderer, x, y, z );
}

AGPU_EXPORT agpu_error agpuBeginImmediateRendererMeshWithVertices ( agpu_immediate_renderer* immediate_renderer, agpu_size vertexCount, agpu_size stride, agpu_size elementCount, agpu_pointer vertices )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuBeginImmediateRendererMeshWithVertices ( immediate_renderer, vertexCount, stride, elementCount, vertices );
}

AGPU_EXPORT agpu_error agpuBeginImmediateRendererMeshWithVertexBinding ( agpu_immediate_renderer* immediate_renderer, agpu_vertex_layout* layout, agpu_vertex_binding* vertices )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuBeginImmediateRendererMeshWithVertexBinding ( immediate_renderer, layout, vertices );
}

AGPU_EXPORT agpu_error agpuImmediateRendererUseIndexBuffer ( agpu_immediate_renderer* immediate_renderer, agpu_buffer* index_buffer )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererUseIndexBuffer ( immediate_renderer, index_buffer );
}

AGPU_EXPORT agpu_error agpuImmediateRendererUseIndexBufferAt ( agpu_immediate_renderer* immediate_renderer, agpu_buffer* index_buffer, agpu_size offset, agpu_size index_size )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererUseIndexBufferAt ( immediate_renderer, index_buffer, offset, index_size );
}

AGPU_EXPORT agpu_error agpuSetImmediateRendererCurrentMeshColors ( agpu_immediate_renderer* immediate_renderer, agpu_size stride, agpu_size elementCount, agpu_pointer colors )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuSetImmediateRendererCurrentMeshColors ( immediate_renderer, stride, elementCount, colors );
}

AGPU_EXPORT agpu_error agpuSetImmediateRendererCurrentMeshNormals ( agpu_immediate_renderer* immediate_renderer, agpu_size stride, agpu_size elementCount, agpu_pointer normals )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuSetImmediateRendererCurrentMeshNormals ( immediate_renderer, stride, elementCount, normals );
}

AGPU_EXPORT agpu_error agpuSetImmediateRendererCurrentMeshTexCoords ( agpu_immediate_renderer* immediate_renderer, agpu_size stride, agpu_size elementCount, agpu_pointer texcoords )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuSetImmediateRendererCurrentMeshTexCoords ( immediate_renderer, stride, elementCount, texcoords );
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetPrimitiveType ( agpu_immediate_renderer* immediate_renderer, agpu_primitive_topology type )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererSetPrimitiveType ( immediate_renderer, type );
}

AGPU_EXPORT agpu_error agpuImmediateRendererDrawArrays ( agpu_immediate_renderer* immediate_renderer, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererDrawArrays ( immediate_renderer, vertex_count, instance_count, first_vertex, base_instance );
}

AGPU_EXPORT agpu_error agpuImmediateRendererDrawElements ( agpu_immediate_renderer* immediate_renderer, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererDrawElements ( immediate_renderer, index_count, instance_count, first_index, base_vertex, base_instance );
}

AGPU_EXPORT agpu_error agpuImmediateRendererDrawElementsWithIndices ( agpu_immediate_renderer* immediate_renderer, agpu_primitive_topology mode, agpu_pointer indices, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuImmediateRendererDrawElementsWithIndices ( immediate_renderer, mode, indices, index_count, instance_count, first_index, base_vertex, base_instance );
}

AGPU_EXPORT agpu_error agpuEndImmediateRendererMesh ( agpu_immediate_renderer* immediate_renderer )
{
	if (immediate_renderer == nullptr)
		return AGPU_NULL_POINTER;
	agpu_icd_dispatch **dispatchTable = reinterpret_cast<agpu_icd_dispatch**> (immediate_renderer);
	return (*dispatchTable)->agpuEndImmediateRendererMesh ( immediate_renderer );
}

