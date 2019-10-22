
#include "agpu_impl.hpp"

inline void* hideType(void *t)
{
	return t;
}

#define asRef(O, I) (*reinterpret_cast<agpu::ref<O> *> (hideType(&I)) )
#define asRefCounter(O, I) (reinterpret_cast<agpu::ref_counter<O> *> (I))

//==============================================================================
// platform C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_device* agpuOpenDevice(agpu_platform* self, agpu_device_open_info* openInfo)
{
	return reinterpret_cast<agpu_device*> (asRef(agpu::platform, self)->openDevice(openInfo));
}

AGPU_EXPORT agpu_cstring agpuGetPlatformName(agpu_platform* self)
{
	return asRef(agpu::platform, self)->getName();
}

AGPU_EXPORT agpu_size agpuGetPlatformGpuCount(agpu_platform* self)
{
	return asRef(agpu::platform, self)->getGpuCount();
}

AGPU_EXPORT agpu_cstring agpuGetPlatformGpuName(agpu_platform* self, agpu_size gpu_index)
{
	return asRef(agpu::platform, self)->getGpuName(gpu_index);
}

AGPU_EXPORT agpu_int agpuGetPlatformVersion(agpu_platform* self)
{
	return asRef(agpu::platform, self)->getVersion();
}

AGPU_EXPORT agpu_int agpuGetPlatformImplementationVersion(agpu_platform* self)
{
	return asRef(agpu::platform, self)->getImplementationVersion();
}

AGPU_EXPORT agpu_bool agpuPlatformHasRealMultithreading(agpu_platform* self)
{
	return asRef(agpu::platform, self)->hasRealMultithreading();
}

AGPU_EXPORT agpu_bool agpuIsNativePlatform(agpu_platform* self)
{
	return asRef(agpu::platform, self)->isNative();
}

AGPU_EXPORT agpu_bool agpuIsCrossPlatform(agpu_platform* self)
{
	return asRef(agpu::platform, self)->isCrossPlatform();
}

AGPU_EXPORT agpu_offline_shader_compiler* agpuCreateOfflineShaderCompiler(agpu_platform* self)
{
	return reinterpret_cast<agpu_offline_shader_compiler*> (asRef(agpu::platform, self)->createOfflineShaderCompiler());
}

//==============================================================================
// device C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddDeviceReference(agpu_device* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::device, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseDevice(agpu_device* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::device, self)->release();
}

AGPU_EXPORT agpu_command_queue* agpuGetDefaultCommandQueue(agpu_device* self)
{
	return reinterpret_cast<agpu_command_queue*> (asRef(agpu::device, self)->getDefaultCommandQueue());
}

AGPU_EXPORT agpu_swap_chain* agpuCreateSwapChain(agpu_device* self, agpu_command_queue* commandQueue, agpu_swap_chain_create_info* swapChainInfo)
{
	return reinterpret_cast<agpu_swap_chain*> (asRef(agpu::device, self)->createSwapChain(asRef(agpu::command_queue, commandQueue), swapChainInfo));
}

AGPU_EXPORT agpu_buffer* agpuCreateBuffer(agpu_device* self, agpu_buffer_description* description, agpu_pointer initial_data)
{
	return reinterpret_cast<agpu_buffer*> (asRef(agpu::device, self)->createBuffer(description, initial_data));
}

AGPU_EXPORT agpu_vertex_layout* agpuCreateVertexLayout(agpu_device* self)
{
	return reinterpret_cast<agpu_vertex_layout*> (asRef(agpu::device, self)->createVertexLayout());
}

AGPU_EXPORT agpu_vertex_binding* agpuCreateVertexBinding(agpu_device* self, agpu_vertex_layout* layout)
{
	return reinterpret_cast<agpu_vertex_binding*> (asRef(agpu::device, self)->createVertexBinding(asRef(agpu::vertex_layout, layout)));
}

AGPU_EXPORT agpu_shader* agpuCreateShader(agpu_device* self, agpu_shader_type type)
{
	return reinterpret_cast<agpu_shader*> (asRef(agpu::device, self)->createShader(type));
}

AGPU_EXPORT agpu_shader_signature_builder* agpuCreateShaderSignatureBuilder(agpu_device* self)
{
	return reinterpret_cast<agpu_shader_signature_builder*> (asRef(agpu::device, self)->createShaderSignatureBuilder());
}

AGPU_EXPORT agpu_pipeline_builder* agpuCreatePipelineBuilder(agpu_device* self)
{
	return reinterpret_cast<agpu_pipeline_builder*> (asRef(agpu::device, self)->createPipelineBuilder());
}

AGPU_EXPORT agpu_compute_pipeline_builder* agpuCreateComputePipelineBuilder(agpu_device* self)
{
	return reinterpret_cast<agpu_compute_pipeline_builder*> (asRef(agpu::device, self)->createComputePipelineBuilder());
}

AGPU_EXPORT agpu_command_allocator* agpuCreateCommandAllocator(agpu_device* self, agpu_command_list_type type, agpu_command_queue* queue)
{
	return reinterpret_cast<agpu_command_allocator*> (asRef(agpu::device, self)->createCommandAllocator(type, asRef(agpu::command_queue, queue)));
}

AGPU_EXPORT agpu_command_list* agpuCreateCommandList(agpu_device* self, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state)
{
	return reinterpret_cast<agpu_command_list*> (asRef(agpu::device, self)->createCommandList(type, asRef(agpu::command_allocator, allocator), asRef(agpu::pipeline_state, initial_pipeline_state)));
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredShaderLanguage(agpu_device* self)
{
	return asRef(agpu::device, self)->getPreferredShaderLanguage();
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredIntermediateShaderLanguage(agpu_device* self)
{
	return asRef(agpu::device, self)->getPreferredIntermediateShaderLanguage();
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredHighLevelShaderLanguage(agpu_device* self)
{
	return asRef(agpu::device, self)->getPreferredHighLevelShaderLanguage();
}

AGPU_EXPORT agpu_framebuffer* agpuCreateFrameBuffer(agpu_device* self, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view** colorViews, agpu_texture_view* depthStencilView)
{
	return reinterpret_cast<agpu_framebuffer*> (asRef(agpu::device, self)->createFrameBuffer(width, height, colorCount, reinterpret_cast<agpu::texture_view_ref*> (colorViews), asRef(agpu::texture_view, depthStencilView)));
}

AGPU_EXPORT agpu_renderpass* agpuCreateRenderPass(agpu_device* self, agpu_renderpass_description* description)
{
	return reinterpret_cast<agpu_renderpass*> (asRef(agpu::device, self)->createRenderPass(description));
}

AGPU_EXPORT agpu_texture* agpuCreateTexture(agpu_device* self, agpu_texture_description* description)
{
	return reinterpret_cast<agpu_texture*> (asRef(agpu::device, self)->createTexture(description));
}

AGPU_EXPORT agpu_sampler* agpuCreateSampler(agpu_device* self, agpu_sampler_description* description)
{
	return reinterpret_cast<agpu_sampler*> (asRef(agpu::device, self)->createSampler(description));
}

AGPU_EXPORT agpu_fence* agpuCreateFence(agpu_device* self)
{
	return reinterpret_cast<agpu_fence*> (asRef(agpu::device, self)->createFence());
}

AGPU_EXPORT agpu_int agpuGetMultiSampleQualityLevels(agpu_device* self, agpu_uint sample_count)
{
	return asRef(agpu::device, self)->getMultiSampleQualityLevels(sample_count);
}

AGPU_EXPORT agpu_bool agpuHasTopLeftNdcOrigin(agpu_device* self)
{
	return asRef(agpu::device, self)->hasTopLeftNdcOrigin();
}

AGPU_EXPORT agpu_bool agpuHasBottomLeftTextureCoordinates(agpu_device* self)
{
	return asRef(agpu::device, self)->hasBottomLeftTextureCoordinates();
}

AGPU_EXPORT agpu_bool agpuIsFeatureSupportedOnDevice(agpu_device* self, agpu_feature feature)
{
	return asRef(agpu::device, self)->isFeatureSupported(feature);
}

AGPU_EXPORT agpu_vr_system* agpuGetVRSystem(agpu_device* self)
{
	return reinterpret_cast<agpu_vr_system*> (asRef(agpu::device, self)->getVRSystem());
}

AGPU_EXPORT agpu_offline_shader_compiler* agpuCreateOfflineShaderCompilerForDevice(agpu_device* self)
{
	return reinterpret_cast<agpu_offline_shader_compiler*> (asRef(agpu::device, self)->createOfflineShaderCompiler());
}

AGPU_EXPORT agpu_state_tracker_cache* agpuCreateStateTrackerCache(agpu_device* self, agpu_command_queue* command_queue_family)
{
	return reinterpret_cast<agpu_state_tracker_cache*> (asRef(agpu::device, self)->createStateTrackerCache(asRef(agpu::command_queue, command_queue_family)));
}

AGPU_EXPORT agpu_error agpuFinishDeviceExecution(agpu_device* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::device, self)->finishExecution();
}

//==============================================================================
// vr_system C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddVRSystemReference(agpu_vr_system* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::vr_system, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseVRSystem(agpu_vr_system* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::vr_system, self)->release();
}

AGPU_EXPORT agpu_cstring agpuGetVRSystemName(agpu_vr_system* self)
{
	return asRef(agpu::vr_system, self)->getVRSystemName();
}

AGPU_EXPORT agpu_pointer agpuGetVRSystemNativeHandle(agpu_vr_system* self)
{
	return asRef(agpu::vr_system, self)->getNativeHandle();
}

AGPU_EXPORT agpu_error agpuGetVRRecommendedRenderTargetSize(agpu_vr_system* self, agpu_size2d* size)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::vr_system, self)->getRecommendedRenderTargetSize(size);
}

AGPU_EXPORT agpu_error agpuGetVREyeToHeadTransformInto(agpu_vr_system* self, agpu_vr_eye eye, agpu_matrix4x4f* transform)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::vr_system, self)->getEyeToHeadTransform(eye, transform);
}

AGPU_EXPORT agpu_error agpuGetVRProjectionMatrix(agpu_vr_system* self, agpu_vr_eye eye, agpu_float near_distance, agpu_float far_distance, agpu_matrix4x4f* projection_matrix)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::vr_system, self)->getProjectionMatrix(eye, near_distance, far_distance, projection_matrix);
}

AGPU_EXPORT agpu_error agpuGetVRProjectionFrustumTangents(agpu_vr_system* self, agpu_vr_eye eye, agpu_frustum_tangents* frustum)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::vr_system, self)->getProjectionFrustumTangents(eye, frustum);
}

AGPU_EXPORT agpu_error agpuSubmitVREyeRenderTargets(agpu_vr_system* self, agpu_texture* left_eye, agpu_texture* right_eye)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::vr_system, self)->submitEyeRenderTargets(asRef(agpu::texture, left_eye), asRef(agpu::texture, right_eye));
}

AGPU_EXPORT agpu_error agpuWaitAndFetchVRPoses(agpu_vr_system* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::vr_system, self)->waitAndFetchPoses();
}

AGPU_EXPORT agpu_size agpuGetValidVRTrackedDevicePoseCount(agpu_vr_system* self)
{
	return asRef(agpu::vr_system, self)->getValidTrackedDevicePoseCount();
}

AGPU_EXPORT agpu_error agpuGetValidVRTrackedDevicePoseInto(agpu_vr_system* self, agpu_size index, agpu_vr_tracked_device_pose* dest)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::vr_system, self)->getValidTrackedDevicePoseInto(index, dest);
}

AGPU_EXPORT agpu_size agpuGetValidVRRenderTrackedDevicePoseCount(agpu_vr_system* self)
{
	return asRef(agpu::vr_system, self)->getValidRenderTrackedDevicePoseCount();
}

AGPU_EXPORT agpu_error agpuGetValidVRRenderTrackedDevicePoseInto(agpu_vr_system* self, agpu_size index, agpu_vr_tracked_device_pose* dest)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::vr_system, self)->getValidRenderTrackedDevicePoseInto(index, dest);
}

AGPU_EXPORT agpu_bool agpuPollVREvent(agpu_vr_system* self, agpu_vr_event* event)
{
	return asRef(agpu::vr_system, self)->pollEvent(event);
}

//==============================================================================
// swap_chain C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddSwapChainReference(agpu_swap_chain* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::swap_chain, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseSwapChain(agpu_swap_chain* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::swap_chain, self)->release();
}

AGPU_EXPORT agpu_error agpuSwapBuffers(agpu_swap_chain* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::swap_chain, self)->swapBuffers();
}

AGPU_EXPORT agpu_framebuffer* agpuGetCurrentBackBuffer(agpu_swap_chain* self)
{
	return reinterpret_cast<agpu_framebuffer*> (asRef(agpu::swap_chain, self)->getCurrentBackBuffer());
}

AGPU_EXPORT agpu_size agpuGetCurrentBackBufferIndex(agpu_swap_chain* self)
{
	return asRef(agpu::swap_chain, self)->getCurrentBackBufferIndex();
}

AGPU_EXPORT agpu_size agpuGetFramebufferCount(agpu_swap_chain* self)
{
	return asRef(agpu::swap_chain, self)->getFramebufferCount();
}

AGPU_EXPORT agpu_error agpuSetSwapChainOverlayPosition(agpu_swap_chain* self, agpu_int x, agpu_int y)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::swap_chain, self)->setOverlayPosition(x, y);
}

//==============================================================================
// compute_pipeline_builder C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddComputePipelineBuilderReference(agpu_compute_pipeline_builder* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::compute_pipeline_builder, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseComputePipelineBuilder(agpu_compute_pipeline_builder* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::compute_pipeline_builder, self)->release();
}

AGPU_EXPORT agpu_pipeline_state* agpuBuildComputePipelineState(agpu_compute_pipeline_builder* self)
{
	return reinterpret_cast<agpu_pipeline_state*> (asRef(agpu::compute_pipeline_builder, self)->build());
}

AGPU_EXPORT agpu_error agpuAttachComputeShader(agpu_compute_pipeline_builder* self, agpu_shader* shader)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::compute_pipeline_builder, self)->attachShader(asRef(agpu::shader, shader));
}

AGPU_EXPORT agpu_error agpuAttachComputeShaderWithEntryPoint(agpu_compute_pipeline_builder* self, agpu_shader* shader, agpu_shader_type type, agpu_cstring entry_point)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::compute_pipeline_builder, self)->attachShaderWithEntryPoint(asRef(agpu::shader, shader), type, entry_point);
}

AGPU_EXPORT agpu_size agpuGetComputePipelineBuildingLogLength(agpu_compute_pipeline_builder* self)
{
	return asRef(agpu::compute_pipeline_builder, self)->getBuildingLogLength();
}

AGPU_EXPORT agpu_error agpuGetComputePipelineBuildingLog(agpu_compute_pipeline_builder* self, agpu_size buffer_size, agpu_string_buffer buffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::compute_pipeline_builder, self)->getBuildingLog(buffer_size, buffer);
}

AGPU_EXPORT agpu_error agpuSetComputePipelineShaderSignature(agpu_compute_pipeline_builder* self, agpu_shader_signature* signature)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::compute_pipeline_builder, self)->setShaderSignature(asRef(agpu::shader_signature, signature));
}

//==============================================================================
// pipeline_builder C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddPipelineBuilderReference(agpu_pipeline_builder* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::pipeline_builder, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleasePipelineBuilder(agpu_pipeline_builder* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::pipeline_builder, self)->release();
}

AGPU_EXPORT agpu_pipeline_state* agpuBuildPipelineState(agpu_pipeline_builder* self)
{
	return reinterpret_cast<agpu_pipeline_state*> (asRef(agpu::pipeline_builder, self)->build());
}

AGPU_EXPORT agpu_error agpuAttachShader(agpu_pipeline_builder* self, agpu_shader* shader)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->attachShader(asRef(agpu::shader, shader));
}

AGPU_EXPORT agpu_error agpuAttachShaderWithEntryPoint(agpu_pipeline_builder* self, agpu_shader* shader, agpu_shader_type type, agpu_cstring entry_point)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->attachShaderWithEntryPoint(asRef(agpu::shader, shader), type, entry_point);
}

AGPU_EXPORT agpu_size agpuGetPipelineBuildingLogLength(agpu_pipeline_builder* self)
{
	return asRef(agpu::pipeline_builder, self)->getBuildingLogLength();
}

AGPU_EXPORT agpu_error agpuGetPipelineBuildingLog(agpu_pipeline_builder* self, agpu_size buffer_size, agpu_string_buffer buffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->getBuildingLog(buffer_size, buffer);
}

AGPU_EXPORT agpu_error agpuSetBlendState(agpu_pipeline_builder* self, agpu_int renderTargetMask, agpu_bool enabled)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setBlendState(renderTargetMask, enabled);
}

AGPU_EXPORT agpu_error agpuSetBlendFunction(agpu_pipeline_builder* self, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setBlendFunction(renderTargetMask, sourceFactor, destFactor, colorOperation, sourceAlphaFactor, destAlphaFactor, alphaOperation);
}

AGPU_EXPORT agpu_error agpuSetColorMask(agpu_pipeline_builder* self, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setColorMask(renderTargetMask, redEnabled, greenEnabled, blueEnabled, alphaEnabled);
}

AGPU_EXPORT agpu_error agpuSetFrontFace(agpu_pipeline_builder* self, agpu_face_winding winding)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setFrontFace(winding);
}

AGPU_EXPORT agpu_error agpuSetCullMode(agpu_pipeline_builder* self, agpu_cull_mode mode)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setCullMode(mode);
}

AGPU_EXPORT agpu_error agpuSetDepthBias(agpu_pipeline_builder* self, agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setDepthBias(constant_factor, clamp, slope_factor);
}

AGPU_EXPORT agpu_error agpuSetDepthState(agpu_pipeline_builder* self, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setDepthState(enabled, writeMask, function);
}

AGPU_EXPORT agpu_error agpuSetPolygonMode(agpu_pipeline_builder* self, agpu_polygon_mode mode)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setPolygonMode(mode);
}

AGPU_EXPORT agpu_error agpuSetStencilState(agpu_pipeline_builder* self, agpu_bool enabled, agpu_int writeMask, agpu_int readMask)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setStencilState(enabled, writeMask, readMask);
}

AGPU_EXPORT agpu_error agpuSetStencilFrontFace(agpu_pipeline_builder* self, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setStencilFrontFace(stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction);
}

AGPU_EXPORT agpu_error agpuSetStencilBackFace(agpu_pipeline_builder* self, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setStencilBackFace(stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction);
}

AGPU_EXPORT agpu_error agpuSetRenderTargetCount(agpu_pipeline_builder* self, agpu_int count)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setRenderTargetCount(count);
}

AGPU_EXPORT agpu_error agpuSetRenderTargetFormat(agpu_pipeline_builder* self, agpu_uint index, agpu_texture_format format)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setRenderTargetFormat(index, format);
}

AGPU_EXPORT agpu_error agpuSetDepthStencilFormat(agpu_pipeline_builder* self, agpu_texture_format format)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setDepthStencilFormat(format);
}

AGPU_EXPORT agpu_error agpuSetPrimitiveType(agpu_pipeline_builder* self, agpu_primitive_topology type)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setPrimitiveType(type);
}

AGPU_EXPORT agpu_error agpuSetVertexLayout(agpu_pipeline_builder* self, agpu_vertex_layout* layout)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setVertexLayout(asRef(agpu::vertex_layout, layout));
}

AGPU_EXPORT agpu_error agpuSetPipelineShaderSignature(agpu_pipeline_builder* self, agpu_shader_signature* signature)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setShaderSignature(asRef(agpu::shader_signature, signature));
}

AGPU_EXPORT agpu_error agpuSetSampleDescription(agpu_pipeline_builder* self, agpu_uint sample_count, agpu_uint sample_quality)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::pipeline_builder, self)->setSampleDescription(sample_count, sample_quality);
}

//==============================================================================
// pipeline_state C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddPipelineStateReference(agpu_pipeline_state* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::pipeline_state, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleasePipelineState(agpu_pipeline_state* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::pipeline_state, self)->release();
}

//==============================================================================
// command_queue C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddCommandQueueReference(agpu_command_queue* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::command_queue, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseCommandQueue(agpu_command_queue* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::command_queue, self)->release();
}

AGPU_EXPORT agpu_error agpuAddCommandList(agpu_command_queue* self, agpu_command_list* command_list)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_queue, self)->addCommandList(asRef(agpu::command_list, command_list));
}

AGPU_EXPORT agpu_error agpuFinishQueueExecution(agpu_command_queue* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_queue, self)->finishExecution();
}

AGPU_EXPORT agpu_error agpuSignalFence(agpu_command_queue* self, agpu_fence* fence)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_queue, self)->signalFence(asRef(agpu::fence, fence));
}

AGPU_EXPORT agpu_error agpuWaitFence(agpu_command_queue* self, agpu_fence* fence)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_queue, self)->waitFence(asRef(agpu::fence, fence));
}

//==============================================================================
// command_allocator C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddCommandAllocatorReference(agpu_command_allocator* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::command_allocator, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseCommandAllocator(agpu_command_allocator* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::command_allocator, self)->release();
}

AGPU_EXPORT agpu_error agpuResetCommandAllocator(agpu_command_allocator* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_allocator, self)->reset();
}

//==============================================================================
// command_list C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddCommandListReference(agpu_command_list* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::command_list, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseCommandList(agpu_command_list* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::command_list, self)->release();
}

AGPU_EXPORT agpu_error agpuSetShaderSignature(agpu_command_list* self, agpu_shader_signature* signature)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->setShaderSignature(asRef(agpu::shader_signature, signature));
}

AGPU_EXPORT agpu_error agpuSetViewport(agpu_command_list* self, agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->setViewport(x, y, w, h);
}

AGPU_EXPORT agpu_error agpuSetScissor(agpu_command_list* self, agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->setScissor(x, y, w, h);
}

AGPU_EXPORT agpu_error agpuUsePipelineState(agpu_command_list* self, agpu_pipeline_state* pipeline)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->usePipelineState(asRef(agpu::pipeline_state, pipeline));
}

AGPU_EXPORT agpu_error agpuUseVertexBinding(agpu_command_list* self, agpu_vertex_binding* vertex_binding)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->useVertexBinding(asRef(agpu::vertex_binding, vertex_binding));
}

AGPU_EXPORT agpu_error agpuUseIndexBuffer(agpu_command_list* self, agpu_buffer* index_buffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->useIndexBuffer(asRef(agpu::buffer, index_buffer));
}

AGPU_EXPORT agpu_error agpuUseIndexBufferAt(agpu_command_list* self, agpu_buffer* index_buffer, agpu_size offset, agpu_size index_size)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->useIndexBufferAt(asRef(agpu::buffer, index_buffer), offset, index_size);
}

AGPU_EXPORT agpu_error agpuUseDrawIndirectBuffer(agpu_command_list* self, agpu_buffer* draw_buffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->useDrawIndirectBuffer(asRef(agpu::buffer, draw_buffer));
}

AGPU_EXPORT agpu_error agpuUseComputeDispatchIndirectBuffer(agpu_command_list* self, agpu_buffer* buffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->useComputeDispatchIndirectBuffer(asRef(agpu::buffer, buffer));
}

AGPU_EXPORT agpu_error agpuUseShaderResources(agpu_command_list* self, agpu_shader_resource_binding* binding)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->useShaderResources(asRef(agpu::shader_resource_binding, binding));
}

AGPU_EXPORT agpu_error agpuUseComputeShaderResources(agpu_command_list* self, agpu_shader_resource_binding* binding)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->useComputeShaderResources(asRef(agpu::shader_resource_binding, binding));
}

AGPU_EXPORT agpu_error agpuDrawArrays(agpu_command_list* self, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->drawArrays(vertex_count, instance_count, first_vertex, base_instance);
}

AGPU_EXPORT agpu_error agpuDrawArraysIndirect(agpu_command_list* self, agpu_size offset, agpu_size drawcount)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->drawArraysIndirect(offset, drawcount);
}

AGPU_EXPORT agpu_error agpuDrawElements(agpu_command_list* self, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->drawElements(index_count, instance_count, first_index, base_vertex, base_instance);
}

AGPU_EXPORT agpu_error agpuDrawElementsIndirect(agpu_command_list* self, agpu_size offset, agpu_size drawcount)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->drawElementsIndirect(offset, drawcount);
}

AGPU_EXPORT agpu_error agpuDispatchCompute(agpu_command_list* self, agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->dispatchCompute(group_count_x, group_count_y, group_count_z);
}

AGPU_EXPORT agpu_error agpuDispatchComputeIndirect(agpu_command_list* self, agpu_size offset)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->dispatchComputeIndirect(offset);
}

AGPU_EXPORT agpu_error agpuSetStencilReference(agpu_command_list* self, agpu_uint reference)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->setStencilReference(reference);
}

AGPU_EXPORT agpu_error agpuExecuteBundle(agpu_command_list* self, agpu_command_list* bundle)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->executeBundle(asRef(agpu::command_list, bundle));
}

AGPU_EXPORT agpu_error agpuCloseCommandList(agpu_command_list* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->close();
}

AGPU_EXPORT agpu_error agpuResetCommandList(agpu_command_list* self, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->reset(asRef(agpu::command_allocator, allocator), asRef(agpu::pipeline_state, initial_pipeline_state));
}

AGPU_EXPORT agpu_error agpuResetBundleCommandList(agpu_command_list* self, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state, agpu_inheritance_info* inheritance_info)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->resetBundle(asRef(agpu::command_allocator, allocator), asRef(agpu::pipeline_state, initial_pipeline_state), inheritance_info);
}

AGPU_EXPORT agpu_error agpuBeginRenderPass(agpu_command_list* self, agpu_renderpass* renderpass, agpu_framebuffer* framebuffer, agpu_bool bundle_content)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->beginRenderPass(asRef(agpu::renderpass, renderpass), asRef(agpu::framebuffer, framebuffer), bundle_content);
}

AGPU_EXPORT agpu_error agpuEndRenderPass(agpu_command_list* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->endRenderPass();
}

AGPU_EXPORT agpu_error agpuResolveFramebuffer(agpu_command_list* self, agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->resolveFramebuffer(asRef(agpu::framebuffer, destFramebuffer), asRef(agpu::framebuffer, sourceFramebuffer));
}

AGPU_EXPORT agpu_error agpuResolveTexture(agpu_command_list* self, agpu_texture* sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, agpu_texture* destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->resolveTexture(asRef(agpu::texture, sourceTexture), sourceLevel, sourceLayer, asRef(agpu::texture, destTexture), destLevel, destLayer, levelCount, layerCount, aspect);
}

AGPU_EXPORT agpu_error agpuPushConstants(agpu_command_list* self, agpu_uint offset, agpu_uint size, agpu_pointer values)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->pushConstants(offset, size, values);
}

AGPU_EXPORT agpu_error agpuMemoryBarrier(agpu_command_list* self, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->memoryBarrier(source_stage, dest_stage, source_accesses, dest_accesses);
}

AGPU_EXPORT agpu_error agpuBufferMemoryBarrier(agpu_command_list* self, agpu_buffer* buffer, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_size offset, agpu_size size)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->bufferMemoryBarrier(asRef(agpu::buffer, buffer), source_stage, dest_stage, source_accesses, dest_accesses, offset, size);
}

AGPU_EXPORT agpu_error agpuTextureMemoryBarrier(agpu_command_list* self, agpu_texture* texture, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_subresource_range* subresource_range)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->textureMemoryBarrier(asRef(agpu::texture, texture), source_stage, dest_stage, source_accesses, dest_accesses, subresource_range);
}

AGPU_EXPORT agpu_error agpuPushBufferTransitionBarrier(agpu_command_list* self, agpu_buffer* buffer, agpu_buffer_usage_mask new_usage)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->pushBufferTransitionBarrier(asRef(agpu::buffer, buffer), new_usage);
}

AGPU_EXPORT agpu_error agpuPushTextureTransitionBarrier(agpu_command_list* self, agpu_texture* texture, agpu_texture_usage_mode_mask new_usage, agpu_subresource_range* subresource_range)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->pushTextureTransitionBarrier(asRef(agpu::texture, texture), new_usage, subresource_range);
}

AGPU_EXPORT agpu_error agpuPopBufferTransitionBarrier(agpu_command_list* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->popBufferTransitionBarrier();
}

AGPU_EXPORT agpu_error agpuPopTextureTransitionBarrier(agpu_command_list* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->popTextureTransitionBarrier();
}

AGPU_EXPORT agpu_error agpuCopyBuffer(agpu_command_list* self, agpu_buffer* source_buffer, agpu_size source_offset, agpu_buffer* dest_buffer, agpu_size dest_offset, agpu_size copy_size)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->copyBuffer(asRef(agpu::buffer, source_buffer), source_offset, asRef(agpu::buffer, dest_buffer), dest_offset, copy_size);
}

AGPU_EXPORT agpu_error agpuCopyBufferToTexture(agpu_command_list* self, agpu_buffer* buffer, agpu_texture* texture, agpu_buffer_image_copy_region* copy_region)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->copyBufferToTexture(asRef(agpu::buffer, buffer), asRef(agpu::texture, texture), copy_region);
}

AGPU_EXPORT agpu_error agpuCopyTextureToBuffer(agpu_command_list* self, agpu_texture* texture, agpu_buffer* buffer, agpu_buffer_image_copy_region* copy_region)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::command_list, self)->copyTextureToBuffer(asRef(agpu::texture, texture), asRef(agpu::buffer, buffer), copy_region);
}

//==============================================================================
// texture C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddTextureReference(agpu_texture* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::texture, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseTexture(agpu_texture* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::texture, self)->release();
}

AGPU_EXPORT agpu_error agpuGetTextureDescription(agpu_texture* self, agpu_texture_description* description)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::texture, self)->getDescription(description);
}

AGPU_EXPORT agpu_pointer agpuMapTextureLevel(agpu_texture* self, agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags, agpu_region3d* region)
{
	return asRef(agpu::texture, self)->mapLevel(level, arrayIndex, flags, region);
}

AGPU_EXPORT agpu_error agpuUnmapTextureLevel(agpu_texture* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::texture, self)->unmapLevel();
}

AGPU_EXPORT agpu_error agpuReadTextureData(agpu_texture* self, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::texture, self)->readTextureData(level, arrayIndex, pitch, slicePitch, buffer);
}

AGPU_EXPORT agpu_error agpuReadTextureSubData(agpu_texture* self, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_region3d* sourceRegion, agpu_size3d* destSize, agpu_pointer buffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::texture, self)->readTextureSubData(level, arrayIndex, pitch, slicePitch, sourceRegion, destSize, buffer);
}

AGPU_EXPORT agpu_error agpuUploadTextureData(agpu_texture* self, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::texture, self)->uploadTextureData(level, arrayIndex, pitch, slicePitch, data);
}

AGPU_EXPORT agpu_error agpuUploadTextureSubData(agpu_texture* self, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_size3d* sourceSize, agpu_region3d* destRegion, agpu_pointer data)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::texture, self)->uploadTextureSubData(level, arrayIndex, pitch, slicePitch, sourceSize, destRegion, data);
}

AGPU_EXPORT agpu_error agpuGetTextureFullViewDescription(agpu_texture* self, agpu_texture_view_description* result)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::texture, self)->getFullViewDescription(result);
}

AGPU_EXPORT agpu_texture_view* agpuCreateTextureView(agpu_texture* self, agpu_texture_view_description* description)
{
	return reinterpret_cast<agpu_texture_view*> (asRef(agpu::texture, self)->createView(description));
}

AGPU_EXPORT agpu_texture_view* agpuGetOrCreateFullTextureView(agpu_texture* self)
{
	return reinterpret_cast<agpu_texture_view*> (asRef(agpu::texture, self)->getOrCreateFullView());
}

//==============================================================================
// texture_view C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddTextureViewReference(agpu_texture_view* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::texture_view, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseTextureView(agpu_texture_view* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::texture_view, self)->release();
}

AGPU_EXPORT agpu_texture* agpuGetTextureFromView(agpu_texture_view* self)
{
	return reinterpret_cast<agpu_texture*> (asRef(agpu::texture_view, self)->getTexture());
}

//==============================================================================
// sampler C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddSamplerReference(agpu_sampler* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::sampler, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseSampler(agpu_sampler* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::sampler, self)->release();
}

//==============================================================================
// buffer C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddBufferReference(agpu_buffer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::buffer, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseBuffer(agpu_buffer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::buffer, self)->release();
}

AGPU_EXPORT agpu_pointer agpuMapBuffer(agpu_buffer* self, agpu_mapping_access flags)
{
	return asRef(agpu::buffer, self)->mapBuffer(flags);
}

AGPU_EXPORT agpu_error agpuUnmapBuffer(agpu_buffer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::buffer, self)->unmapBuffer();
}

AGPU_EXPORT agpu_error agpuGetBufferDescription(agpu_buffer* self, agpu_buffer_description* description)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::buffer, self)->getDescription(description);
}

AGPU_EXPORT agpu_error agpuUploadBufferData(agpu_buffer* self, agpu_size offset, agpu_size size, agpu_pointer data)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::buffer, self)->uploadBufferData(offset, size, data);
}

AGPU_EXPORT agpu_error agpuReadBufferData(agpu_buffer* self, agpu_size offset, agpu_size size, agpu_pointer data)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::buffer, self)->readBufferData(offset, size, data);
}

AGPU_EXPORT agpu_error agpuFlushWholeBuffer(agpu_buffer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::buffer, self)->flushWholeBuffer();
}

AGPU_EXPORT agpu_error agpuInvalidateWholeBuffer(agpu_buffer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::buffer, self)->invalidateWholeBuffer();
}

//==============================================================================
// vertex_binding C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddVertexBindingReference(agpu_vertex_binding* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::vertex_binding, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseVertexBinding(agpu_vertex_binding* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::vertex_binding, self)->release();
}

AGPU_EXPORT agpu_error agpuBindVertexBuffers(agpu_vertex_binding* self, agpu_uint count, agpu_buffer** vertex_buffers)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::vertex_binding, self)->bindVertexBuffers(count, reinterpret_cast<agpu::buffer_ref*> (vertex_buffers));
}

AGPU_EXPORT agpu_error agpuBindVertexBuffersWithOffsets(agpu_vertex_binding* self, agpu_uint count, agpu_buffer** vertex_buffers, agpu_size* offsets)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::vertex_binding, self)->bindVertexBuffersWithOffsets(count, reinterpret_cast<agpu::buffer_ref*> (vertex_buffers), offsets);
}

//==============================================================================
// vertex_layout C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddVertexLayoutReference(agpu_vertex_layout* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::vertex_layout, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseVertexLayout(agpu_vertex_layout* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::vertex_layout, self)->release();
}

AGPU_EXPORT agpu_error agpuAddVertexAttributeBindings(agpu_vertex_layout* self, agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::vertex_layout, self)->addVertexAttributeBindings(vertex_buffer_count, vertex_strides, attribute_count, attributes);
}

//==============================================================================
// shader C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddShaderReference(agpu_shader* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::shader, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseShader(agpu_shader* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::shader, self)->release();
}

AGPU_EXPORT agpu_error agpuSetShaderSource(agpu_shader* self, agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader, self)->setShaderSource(language, sourceText, sourceTextLength);
}

AGPU_EXPORT agpu_error agpuCompileShader(agpu_shader* self, agpu_cstring options)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader, self)->compileShader(options);
}

AGPU_EXPORT agpu_size agpuGetShaderCompilationLogLength(agpu_shader* self)
{
	return asRef(agpu::shader, self)->getCompilationLogLength();
}

AGPU_EXPORT agpu_error agpuGetShaderCompilationLog(agpu_shader* self, agpu_size buffer_size, agpu_string_buffer buffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader, self)->getCompilationLog(buffer_size, buffer);
}

//==============================================================================
// framebuffer C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddFramebufferReference(agpu_framebuffer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::framebuffer, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseFramebuffer(agpu_framebuffer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::framebuffer, self)->release();
}

//==============================================================================
// renderpass C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddRenderPassReference(agpu_renderpass* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::renderpass, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseRenderPass(agpu_renderpass* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::renderpass, self)->release();
}

AGPU_EXPORT agpu_error agpuSetDepthStencilClearValue(agpu_renderpass* self, agpu_depth_stencil_value value)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::renderpass, self)->setDepthStencilClearValue(value);
}

AGPU_EXPORT agpu_error agpuSetColorClearValue(agpu_renderpass* self, agpu_uint attachment_index, agpu_color4f value)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::renderpass, self)->setColorClearValue(attachment_index, value);
}

AGPU_EXPORT agpu_error agpuSetColorClearValueFrom(agpu_renderpass* self, agpu_uint attachment_index, agpu_color4f* value)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::renderpass, self)->setColorClearValueFrom(attachment_index, value);
}

AGPU_EXPORT agpu_error agpuGetRenderPassColorAttachmentFormats(agpu_renderpass* self, agpu_uint* color_attachment_count, agpu_texture_format* formats)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::renderpass, self)->getColorAttachmentFormats(color_attachment_count, formats);
}

AGPU_EXPORT agpu_texture_format agpuGetRenderPassDepthStencilAttachmentFormat(agpu_renderpass* self)
{
	return asRef(agpu::renderpass, self)->getDepthStencilAttachmentFormat();
}

AGPU_EXPORT agpu_uint agpuGetRenderPassSampleCount(agpu_renderpass* self)
{
	return asRef(agpu::renderpass, self)->getSampleCount();
}

AGPU_EXPORT agpu_uint agpuGetRenderPassSampleQuality(agpu_renderpass* self)
{
	return asRef(agpu::renderpass, self)->getSampleQuality();
}

//==============================================================================
// shader_signature_builder C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddShaderSignatureBuilderReference(agpu_shader_signature_builder* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::shader_signature_builder, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseShaderSignatureBuilder(agpu_shader_signature_builder* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::shader_signature_builder, self)->release();
}

AGPU_EXPORT agpu_shader_signature* agpuBuildShaderSignature(agpu_shader_signature_builder* self)
{
	return reinterpret_cast<agpu_shader_signature*> (asRef(agpu::shader_signature_builder, self)->build());
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingConstant(agpu_shader_signature_builder* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader_signature_builder, self)->addBindingConstant();
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingElement(agpu_shader_signature_builder* self, agpu_shader_binding_type type, agpu_uint maxBindings)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader_signature_builder, self)->addBindingElement(type, maxBindings);
}

AGPU_EXPORT agpu_error agpuBeginShaderSignatureBindingBank(agpu_shader_signature_builder* self, agpu_uint maxBindings)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader_signature_builder, self)->beginBindingBank(maxBindings);
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingBankElement(agpu_shader_signature_builder* self, agpu_shader_binding_type type, agpu_uint bindingPointCount)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader_signature_builder, self)->addBindingBankElement(type, bindingPointCount);
}

//==============================================================================
// shader_signature C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddShaderSignature(agpu_shader_signature* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::shader_signature, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseShaderSignature(agpu_shader_signature* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::shader_signature, self)->release();
}

AGPU_EXPORT agpu_shader_resource_binding* agpuCreateShaderResourceBinding(agpu_shader_signature* self, agpu_uint element)
{
	return reinterpret_cast<agpu_shader_resource_binding*> (asRef(agpu::shader_signature, self)->createShaderResourceBinding(element));
}

//==============================================================================
// shader_resource_binding C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddShaderResourceBindingReference(agpu_shader_resource_binding* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::shader_resource_binding, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseShaderResourceBinding(agpu_shader_resource_binding* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::shader_resource_binding, self)->release();
}

AGPU_EXPORT agpu_error agpuBindUniformBuffer(agpu_shader_resource_binding* self, agpu_int location, agpu_buffer* uniform_buffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader_resource_binding, self)->bindUniformBuffer(location, asRef(agpu::buffer, uniform_buffer));
}

AGPU_EXPORT agpu_error agpuBindUniformBufferRange(agpu_shader_resource_binding* self, agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader_resource_binding, self)->bindUniformBufferRange(location, asRef(agpu::buffer, uniform_buffer), offset, size);
}

AGPU_EXPORT agpu_error agpuBindStorageBuffer(agpu_shader_resource_binding* self, agpu_int location, agpu_buffer* storage_buffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader_resource_binding, self)->bindStorageBuffer(location, asRef(agpu::buffer, storage_buffer));
}

AGPU_EXPORT agpu_error agpuBindStorageBufferRange(agpu_shader_resource_binding* self, agpu_int location, agpu_buffer* storage_buffer, agpu_size offset, agpu_size size)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader_resource_binding, self)->bindStorageBufferRange(location, asRef(agpu::buffer, storage_buffer), offset, size);
}

AGPU_EXPORT agpu_error agpuBindSampledTextureView(agpu_shader_resource_binding* self, agpu_int location, agpu_texture_view* view)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader_resource_binding, self)->bindSampledTextureView(location, asRef(agpu::texture_view, view));
}

AGPU_EXPORT agpu_error agpuBindStorageImageView(agpu_shader_resource_binding* self, agpu_int location, agpu_texture_view* view)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader_resource_binding, self)->bindStorageImageView(location, asRef(agpu::texture_view, view));
}

AGPU_EXPORT agpu_error agpuBindSampler(agpu_shader_resource_binding* self, agpu_int location, agpu_sampler* sampler)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader_resource_binding, self)->bindSampler(location, asRef(agpu::sampler, sampler));
}

//==============================================================================
// fence C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddFenceReference(agpu_fence* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::fence, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseFenceReference(agpu_fence* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::fence, self)->release();
}

AGPU_EXPORT agpu_error agpuWaitOnClient(agpu_fence* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::fence, self)->waitOnClient();
}

//==============================================================================
// offline_shader_compiler C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddOfflineShaderCompilerReference(agpu_offline_shader_compiler* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::offline_shader_compiler, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseOfflineShaderCompiler(agpu_offline_shader_compiler* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::offline_shader_compiler, self)->release();
}

AGPU_EXPORT agpu_bool agpuIsShaderLanguageSupportedByOfflineCompiler(agpu_offline_shader_compiler* self, agpu_shader_language language)
{
	return asRef(agpu::offline_shader_compiler, self)->isShaderLanguageSupported(language);
}

AGPU_EXPORT agpu_bool agpuIsTargetShaderLanguageSupportedByOfflineCompiler(agpu_offline_shader_compiler* self, agpu_shader_language language)
{
	return asRef(agpu::offline_shader_compiler, self)->isTargetShaderLanguageSupported(language);
}

AGPU_EXPORT agpu_error agpuSetOfflineShaderCompilerSource(agpu_offline_shader_compiler* self, agpu_shader_language language, agpu_shader_type stage, agpu_string sourceText, agpu_string_length sourceTextLength)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::offline_shader_compiler, self)->setShaderSource(language, stage, sourceText, sourceTextLength);
}

AGPU_EXPORT agpu_error agpuCompileOfflineShader(agpu_offline_shader_compiler* self, agpu_shader_language target_language, agpu_cstring options)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::offline_shader_compiler, self)->compileShader(target_language, options);
}

AGPU_EXPORT agpu_size agpuGetOfflineShaderCompilationLogLength(agpu_offline_shader_compiler* self)
{
	return asRef(agpu::offline_shader_compiler, self)->getCompilationLogLength();
}

AGPU_EXPORT agpu_error agpuGetOfflineShaderCompilationLog(agpu_offline_shader_compiler* self, agpu_size buffer_size, agpu_string_buffer buffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::offline_shader_compiler, self)->getCompilationLog(buffer_size, buffer);
}

AGPU_EXPORT agpu_size agpuGetOfflineShaderCompilationResultLength(agpu_offline_shader_compiler* self)
{
	return asRef(agpu::offline_shader_compiler, self)->getCompilationResultLength();
}

AGPU_EXPORT agpu_error agpuGetOfflineShaderCompilationResult(agpu_offline_shader_compiler* self, agpu_size buffer_size, agpu_string_buffer buffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::offline_shader_compiler, self)->getCompilationResult(buffer_size, buffer);
}

AGPU_EXPORT agpu_shader* agpuGetOfflineShaderCompilerResultAsShader(agpu_offline_shader_compiler* self)
{
	return reinterpret_cast<agpu_shader*> (asRef(agpu::offline_shader_compiler, self)->getResultAsShader());
}

//==============================================================================
// state_tracker_cache C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddStateTrackerCacheReference(agpu_state_tracker_cache* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::state_tracker_cache, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseStateTrackerCacheReference(agpu_state_tracker_cache* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::state_tracker_cache, self)->release();
}

AGPU_EXPORT agpu_state_tracker* agpuCreateStateTracker(agpu_state_tracker_cache* self, agpu_command_list_type type, agpu_command_queue* command_queue)
{
	return reinterpret_cast<agpu_state_tracker*> (asRef(agpu::state_tracker_cache, self)->createStateTracker(type, asRef(agpu::command_queue, command_queue)));
}

AGPU_EXPORT agpu_state_tracker* agpuCreateStateTrackerWithCommandAllocator(agpu_state_tracker_cache* self, agpu_command_list_type type, agpu_command_queue* command_queue, agpu_command_allocator* command_allocator)
{
	return reinterpret_cast<agpu_state_tracker*> (asRef(agpu::state_tracker_cache, self)->createStateTrackerWithCommandAllocator(type, asRef(agpu::command_queue, command_queue), asRef(agpu::command_allocator, command_allocator)));
}

AGPU_EXPORT agpu_state_tracker* agpuCreateStateTrackerWithFrameBuffering(agpu_state_tracker_cache* self, agpu_command_list_type type, agpu_command_queue* command_queue, agpu_uint framebuffering_count)
{
	return reinterpret_cast<agpu_state_tracker*> (asRef(agpu::state_tracker_cache, self)->createStateTrackerWithFrameBuffering(type, asRef(agpu::command_queue, command_queue), framebuffering_count));
}

AGPU_EXPORT agpu_immediate_renderer* agpuCreateImmediateRenderer(agpu_state_tracker_cache* self)
{
	return reinterpret_cast<agpu_immediate_renderer*> (asRef(agpu::state_tracker_cache, self)->createImmediateRenderer());
}

//==============================================================================
// state_tracker C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddStateTrackerReference(agpu_state_tracker* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::state_tracker, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseStateTrackerReference(agpu_state_tracker* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::state_tracker, self)->release();
}

AGPU_EXPORT agpu_error agpuStateTrackerBeginRecordingCommands(agpu_state_tracker* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->beginRecordingCommands();
}

AGPU_EXPORT agpu_command_list* agpuStateTrackerEndRecordingCommands(agpu_state_tracker* self)
{
	return reinterpret_cast<agpu_command_list*> (asRef(agpu::state_tracker, self)->endRecordingCommands());
}

AGPU_EXPORT agpu_error agpuStateTrackerEndRecordingAndFlushCommands(agpu_state_tracker* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->endRecordingAndFlushCommands();
}

AGPU_EXPORT agpu_error agpuStateTrackerReset(agpu_state_tracker* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->reset();
}

AGPU_EXPORT agpu_error agpuStateTrackerResetGraphicsPipeline(agpu_state_tracker* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->resetGraphicsPipeline();
}

AGPU_EXPORT agpu_error agpuStateTrackerResetComputePipeline(agpu_state_tracker* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->resetComputePipeline();
}

AGPU_EXPORT agpu_error agpuStateTrackerSetComputeStage(agpu_state_tracker* self, agpu_shader* shader, agpu_cstring entryPoint)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setComputeStage(asRef(agpu::shader, shader), entryPoint);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetVertexStage(agpu_state_tracker* self, agpu_shader* shader, agpu_cstring entryPoint)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setVertexStage(asRef(agpu::shader, shader), entryPoint);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetFragmentStage(agpu_state_tracker* self, agpu_shader* shader, agpu_cstring entryPoint)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setFragmentStage(asRef(agpu::shader, shader), entryPoint);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetGeometryStage(agpu_state_tracker* self, agpu_shader* shader, agpu_cstring entryPoint)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setGeometryStage(asRef(agpu::shader, shader), entryPoint);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetTessellationControlStage(agpu_state_tracker* self, agpu_shader* shader, agpu_cstring entryPoint)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setTessellationControlStage(asRef(agpu::shader, shader), entryPoint);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetTessellationEvaluationStage(agpu_state_tracker* self, agpu_shader* shader, agpu_cstring entryPoint)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setTessellationEvaluationStage(asRef(agpu::shader, shader), entryPoint);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetBlendState(agpu_state_tracker* self, agpu_int renderTargetMask, agpu_bool enabled)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setBlendState(renderTargetMask, enabled);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetBlendFunction(agpu_state_tracker* self, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setBlendFunction(renderTargetMask, sourceFactor, destFactor, colorOperation, sourceAlphaFactor, destAlphaFactor, alphaOperation);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetColorMask(agpu_state_tracker* self, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setColorMask(renderTargetMask, redEnabled, greenEnabled, blueEnabled, alphaEnabled);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetFrontFace(agpu_state_tracker* self, agpu_face_winding winding)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setFrontFace(winding);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetCullMode(agpu_state_tracker* self, agpu_cull_mode mode)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setCullMode(mode);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetDepthBias(agpu_state_tracker* self, agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setDepthBias(constant_factor, clamp, slope_factor);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetDepthState(agpu_state_tracker* self, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setDepthState(enabled, writeMask, function);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetPolygonMode(agpu_state_tracker* self, agpu_polygon_mode mode)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setPolygonMode(mode);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetStencilState(agpu_state_tracker* self, agpu_bool enabled, agpu_int writeMask, agpu_int readMask)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setStencilState(enabled, writeMask, readMask);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetStencilFrontFace(agpu_state_tracker* self, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setStencilFrontFace(stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetStencilBackFace(agpu_state_tracker* self, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setStencilBackFace(stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetPrimitiveType(agpu_state_tracker* self, agpu_primitive_topology type)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setPrimitiveType(type);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetVertexLayout(agpu_state_tracker* self, agpu_vertex_layout* layout)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setVertexLayout(asRef(agpu::vertex_layout, layout));
}

AGPU_EXPORT agpu_error agpuStateTrackerSetShaderSignature(agpu_state_tracker* self, agpu_shader_signature* signature)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setShaderSignature(asRef(agpu::shader_signature, signature));
}

AGPU_EXPORT agpu_error agpuStateTrackerSetSampleDescription(agpu_state_tracker* self, agpu_uint sample_count, agpu_uint sample_quality)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setSampleDescription(sample_count, sample_quality);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetViewport(agpu_state_tracker* self, agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setViewport(x, y, w, h);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetScissor(agpu_state_tracker* self, agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setScissor(x, y, w, h);
}

AGPU_EXPORT agpu_error agpuStateTrackerUseVertexBinding(agpu_state_tracker* self, agpu_vertex_binding* vertex_binding)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->useVertexBinding(asRef(agpu::vertex_binding, vertex_binding));
}

AGPU_EXPORT agpu_error agpuStateTrackerUseIndexBuffer(agpu_state_tracker* self, agpu_buffer* index_buffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->useIndexBuffer(asRef(agpu::buffer, index_buffer));
}

AGPU_EXPORT agpu_error agpuStateTrackerUseIndexBufferAt(agpu_state_tracker* self, agpu_buffer* index_buffer, agpu_size offset, agpu_size index_size)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->useIndexBufferAt(asRef(agpu::buffer, index_buffer), offset, index_size);
}

AGPU_EXPORT agpu_error agpuStateTrackerUseDrawIndirectBuffer(agpu_state_tracker* self, agpu_buffer* draw_buffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->useDrawIndirectBuffer(asRef(agpu::buffer, draw_buffer));
}

AGPU_EXPORT agpu_error agpuStateTrackerUseComputeDispatchIndirectBuffer(agpu_state_tracker* self, agpu_buffer* buffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->useComputeDispatchIndirectBuffer(asRef(agpu::buffer, buffer));
}

AGPU_EXPORT agpu_error agpuStateTrackerUseShaderResources(agpu_state_tracker* self, agpu_shader_resource_binding* binding)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->useShaderResources(asRef(agpu::shader_resource_binding, binding));
}

AGPU_EXPORT agpu_error agpuStateTrackerUseComputeShaderResources(agpu_state_tracker* self, agpu_shader_resource_binding* binding)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->useComputeShaderResources(asRef(agpu::shader_resource_binding, binding));
}

AGPU_EXPORT agpu_error agpuStateTrackerDrawArrays(agpu_state_tracker* self, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->drawArrays(vertex_count, instance_count, first_vertex, base_instance);
}

AGPU_EXPORT agpu_error agpuStateTrackerDrawArraysIndirect(agpu_state_tracker* self, agpu_size offset, agpu_size drawcount)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->drawArraysIndirect(offset, drawcount);
}

AGPU_EXPORT agpu_error agpuStateTrackerDrawElements(agpu_state_tracker* self, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->drawElements(index_count, instance_count, first_index, base_vertex, base_instance);
}

AGPU_EXPORT agpu_error agpuStateTrackerDrawElementsIndirect(agpu_state_tracker* self, agpu_size offset, agpu_size drawcount)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->drawElementsIndirect(offset, drawcount);
}

AGPU_EXPORT agpu_error agpuStateTrackerDispatchCompute(agpu_state_tracker* self, agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->dispatchCompute(group_count_x, group_count_y, group_count_z);
}

AGPU_EXPORT agpu_error agpuStateTrackerDispatchComputeIndirect(agpu_state_tracker* self, agpu_size offset)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->dispatchComputeIndirect(offset);
}

AGPU_EXPORT agpu_error agpuStateTrackerSetStencilReference(agpu_state_tracker* self, agpu_uint reference)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->setStencilReference(reference);
}

AGPU_EXPORT agpu_error agpuStateTrackerExecuteBundle(agpu_state_tracker* self, agpu_command_list* bundle)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->executeBundle(asRef(agpu::command_list, bundle));
}

AGPU_EXPORT agpu_error agpuStateTrackerBeginRenderPass(agpu_state_tracker* self, agpu_renderpass* renderpass, agpu_framebuffer* framebuffer, agpu_bool bundle_content)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->beginRenderPass(asRef(agpu::renderpass, renderpass), asRef(agpu::framebuffer, framebuffer), bundle_content);
}

AGPU_EXPORT agpu_error agpuStateTrackerEndRenderPass(agpu_state_tracker* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->endRenderPass();
}

AGPU_EXPORT agpu_error agpuStateTrackerResolveFramebuffer(agpu_state_tracker* self, agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->resolveFramebuffer(asRef(agpu::framebuffer, destFramebuffer), asRef(agpu::framebuffer, sourceFramebuffer));
}

AGPU_EXPORT agpu_error agpuStateTrackerResolveTexture(agpu_state_tracker* self, agpu_texture* sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, agpu_texture* destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->resolveTexture(asRef(agpu::texture, sourceTexture), sourceLevel, sourceLayer, asRef(agpu::texture, destTexture), destLevel, destLayer, levelCount, layerCount, aspect);
}

AGPU_EXPORT agpu_error agpuStateTrackerPushConstants(agpu_state_tracker* self, agpu_uint offset, agpu_uint size, agpu_pointer values)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->pushConstants(offset, size, values);
}

AGPU_EXPORT agpu_error agpuStateTrackerMemoryBarrier(agpu_state_tracker* self, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->memoryBarrier(source_stage, dest_stage, source_accesses, dest_accesses);
}

AGPU_EXPORT agpu_error agpuStateTrackerBufferMemoryBarrier(agpu_state_tracker* self, agpu_buffer* buffer, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_size offset, agpu_size size)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->bufferMemoryBarrier(asRef(agpu::buffer, buffer), source_stage, dest_stage, source_accesses, dest_accesses, offset, size);
}

AGPU_EXPORT agpu_error agpuStateTrackerTextureMemoryBarrier(agpu_state_tracker* self, agpu_texture* texture, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_subresource_range* subresource_range)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->textureMemoryBarrier(asRef(agpu::texture, texture), source_stage, dest_stage, source_accesses, dest_accesses, subresource_range);
}

AGPU_EXPORT agpu_error agpuStateTrackerPushBufferTransitionBarrier(agpu_state_tracker* self, agpu_buffer* buffer, agpu_buffer_usage_mask new_usage)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->pushBufferTransitionBarrier(asRef(agpu::buffer, buffer), new_usage);
}

AGPU_EXPORT agpu_error agpuStateTrackerPushTextureTransitionBarrier(agpu_state_tracker* self, agpu_texture* texture, agpu_texture_usage_mode_mask new_usage, agpu_subresource_range* subresource_range)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->pushTextureTransitionBarrier(asRef(agpu::texture, texture), new_usage, subresource_range);
}

AGPU_EXPORT agpu_error agpuStateTrackerPopBufferTransitionBarrier(agpu_state_tracker* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->popBufferTransitionBarrier();
}

AGPU_EXPORT agpu_error agpuStateTrackerPopTextureTransitionBarrier(agpu_state_tracker* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->popTextureTransitionBarrier();
}

AGPU_EXPORT agpu_error agpuStateTrackerCopyBuffer(agpu_state_tracker* self, agpu_buffer* source_buffer, agpu_size source_offset, agpu_buffer* dest_buffer, agpu_size dest_offset, agpu_size copy_size)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->copyBuffer(asRef(agpu::buffer, source_buffer), source_offset, asRef(agpu::buffer, dest_buffer), dest_offset, copy_size);
}

AGPU_EXPORT agpu_error agpuStateTrackerCopyBufferToTexture(agpu_state_tracker* self, agpu_buffer* buffer, agpu_texture* texture, agpu_buffer_image_copy_region* copy_region)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->copyBufferToTexture(asRef(agpu::buffer, buffer), asRef(agpu::texture, texture), copy_region);
}

AGPU_EXPORT agpu_error agpuStateTrackerCopyTextureToBuffer(agpu_state_tracker* self, agpu_texture* texture, agpu_buffer* buffer, agpu_buffer_image_copy_region* copy_region)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::state_tracker, self)->copyTextureToBuffer(asRef(agpu::texture, texture), asRef(agpu::buffer, buffer), copy_region);
}

//==============================================================================
// immediate_renderer C dispatching functions.
//==============================================================================

AGPU_EXPORT agpu_error agpuAddImmediateRendererReference(agpu_immediate_renderer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::immediate_renderer, self)->retain();
}

AGPU_EXPORT agpu_error agpuReleaseImmediateRendererReference(agpu_immediate_renderer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRefCounter(agpu::immediate_renderer, self)->release();
}

AGPU_EXPORT agpu_error agpuBeginImmediateRendering(agpu_immediate_renderer* self, agpu_state_tracker* state_tracker)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->beginRendering(asRef(agpu::state_tracker, state_tracker));
}

AGPU_EXPORT agpu_error agpuEndImmediateRendering(agpu_immediate_renderer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->endRendering();
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetBlendState(agpu_immediate_renderer* self, agpu_int renderTargetMask, agpu_bool enabled)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setBlendState(renderTargetMask, enabled);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetBlendFunction(agpu_immediate_renderer* self, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setBlendFunction(renderTargetMask, sourceFactor, destFactor, colorOperation, sourceAlphaFactor, destAlphaFactor, alphaOperation);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetColorMask(agpu_immediate_renderer* self, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setColorMask(renderTargetMask, redEnabled, greenEnabled, blueEnabled, alphaEnabled);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetFrontFace(agpu_immediate_renderer* self, agpu_face_winding winding)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setFrontFace(winding);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetCullMode(agpu_immediate_renderer* self, agpu_cull_mode mode)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setCullMode(mode);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetDepthBias(agpu_immediate_renderer* self, agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setDepthBias(constant_factor, clamp, slope_factor);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetDepthState(agpu_immediate_renderer* self, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setDepthState(enabled, writeMask, function);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetPolygonMode(agpu_immediate_renderer* self, agpu_polygon_mode mode)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setPolygonMode(mode);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetStencilState(agpu_immediate_renderer* self, agpu_bool enabled, agpu_int writeMask, agpu_int readMask)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setStencilState(enabled, writeMask, readMask);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetStencilFrontFace(agpu_immediate_renderer* self, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setStencilFrontFace(stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetStencilBackFace(agpu_immediate_renderer* self, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setStencilBackFace(stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetViewport(agpu_immediate_renderer* self, agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setViewport(x, y, w, h);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetScissor(agpu_immediate_renderer* self, agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setScissor(x, y, w, h);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetStencilReference(agpu_immediate_renderer* self, agpu_uint reference)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setStencilReference(reference);
}

AGPU_EXPORT agpu_error agpuImmediateRendererProjectionMatrixMode(agpu_immediate_renderer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->projectionMatrixMode();
}

AGPU_EXPORT agpu_error agpuImmediateRendererModelViewMatrixMode(agpu_immediate_renderer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->modelViewMatrixMode();
}

AGPU_EXPORT agpu_error agpuImmediateRendererTextureMatrixMode(agpu_immediate_renderer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->textureMatrixMode();
}

AGPU_EXPORT agpu_error agpuImmediateRendererIdentity(agpu_immediate_renderer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->loadIdentity();
}

AGPU_EXPORT agpu_error agpuImmediateRendererPushMatrix(agpu_immediate_renderer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->pushMatrix();
}

AGPU_EXPORT agpu_error agpuImmediateRendererPopMatrix(agpu_immediate_renderer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->popMatrix();
}

AGPU_EXPORT agpu_error agpuImmediateRendererLoadMatrix(agpu_immediate_renderer* self, agpu_float* elements)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->loadMatrix(elements);
}

AGPU_EXPORT agpu_error agpuImmediateRendererLoadTransposeMatrix(agpu_immediate_renderer* self, agpu_float* elements)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->loadTransposeMatrix(elements);
}

AGPU_EXPORT agpu_error agpuImmediateRendererMultiplyMatrix(agpu_immediate_renderer* self, agpu_float* elements)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->multiplyMatrix(elements);
}

AGPU_EXPORT agpu_error agpuImmediateRendererMultiplyTransposeMatrix(agpu_immediate_renderer* self, agpu_float* elements)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->multiplyTransposeMatrix(elements);
}

AGPU_EXPORT agpu_error agpuImmediateRendererOrtho(agpu_immediate_renderer* self, agpu_float left, agpu_float right, agpu_float bottom, agpu_float top, agpu_float near, agpu_float far)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->ortho(left, right, bottom, top, near, far);
}

AGPU_EXPORT agpu_error agpuImmediateRendererFrustum(agpu_immediate_renderer* self, agpu_float left, agpu_float right, agpu_float bottom, agpu_float top, agpu_float near, agpu_float far)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->frustum(left, right, bottom, top, near, far);
}

AGPU_EXPORT agpu_error agpuImmediateRendererPerspective(agpu_immediate_renderer* self, agpu_float fovy, agpu_float aspect, agpu_float near, agpu_float far)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->perspective(fovy, aspect, near, far);
}

AGPU_EXPORT agpu_error agpuImmediateRendererRotate(agpu_immediate_renderer* self, agpu_float angle, agpu_float x, agpu_float y, agpu_float z)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->rotate(angle, x, y, z);
}

AGPU_EXPORT agpu_error agpuImmediateRendererTranslate(agpu_immediate_renderer* self, agpu_float x, agpu_float y, agpu_float z)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->translate(x, y, z);
}

AGPU_EXPORT agpu_error agpuImmediateRendererScale(agpu_immediate_renderer* self, agpu_float x, agpu_float y, agpu_float z)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->scale(x, y, z);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetFlatShading(agpu_immediate_renderer* self, agpu_bool enabled)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setFlatShading(enabled);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetLightingEnabled(agpu_immediate_renderer* self, agpu_bool enabled)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setLightingEnabled(enabled);
}

AGPU_EXPORT agpu_error agpuImmediateRendererClearLights(agpu_immediate_renderer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->clearLights();
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetAmbientLighting(agpu_immediate_renderer* self, agpu_float r, agpu_float g, agpu_float b, agpu_float a)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setAmbientLighting(r, g, b, a);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetLight(agpu_immediate_renderer* self, agpu_uint index, agpu_bool enabled, agpu_immediate_renderer_light* state)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setLight(index, enabled, state);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetMaterial(agpu_immediate_renderer* self, agpu_immediate_renderer_material* state)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setMaterial(state);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetTextureEnabled(agpu_immediate_renderer* self, agpu_bool enabled)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setTexturingEnabled(enabled);
}

AGPU_EXPORT agpu_error agpuImmediateRendererBindTexture(agpu_immediate_renderer* self, agpu_texture* texture)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->bindTexture(asRef(agpu::texture, texture));
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetClipPlane(agpu_immediate_renderer* self, agpu_uint index, agpu_bool enabled, agpu_float p1, agpu_float p2, agpu_float p3, agpu_float p4)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setClipPlane(index, enabled, p1, p2, p3, p4);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetFogMode(agpu_immediate_renderer* self, agpu_immediate_renderer_fog_mode mode)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setFogMode(mode);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetFogColor(agpu_immediate_renderer* self, agpu_float r, agpu_float g, agpu_float b, agpu_float a)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setFogColor(r, g, b, a);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetFogDistances(agpu_immediate_renderer* self, agpu_float start, agpu_float end)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setFogDistances(start, end);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetFogDensity(agpu_immediate_renderer* self, agpu_float density)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setFogDensity(density);
}

AGPU_EXPORT agpu_error agpuBeginImmediateRendererPrimitives(agpu_immediate_renderer* self, agpu_primitive_topology type)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->beginPrimitives(type);
}

AGPU_EXPORT agpu_error agpuEndImmediateRendererPrimitives(agpu_immediate_renderer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->endPrimitives();
}

AGPU_EXPORT agpu_error agpuSetImmediateRendererColor(agpu_immediate_renderer* self, agpu_float r, agpu_float g, agpu_float b, agpu_float a)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->color(r, g, b, a);
}

AGPU_EXPORT agpu_error agpuSetImmediateRendererTexcoord(agpu_immediate_renderer* self, agpu_float x, agpu_float y)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->texcoord(x, y);
}

AGPU_EXPORT agpu_error agpuSetImmediateRendererNormal(agpu_immediate_renderer* self, agpu_float x, agpu_float y, agpu_float z)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->normal(x, y, z);
}

AGPU_EXPORT agpu_error agpuAddImmediateRendererVertex(agpu_immediate_renderer* self, agpu_float x, agpu_float y, agpu_float z)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->vertex(x, y, z);
}

AGPU_EXPORT agpu_error agpuBeginImmediateRendererMeshWithVertices(agpu_immediate_renderer* self, agpu_size vertexCount, agpu_size stride, agpu_size elementCount, agpu_pointer vertices)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->beginMeshWithVertices(vertexCount, stride, elementCount, vertices);
}

AGPU_EXPORT agpu_error agpuBeginImmediateRendererMeshWithVertexBinding(agpu_immediate_renderer* self, agpu_vertex_layout* layout, agpu_vertex_binding* vertices)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->beginMeshWithVertexBinding(asRef(agpu::vertex_layout, layout), asRef(agpu::vertex_binding, vertices));
}

AGPU_EXPORT agpu_error agpuImmediateRendererUseIndexBuffer(agpu_immediate_renderer* self, agpu_buffer* index_buffer)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->useIndexBuffer(asRef(agpu::buffer, index_buffer));
}

AGPU_EXPORT agpu_error agpuImmediateRendererUseIndexBufferAt(agpu_immediate_renderer* self, agpu_buffer* index_buffer, agpu_size offset, agpu_size index_size)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->useIndexBufferAt(asRef(agpu::buffer, index_buffer), offset, index_size);
}

AGPU_EXPORT agpu_error agpuSetImmediateRendererCurrentMeshColors(agpu_immediate_renderer* self, agpu_size stride, agpu_size elementCount, agpu_pointer colors)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setCurrentMeshColors(stride, elementCount, colors);
}

AGPU_EXPORT agpu_error agpuSetImmediateRendererCurrentMeshNormals(agpu_immediate_renderer* self, agpu_size stride, agpu_size elementCount, agpu_pointer normals)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setCurrentMeshNormals(stride, elementCount, normals);
}

AGPU_EXPORT agpu_error agpuSetImmediateRendererCurrentMeshTexCoords(agpu_immediate_renderer* self, agpu_size stride, agpu_size elementCount, agpu_pointer texcoords)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setCurrentMeshTexCoords(stride, elementCount, texcoords);
}

AGPU_EXPORT agpu_error agpuImmediateRendererSetPrimitiveType(agpu_immediate_renderer* self, agpu_primitive_topology type)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->setPrimitiveType(type);
}

AGPU_EXPORT agpu_error agpuImmediateRendererDrawArrays(agpu_immediate_renderer* self, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->drawArrays(vertex_count, instance_count, first_vertex, base_instance);
}

AGPU_EXPORT agpu_error agpuImmediateRendererDrawElements(agpu_immediate_renderer* self, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->drawElements(index_count, instance_count, first_index, base_vertex, base_instance);
}

AGPU_EXPORT agpu_error agpuImmediateRendererDrawElementsWithIndices(agpu_immediate_renderer* self, agpu_primitive_topology mode, agpu_pointer indices, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->drawElementsWithIndices(mode, indices, index_count, instance_count, first_index, base_vertex, base_instance);
}

AGPU_EXPORT agpu_error agpuEndImmediateRendererMesh(agpu_immediate_renderer* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::immediate_renderer, self)->endMesh();
}



#undef asRef
#undef asRefCounter

namespace agpu
{
agpu_icd_dispatch cppRefcountedDispatchTable = {
#include "agpu_icd.10.inc"
};
} // End of agpu
