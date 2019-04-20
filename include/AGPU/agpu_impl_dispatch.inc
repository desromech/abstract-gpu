
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

AGPU_EXPORT agpu_framebuffer* agpuCreateFrameBuffer(agpu_device* self, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView)
{
	return reinterpret_cast<agpu_framebuffer*> (asRef(agpu::device, self)->createFrameBuffer(width, height, colorCount, colorViews, depthStencilView));
}

AGPU_EXPORT agpu_renderpass* agpuCreateRenderPass(agpu_device* self, agpu_renderpass_description* description)
{
	return reinterpret_cast<agpu_renderpass*> (asRef(agpu::device, self)->createRenderPass(description));
}

AGPU_EXPORT agpu_texture* agpuCreateTexture(agpu_device* self, agpu_texture_description* description)
{
	return reinterpret_cast<agpu_texture*> (asRef(agpu::device, self)->createTexture(description));
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

AGPU_EXPORT agpu_error agpuDiscardTextureUploadBuffer(agpu_texture* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::texture, self)->discardUploadBuffer();
}

AGPU_EXPORT agpu_error agpuDiscardTextureReadbackBuffer(agpu_texture* self)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::texture, self)->discardReadbackBuffer();
}

AGPU_EXPORT agpu_error agpuGetTextureFullViewDescription(agpu_texture* self, agpu_texture_view_description* result)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::texture, self)->getFullViewDescription(result);
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

AGPU_EXPORT agpu_error agpuBindTexture(agpu_shader_resource_binding* self, agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader_resource_binding, self)->bindTexture(location, asRef(agpu::texture, texture), startMiplevel, miplevels, lodclamp);
}

AGPU_EXPORT agpu_error agpuBindTextureArrayRange(agpu_shader_resource_binding* self, agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader_resource_binding, self)->bindTextureArrayRange(location, asRef(agpu::texture, texture), startMiplevel, miplevels, firstElement, numberOfElements, lodclamp);
}

AGPU_EXPORT agpu_error agpuBindImage(agpu_shader_resource_binding* self, agpu_int location, agpu_texture* texture, agpu_int level, agpu_int layer, agpu_mapping_access access, agpu_texture_format format)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader_resource_binding, self)->bindImage(location, asRef(agpu::texture, texture), level, layer, access, format);
}

AGPU_EXPORT agpu_error agpuCreateSampler(agpu_shader_resource_binding* self, agpu_int location, agpu_sampler_description* description)
{
	if(!self) return AGPU_NULL_POINTER;
	return asRef(agpu::shader_resource_binding, self)->createSampler(location, description);
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



#undef asRef
#undef asRefCounter

namespace agpu
{
agpu_icd_dispatch cppRefcountedDispatchTable = {
#include "agpu_icd.10.inc"
};
} // End of agpu
