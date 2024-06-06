Class {
	#name : #AGPUCBindings,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #AGPUCBindingsBase,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'platform' }
AGPUCBindings >> openDevice_platform: platform openInfo: openInfo [
	^ self ffiCall: #(agpu_device* agpuOpenDevice (agpu_platform* platform , agpu_device_open_info* openInfo) )
]

{ #category : #'platform' }
AGPUCBindings >> getName_platform: platform [
	^ self ffiCall: #(agpu_cstring agpuGetPlatformName (agpu_platform* platform) )
]

{ #category : #'platform' }
AGPUCBindings >> getGpuCount_platform: platform [
	^ self ffiCall: #(agpu_size agpuGetPlatformGpuCount (agpu_platform* platform) )
]

{ #category : #'platform' }
AGPUCBindings >> getGpuName_platform: platform gpu_index: gpu_index [
	^ self ffiCall: #(agpu_cstring agpuGetPlatformGpuName (agpu_platform* platform , agpu_size gpu_index) )
]

{ #category : #'platform' }
AGPUCBindings >> getGpuDeviceType_platform: platform gpu_index: gpu_index [
	^ self ffiCall: #(agpu_device_type agpuGetPlatformGpuDeviceType (agpu_platform* platform , agpu_size gpu_index) )
]

{ #category : #'platform' }
AGPUCBindings >> isFeatureSupportedOnGPU_platform: platform gpu_index: gpu_index feature: feature [
	^ self ffiCall: #(agpu_bool agpuIsFeatureSupportedOnGPU (agpu_platform* platform , agpu_size gpu_index , agpu_feature feature) )
]

{ #category : #'platform' }
AGPUCBindings >> getLimitValueOnGPU_platform: platform gpu_index: gpu_index limit: limit [
	^ self ffiCall: #(agpu_uint agpuGetLimitValueOnGPU (agpu_platform* platform , agpu_size gpu_index , agpu_limit limit) )
]

{ #category : #'platform' }
AGPUCBindings >> getVersion_platform: platform [
	^ self ffiCall: #(agpu_int agpuGetPlatformVersion (agpu_platform* platform) )
]

{ #category : #'platform' }
AGPUCBindings >> getImplementationVersion_platform: platform [
	^ self ffiCall: #(agpu_int agpuGetPlatformImplementationVersion (agpu_platform* platform) )
]

{ #category : #'platform' }
AGPUCBindings >> hasRealMultithreading_platform: platform [
	^ self ffiCall: #(agpu_bool agpuPlatformHasRealMultithreading (agpu_platform* platform) )
]

{ #category : #'platform' }
AGPUCBindings >> isNative_platform: platform [
	^ self ffiCall: #(agpu_bool agpuIsNativePlatform (agpu_platform* platform) )
]

{ #category : #'platform' }
AGPUCBindings >> isCrossPlatform_platform: platform [
	^ self ffiCall: #(agpu_bool agpuIsCrossPlatform (agpu_platform* platform) )
]

{ #category : #'platform' }
AGPUCBindings >> createOfflineShaderCompiler_platform: platform [
	^ self ffiCall: #(agpu_offline_shader_compiler* agpuCreateOfflineShaderCompiler (agpu_platform* platform) )
]

{ #category : #'device' }
AGPUCBindings >> addReference_device: device [
	^ self ffiCall: #(agpu_error agpuAddDeviceReference (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> release_device: device [
	^ self ffiCall: #(agpu_error agpuReleaseDevice (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> getDefaultCommandQueue_device: device [
	^ self ffiCall: #(agpu_command_queue* agpuGetDefaultCommandQueue (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> createSwapChain_device: device commandQueue: commandQueue swapChainInfo: swapChainInfo [
	^ self ffiCall: #(agpu_swap_chain* agpuCreateSwapChain (agpu_device* device , agpu_command_queue* commandQueue , agpu_swap_chain_create_info* swapChainInfo) )
]

{ #category : #'device' }
AGPUCBindings >> createBuffer_device: device description: description initial_data: initial_data [
	^ self ffiCall: #(agpu_buffer* agpuCreateBuffer (agpu_device* device , agpu_buffer_description* description , agpu_pointer initial_data) )
]

{ #category : #'device' }
AGPUCBindings >> createVertexLayout_device: device [
	^ self ffiCall: #(agpu_vertex_layout* agpuCreateVertexLayout (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> createVertexBinding_device: device layout: layout [
	^ self ffiCall: #(agpu_vertex_binding* agpuCreateVertexBinding (agpu_device* device , agpu_vertex_layout* layout) )
]

{ #category : #'device' }
AGPUCBindings >> createShader_device: device type: type [
	^ self ffiCall: #(agpu_shader* agpuCreateShader (agpu_device* device , agpu_shader_type type) )
]

{ #category : #'device' }
AGPUCBindings >> createShaderSignatureBuilder_device: device [
	^ self ffiCall: #(agpu_shader_signature_builder* agpuCreateShaderSignatureBuilder (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> createPipelineBuilder_device: device [
	^ self ffiCall: #(agpu_pipeline_builder* agpuCreatePipelineBuilder (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> createComputePipelineBuilder_device: device [
	^ self ffiCall: #(agpu_compute_pipeline_builder* agpuCreateComputePipelineBuilder (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> createCommandAllocator_device: device type: type queue: queue [
	^ self ffiCall: #(agpu_command_allocator* agpuCreateCommandAllocator (agpu_device* device , agpu_command_list_type type , agpu_command_queue* queue) )
]

{ #category : #'device' }
AGPUCBindings >> createCommandList_device: device type: type allocator: allocator initial_pipeline_state: initial_pipeline_state [
	^ self ffiCall: #(agpu_command_list* agpuCreateCommandList (agpu_device* device , agpu_command_list_type type , agpu_command_allocator* allocator , agpu_pipeline_state* initial_pipeline_state) )
]

{ #category : #'device' }
AGPUCBindings >> getPreferredShaderLanguage_device: device [
	^ self ffiCall: #(agpu_shader_language agpuGetPreferredShaderLanguage (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> getPreferredIntermediateShaderLanguage_device: device [
	^ self ffiCall: #(agpu_shader_language agpuGetPreferredIntermediateShaderLanguage (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> getPreferredHighLevelShaderLanguage_device: device [
	^ self ffiCall: #(agpu_shader_language agpuGetPreferredHighLevelShaderLanguage (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> createFrameBuffer_device: device width: width height: height colorCount: colorCount colorViews: colorViews depthStencilView: depthStencilView [
	^ self ffiCall: #(agpu_framebuffer* agpuCreateFrameBuffer (agpu_device* device , agpu_uint width , agpu_uint height , agpu_uint colorCount , agpu_texture_view* colorViews , agpu_texture_view* depthStencilView) )
]

{ #category : #'device' }
AGPUCBindings >> createRenderPass_device: device description: description [
	^ self ffiCall: #(agpu_renderpass* agpuCreateRenderPass (agpu_device* device , agpu_renderpass_description* description) )
]

{ #category : #'device' }
AGPUCBindings >> createTexture_device: device description: description [
	^ self ffiCall: #(agpu_texture* agpuCreateTexture (agpu_device* device , agpu_texture_description* description) )
]

{ #category : #'device' }
AGPUCBindings >> createSampler_device: device description: description [
	^ self ffiCall: #(agpu_sampler* agpuCreateSampler (agpu_device* device , agpu_sampler_description* description) )
]

{ #category : #'device' }
AGPUCBindings >> createFence_device: device [
	^ self ffiCall: #(agpu_fence* agpuCreateFence (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> getMultiSampleQualityLevels_device: device format: format sample_count: sample_count [
	^ self ffiCall: #(agpu_int agpuGetMultiSampleQualityLevels (agpu_device* device , agpu_texture_format format , agpu_uint sample_count) )
]

{ #category : #'device' }
AGPUCBindings >> hasTopLeftNdcOrigin_device: device [
	^ self ffiCall: #(agpu_bool agpuHasTopLeftNdcOrigin (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> hasBottomLeftTextureCoordinates_device: device [
	^ self ffiCall: #(agpu_bool agpuHasBottomLeftTextureCoordinates (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> getName_device: device [
	^ self ffiCall: #(agpu_cstring agpuGetDeviceName (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> getType_device: device [
	^ self ffiCall: #(agpu_device_type agpuGetDeviceType (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> isFeatureSupported_device: device feature: feature [
	^ self ffiCall: #(agpu_bool agpuIsFeatureSupportedOnDevice (agpu_device* device , agpu_feature feature) )
]

{ #category : #'device' }
AGPUCBindings >> getLimitValue_device: device limit: limit [
	^ self ffiCall: #(agpu_uint agpuGetLimitValue (agpu_device* device , agpu_limit limit) )
]

{ #category : #'device' }
AGPUCBindings >> getVRSystem_device: device [
	^ self ffiCall: #(agpu_vr_system* agpuGetVRSystem (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> createWindowScraper_device: device [
	^ self ffiCall: #(agpu_window_scraper* agpuCreateWindowScraper (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> createOfflineShaderCompiler_device: device [
	^ self ffiCall: #(agpu_offline_shader_compiler* agpuCreateOfflineShaderCompilerForDevice (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> createStateTrackerCache_device: device command_queue_family: command_queue_family [
	^ self ffiCall: #(agpu_state_tracker_cache* agpuCreateStateTrackerCache (agpu_device* device , agpu_command_queue* command_queue_family) )
]

{ #category : #'device' }
AGPUCBindings >> finishExecution_device: device [
	^ self ffiCall: #(agpu_error agpuFinishDeviceExecution (agpu_device* device) )
]

{ #category : #'vr_system' }
AGPUCBindings >> addReference_vr_system: vr_system [
	^ self ffiCall: #(agpu_error agpuAddVRSystemReference (agpu_vr_system* vr_system) )
]

{ #category : #'vr_system' }
AGPUCBindings >> release_vr_system: vr_system [
	^ self ffiCall: #(agpu_error agpuReleaseVRSystem (agpu_vr_system* vr_system) )
]

{ #category : #'vr_system' }
AGPUCBindings >> getVRSystemName_vr_system: vr_system [
	^ self ffiCall: #(agpu_cstring agpuGetVRSystemName (agpu_vr_system* vr_system) )
]

{ #category : #'vr_system' }
AGPUCBindings >> getNativeHandle_vr_system: vr_system [
	^ self ffiCall: #(agpu_pointer agpuGetVRSystemNativeHandle (agpu_vr_system* vr_system) )
]

{ #category : #'vr_system' }
AGPUCBindings >> getRecommendedRenderTargetSize_vr_system: vr_system size: size [
	^ self ffiCall: #(agpu_error agpuGetVRRecommendedRenderTargetSize (agpu_vr_system* vr_system , agpu_size2d* size) )
]

{ #category : #'vr_system' }
AGPUCBindings >> getEyeToHeadTransform_vr_system: vr_system eye: eye transform: transform [
	^ self ffiCall: #(agpu_error agpuGetVREyeToHeadTransformInto (agpu_vr_system* vr_system , agpu_vr_eye eye , agpu_matrix4x4f* transform) )
]

{ #category : #'vr_system' }
AGPUCBindings >> getProjectionMatrix_vr_system: vr_system eye: eye near_distance: near_distance far_distance: far_distance projection_matrix: projection_matrix [
	^ self ffiCall: #(agpu_error agpuGetVRProjectionMatrix (agpu_vr_system* vr_system , agpu_vr_eye eye , agpu_float near_distance , agpu_float far_distance , agpu_matrix4x4f* projection_matrix) )
]

{ #category : #'vr_system' }
AGPUCBindings >> getProjectionFrustumTangents_vr_system: vr_system eye: eye frustum: frustum [
	^ self ffiCall: #(agpu_error agpuGetVRProjectionFrustumTangents (agpu_vr_system* vr_system , agpu_vr_eye eye , agpu_frustum_tangents* frustum) )
]

{ #category : #'vr_system' }
AGPUCBindings >> submitEyeRenderTargets_vr_system: vr_system left_eye: left_eye right_eye: right_eye [
	^ self ffiCall: #(agpu_error agpuSubmitVREyeRenderTargets (agpu_vr_system* vr_system , agpu_texture* left_eye , agpu_texture* right_eye) )
]

{ #category : #'vr_system' }
AGPUCBindings >> waitAndFetchPoses_vr_system: vr_system [
	^ self ffiCall: #(agpu_error agpuWaitAndFetchVRPoses (agpu_vr_system* vr_system) )
]

{ #category : #'vr_system' }
AGPUCBindings >> getMaxTrackedDevicePoseCount_vr_system: vr_system [
	^ self ffiCall: #(agpu_size agpuGetMaxVRTrackedDevicePoseCount (agpu_vr_system* vr_system) )
]

{ #category : #'vr_system' }
AGPUCBindings >> getCurrentTrackedDevicePoseCount_vr_system: vr_system [
	^ self ffiCall: #(agpu_size agpuGetCurrentVRTrackedDevicePoseCount (agpu_vr_system* vr_system) )
]

{ #category : #'vr_system' }
AGPUCBindings >> getCurrentTrackedDevicePoseInto_vr_system: vr_system index: index dest: dest [
	^ self ffiCall: #(agpu_error agpuGetCurrentVRTrackedDevicePoseInto (agpu_vr_system* vr_system , agpu_size index , agpu_vr_tracked_device_pose* dest) )
]

{ #category : #'vr_system' }
AGPUCBindings >> getMaxRenderTrackedDevicePoseCount_vr_system: vr_system [
	^ self ffiCall: #(agpu_size agpuGetMaxVRRenderTrackedDevicePoseCount (agpu_vr_system* vr_system) )
]

{ #category : #'vr_system' }
AGPUCBindings >> getCurrentRenderTrackedDevicePoseCount_vr_system: vr_system [
	^ self ffiCall: #(agpu_size agpuGetCurrentVRRenderTrackedDevicePoseCount (agpu_vr_system* vr_system) )
]

{ #category : #'vr_system' }
AGPUCBindings >> getCurrentRenderTrackedDevicePoseInto_vr_system: vr_system index: index dest: dest [
	^ self ffiCall: #(agpu_error agpuGetCurrentVRRenderTrackedDevicePoseInto (agpu_vr_system* vr_system , agpu_size index , agpu_vr_tracked_device_pose* dest) )
]

{ #category : #'vr_system' }
AGPUCBindings >> getTrackedDeviceRenderModel_vr_system: vr_system index: index [
	^ self ffiCall: #(agpu_vr_render_model* agpuGetVRTrackedDeviceRenderModel (agpu_vr_system* vr_system , agpu_size index) )
]

{ #category : #'vr_system' }
AGPUCBindings >> getControllerState_vr_system: vr_system index: index dest: dest [
	^ self ffiCall: #(agpu_bool agpuGetVRControllerState (agpu_vr_system* vr_system , agpu_size index , agpu_vr_controller_state* dest) )
]

{ #category : #'vr_system' }
AGPUCBindings >> pollEvent_vr_system: vr_system event: event [
	^ self ffiCall: #(agpu_bool agpuPollVREvent (agpu_vr_system* vr_system , agpu_vr_event* event) )
]

{ #category : #'window_scraper' }
AGPUCBindings >> addReference_window_scraper: window_scraper [
	^ self ffiCall: #(agpu_error agpuAddWindowScraperReference (agpu_window_scraper* window_scraper) )
]

{ #category : #'window_scraper' }
AGPUCBindings >> release_window_scraper: window_scraper [
	^ self ffiCall: #(agpu_error agpuReleaseWindowScraper (agpu_window_scraper* window_scraper) )
]

{ #category : #'window_scraper' }
AGPUCBindings >> enumerateWindows_window_scraper: window_scraper [
	^ self ffiCall: #(agpu_uint agpuWindowScraperEnumerateWindows (agpu_window_scraper* window_scraper) )
]

{ #category : #'window_scraper' }
AGPUCBindings >> getWindowTitle_window_scraper: window_scraper index: index [
	^ self ffiCall: #(agpu_cstring agpuWindowScraperGetWindowTitle (agpu_window_scraper* window_scraper , agpu_uint index) )
]

{ #category : #'window_scraper' }
AGPUCBindings >> createWindowHandle_window_scraper: window_scraper index: index [
	^ self ffiCall: #(agpu_window_scraper_handle* agpuWindowScraperCreateWindowHandle (agpu_window_scraper* window_scraper , agpu_uint index) )
]

{ #category : #'window_scraper_handle' }
AGPUCBindings >> addReference_window_scraper_handle: window_scraper_handle [
	^ self ffiCall: #(agpu_error agpuAddWindowScraperHandleReference (agpu_window_scraper_handle* window_scraper_handle) )
]

{ #category : #'window_scraper_handle' }
AGPUCBindings >> release_window_scraper_handle: window_scraper_handle [
	^ self ffiCall: #(agpu_error agpuReleaseWindowHandleScraper (agpu_window_scraper_handle* window_scraper_handle) )
]

{ #category : #'window_scraper_handle' }
AGPUCBindings >> isValid_window_scraper_handle: window_scraper_handle [
	^ self ffiCall: #(agpu_bool agpuWindowScraperHandleIsValid (agpu_window_scraper_handle* window_scraper_handle) )
]

{ #category : #'window_scraper_handle' }
AGPUCBindings >> isVisible_window_scraper_handle: window_scraper_handle [
	^ self ffiCall: #(agpu_bool agpuWindowScraperHandleIsVisible (agpu_window_scraper_handle* window_scraper_handle) )
]

{ #category : #'window_scraper_handle' }
AGPUCBindings >> getWidth_window_scraper_handle: window_scraper_handle [
	^ self ffiCall: #(agpu_uint agpuWindowScraperHandleGetWidth (agpu_window_scraper_handle* window_scraper_handle) )
]

{ #category : #'window_scraper_handle' }
AGPUCBindings >> getHeight_window_scraper_handle: window_scraper_handle [
	^ self ffiCall: #(agpu_uint agpuWindowScraperHandleGetHeight (agpu_window_scraper_handle* window_scraper_handle) )
]

{ #category : #'window_scraper_handle' }
AGPUCBindings >> captureInTexture_window_scraper_handle: window_scraper_handle [
	^ self ffiCall: #(agpu_texture* agpuWindowScraperHandleCaptureInTexture (agpu_window_scraper_handle* window_scraper_handle) )
]

{ #category : #'swap_chain' }
AGPUCBindings >> addReference_swap_chain: swap_chain [
	^ self ffiCall: #(agpu_error agpuAddSwapChainReference (agpu_swap_chain* swap_chain) )
]

{ #category : #'swap_chain' }
AGPUCBindings >> release_swap_chain: swap_chain [
	^ self ffiCall: #(agpu_error agpuReleaseSwapChain (agpu_swap_chain* swap_chain) )
]

{ #category : #'swap_chain' }
AGPUCBindings >> swapBuffers_swap_chain: swap_chain [
	^ self ffiCall: #(agpu_error agpuSwapBuffers (agpu_swap_chain* swap_chain) )
]

{ #category : #'swap_chain' }
AGPUCBindings >> getCurrentBackBuffer_swap_chain: swap_chain [
	^ self ffiCall: #(agpu_framebuffer* agpuGetCurrentBackBuffer (agpu_swap_chain* swap_chain) )
]

{ #category : #'swap_chain' }
AGPUCBindings >> getCurrentBackBufferForLayer_swap_chain: swap_chain layer: layer [
	^ self ffiCall: #(agpu_framebuffer* agpuGetCurrentBackBufferForLayer (agpu_swap_chain* swap_chain , agpu_uint layer) )
]

{ #category : #'swap_chain' }
AGPUCBindings >> getCurrentBackBufferIndex_swap_chain: swap_chain [
	^ self ffiCall: #(agpu_size agpuGetCurrentBackBufferIndex (agpu_swap_chain* swap_chain) )
]

{ #category : #'swap_chain' }
AGPUCBindings >> getFramebufferCount_swap_chain: swap_chain [
	^ self ffiCall: #(agpu_size agpuGetFramebufferCount (agpu_swap_chain* swap_chain) )
]

{ #category : #'swap_chain' }
AGPUCBindings >> getWidth_swap_chain: swap_chain [
	^ self ffiCall: #(agpu_uint agpuGetSwapChainWidth (agpu_swap_chain* swap_chain) )
]

{ #category : #'swap_chain' }
AGPUCBindings >> getHeight_swap_chain: swap_chain [
	^ self ffiCall: #(agpu_uint agpuGetSwapChainHeight (agpu_swap_chain* swap_chain) )
]

{ #category : #'swap_chain' }
AGPUCBindings >> getLayerCount_swap_chain: swap_chain [
	^ self ffiCall: #(agpu_uint agpuGetSwapChainLayerCount (agpu_swap_chain* swap_chain) )
]

{ #category : #'swap_chain' }
AGPUCBindings >> setOverlayPosition_swap_chain: swap_chain x: x y: y [
	^ self ffiCall: #(agpu_error agpuSetSwapChainOverlayPosition (agpu_swap_chain* swap_chain , agpu_int x , agpu_int y) )
]

{ #category : #'compute_pipeline_builder' }
AGPUCBindings >> addReference_compute_pipeline_builder: compute_pipeline_builder [
	^ self ffiCall: #(agpu_error agpuAddComputePipelineBuilderReference (agpu_compute_pipeline_builder* compute_pipeline_builder) )
]

{ #category : #'compute_pipeline_builder' }
AGPUCBindings >> release_compute_pipeline_builder: compute_pipeline_builder [
	^ self ffiCall: #(agpu_error agpuReleaseComputePipelineBuilder (agpu_compute_pipeline_builder* compute_pipeline_builder) )
]

{ #category : #'compute_pipeline_builder' }
AGPUCBindings >> build_compute_pipeline_builder: compute_pipeline_builder [
	^ self ffiCall: #(agpu_pipeline_state* agpuBuildComputePipelineState (agpu_compute_pipeline_builder* compute_pipeline_builder) )
]

{ #category : #'compute_pipeline_builder' }
AGPUCBindings >> attachShader_compute_pipeline_builder: compute_pipeline_builder shader: shader [
	^ self ffiCall: #(agpu_error agpuAttachComputeShader (agpu_compute_pipeline_builder* compute_pipeline_builder , agpu_shader* shader) )
]

{ #category : #'compute_pipeline_builder' }
AGPUCBindings >> attachShaderWithEntryPoint_compute_pipeline_builder: compute_pipeline_builder shader: shader type: type entry_point: entry_point [
	^ self ffiCall: #(agpu_error agpuAttachComputeShaderWithEntryPoint (agpu_compute_pipeline_builder* compute_pipeline_builder , agpu_shader* shader , agpu_shader_type type , agpu_cstring entry_point) )
]

{ #category : #'compute_pipeline_builder' }
AGPUCBindings >> getBuildingLogLength_compute_pipeline_builder: compute_pipeline_builder [
	^ self ffiCall: #(agpu_size agpuGetComputePipelineBuildingLogLength (agpu_compute_pipeline_builder* compute_pipeline_builder) )
]

{ #category : #'compute_pipeline_builder' }
AGPUCBindings >> getBuildingLog_compute_pipeline_builder: compute_pipeline_builder buffer_size: buffer_size buffer: buffer [
	^ self ffiCall: #(agpu_error agpuGetComputePipelineBuildingLog (agpu_compute_pipeline_builder* compute_pipeline_builder , agpu_size buffer_size , agpu_string_buffer buffer) )
]

{ #category : #'compute_pipeline_builder' }
AGPUCBindings >> setShaderSignature_compute_pipeline_builder: compute_pipeline_builder signature: signature [
	^ self ffiCall: #(agpu_error agpuSetComputePipelineShaderSignature (agpu_compute_pipeline_builder* compute_pipeline_builder , agpu_shader_signature* signature) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> addReference_pipeline_builder: pipeline_builder [
	^ self ffiCall: #(agpu_error agpuAddPipelineBuilderReference (agpu_pipeline_builder* pipeline_builder) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> release_pipeline_builder: pipeline_builder [
	^ self ffiCall: #(agpu_error agpuReleasePipelineBuilder (agpu_pipeline_builder* pipeline_builder) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> build_pipeline_builder: pipeline_builder [
	^ self ffiCall: #(agpu_pipeline_state* agpuBuildPipelineState (agpu_pipeline_builder* pipeline_builder) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> attachShader_pipeline_builder: pipeline_builder shader: shader [
	^ self ffiCall: #(agpu_error agpuAttachShader (agpu_pipeline_builder* pipeline_builder , agpu_shader* shader) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> attachShaderWithEntryPoint_pipeline_builder: pipeline_builder shader: shader type: type entry_point: entry_point [
	^ self ffiCall: #(agpu_error agpuAttachShaderWithEntryPoint (agpu_pipeline_builder* pipeline_builder , agpu_shader* shader , agpu_shader_type type , agpu_cstring entry_point) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> getBuildingLogLength_pipeline_builder: pipeline_builder [
	^ self ffiCall: #(agpu_size agpuGetPipelineBuildingLogLength (agpu_pipeline_builder* pipeline_builder) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> getBuildingLog_pipeline_builder: pipeline_builder buffer_size: buffer_size buffer: buffer [
	^ self ffiCall: #(agpu_error agpuGetPipelineBuildingLog (agpu_pipeline_builder* pipeline_builder , agpu_size buffer_size , agpu_string_buffer buffer) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setBlendState_pipeline_builder: pipeline_builder renderTargetMask: renderTargetMask enabled: enabled [
	^ self ffiCall: #(agpu_error agpuSetBlendState (agpu_pipeline_builder* pipeline_builder , agpu_int renderTargetMask , agpu_bool enabled) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setBlendFunction_pipeline_builder: pipeline_builder renderTargetMask: renderTargetMask sourceFactor: sourceFactor destFactor: destFactor colorOperation: colorOperation sourceAlphaFactor: sourceAlphaFactor destAlphaFactor: destAlphaFactor alphaOperation: alphaOperation [
	^ self ffiCall: #(agpu_error agpuSetBlendFunction (agpu_pipeline_builder* pipeline_builder , agpu_int renderTargetMask , agpu_blending_factor sourceFactor , agpu_blending_factor destFactor , agpu_blending_operation colorOperation , agpu_blending_factor sourceAlphaFactor , agpu_blending_factor destAlphaFactor , agpu_blending_operation alphaOperation) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setColorMask_pipeline_builder: pipeline_builder renderTargetMask: renderTargetMask redEnabled: redEnabled greenEnabled: greenEnabled blueEnabled: blueEnabled alphaEnabled: alphaEnabled [
	^ self ffiCall: #(agpu_error agpuSetColorMask (agpu_pipeline_builder* pipeline_builder , agpu_int renderTargetMask , agpu_bool redEnabled , agpu_bool greenEnabled , agpu_bool blueEnabled , agpu_bool alphaEnabled) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setFrontFace_pipeline_builder: pipeline_builder winding: winding [
	^ self ffiCall: #(agpu_error agpuSetFrontFace (agpu_pipeline_builder* pipeline_builder , agpu_face_winding winding) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setCullMode_pipeline_builder: pipeline_builder mode: mode [
	^ self ffiCall: #(agpu_error agpuSetCullMode (agpu_pipeline_builder* pipeline_builder , agpu_cull_mode mode) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setDepthBias_pipeline_builder: pipeline_builder constant_factor: constant_factor clamp: clamp slope_factor: slope_factor [
	^ self ffiCall: #(agpu_error agpuSetDepthBias (agpu_pipeline_builder* pipeline_builder , agpu_float constant_factor , agpu_float clamp , agpu_float slope_factor) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setDepthState_pipeline_builder: pipeline_builder enabled: enabled writeMask: writeMask function: function [
	^ self ffiCall: #(agpu_error agpuSetDepthState (agpu_pipeline_builder* pipeline_builder , agpu_bool enabled , agpu_bool writeMask , agpu_compare_function function) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setPolygonMode_pipeline_builder: pipeline_builder mode: mode [
	^ self ffiCall: #(agpu_error agpuSetPolygonMode (agpu_pipeline_builder* pipeline_builder , agpu_polygon_mode mode) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setStencilState_pipeline_builder: pipeline_builder enabled: enabled writeMask: writeMask readMask: readMask [
	^ self ffiCall: #(agpu_error agpuSetStencilState (agpu_pipeline_builder* pipeline_builder , agpu_bool enabled , agpu_int writeMask , agpu_int readMask) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setStencilFrontFace_pipeline_builder: pipeline_builder stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	^ self ffiCall: #(agpu_error agpuSetStencilFrontFace (agpu_pipeline_builder* pipeline_builder , agpu_stencil_operation stencilFailOperation , agpu_stencil_operation depthFailOperation , agpu_stencil_operation stencilDepthPassOperation , agpu_compare_function stencilFunction) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setStencilBackFace_pipeline_builder: pipeline_builder stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	^ self ffiCall: #(agpu_error agpuSetStencilBackFace (agpu_pipeline_builder* pipeline_builder , agpu_stencil_operation stencilFailOperation , agpu_stencil_operation depthFailOperation , agpu_stencil_operation stencilDepthPassOperation , agpu_compare_function stencilFunction) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setRenderTargetCount_pipeline_builder: pipeline_builder count: count [
	^ self ffiCall: #(agpu_error agpuSetRenderTargetCount (agpu_pipeline_builder* pipeline_builder , agpu_int count) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setRenderTargetFormat_pipeline_builder: pipeline_builder index: index format: format [
	^ self ffiCall: #(agpu_error agpuSetRenderTargetFormat (agpu_pipeline_builder* pipeline_builder , agpu_uint index , agpu_texture_format format) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setDepthStencilFormat_pipeline_builder: pipeline_builder format: format [
	^ self ffiCall: #(agpu_error agpuSetDepthStencilFormat (agpu_pipeline_builder* pipeline_builder , agpu_texture_format format) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setPrimitiveType_pipeline_builder: pipeline_builder type: type [
	^ self ffiCall: #(agpu_error agpuSetPrimitiveType (agpu_pipeline_builder* pipeline_builder , agpu_primitive_topology type) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setVertexLayout_pipeline_builder: pipeline_builder layout: layout [
	^ self ffiCall: #(agpu_error agpuSetVertexLayout (agpu_pipeline_builder* pipeline_builder , agpu_vertex_layout* layout) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setShaderSignature_pipeline_builder: pipeline_builder signature: signature [
	^ self ffiCall: #(agpu_error agpuSetPipelineShaderSignature (agpu_pipeline_builder* pipeline_builder , agpu_shader_signature* signature) )
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setSampleDescription_pipeline_builder: pipeline_builder sample_count: sample_count sample_quality: sample_quality [
	^ self ffiCall: #(agpu_error agpuSetSampleDescription (agpu_pipeline_builder* pipeline_builder , agpu_uint sample_count , agpu_uint sample_quality) )
]

{ #category : #'pipeline_state' }
AGPUCBindings >> addReference_pipeline_state: pipeline_state [
	^ self ffiCall: #(agpu_error agpuAddPipelineStateReference (agpu_pipeline_state* pipeline_state) )
]

{ #category : #'pipeline_state' }
AGPUCBindings >> release_pipeline_state: pipeline_state [
	^ self ffiCall: #(agpu_error agpuReleasePipelineState (agpu_pipeline_state* pipeline_state) )
]

{ #category : #'command_queue' }
AGPUCBindings >> addReference_command_queue: command_queue [
	^ self ffiCall: #(agpu_error agpuAddCommandQueueReference (agpu_command_queue* command_queue) )
]

{ #category : #'command_queue' }
AGPUCBindings >> release_command_queue: command_queue [
	^ self ffiCall: #(agpu_error agpuReleaseCommandQueue (agpu_command_queue* command_queue) )
]

{ #category : #'command_queue' }
AGPUCBindings >> addCommandList_command_queue: command_queue command_list: command_list [
	^ self ffiCall: #(agpu_error agpuAddCommandList (agpu_command_queue* command_queue , agpu_command_list* command_list) )
]

{ #category : #'command_queue' }
AGPUCBindings >> addCommandListsAndSignalFence_command_queue: command_queue count: count command_lists: command_lists fence: fence [
	^ self ffiCall: #(agpu_error agpuAddCommandListsAndSignalFence (agpu_command_queue* command_queue , agpu_uint count , agpu_command_list* command_lists , agpu_fence* fence) )
]

{ #category : #'command_queue' }
AGPUCBindings >> finishExecution_command_queue: command_queue [
	^ self ffiCall: #(agpu_error agpuFinishQueueExecution (agpu_command_queue* command_queue) )
]

{ #category : #'command_queue' }
AGPUCBindings >> signalFence_command_queue: command_queue fence: fence [
	^ self ffiCall: #(agpu_error agpuSignalFence (agpu_command_queue* command_queue , agpu_fence* fence) )
]

{ #category : #'command_queue' }
AGPUCBindings >> waitFence_command_queue: command_queue fence: fence [
	^ self ffiCall: #(agpu_error agpuWaitFence (agpu_command_queue* command_queue , agpu_fence* fence) )
]

{ #category : #'command_allocator' }
AGPUCBindings >> addReference_command_allocator: command_allocator [
	^ self ffiCall: #(agpu_error agpuAddCommandAllocatorReference (agpu_command_allocator* command_allocator) )
]

{ #category : #'command_allocator' }
AGPUCBindings >> release_command_allocator: command_allocator [
	^ self ffiCall: #(agpu_error agpuReleaseCommandAllocator (agpu_command_allocator* command_allocator) )
]

{ #category : #'command_allocator' }
AGPUCBindings >> reset_command_allocator: command_allocator [
	^ self ffiCall: #(agpu_error agpuResetCommandAllocator (agpu_command_allocator* command_allocator) )
]

{ #category : #'command_list' }
AGPUCBindings >> addReference_command_list: command_list [
	^ self ffiCall: #(agpu_error agpuAddCommandListReference (agpu_command_list* command_list) )
]

{ #category : #'command_list' }
AGPUCBindings >> release_command_list: command_list [
	^ self ffiCall: #(agpu_error agpuReleaseCommandList (agpu_command_list* command_list) )
]

{ #category : #'command_list' }
AGPUCBindings >> setShaderSignature_command_list: command_list signature: signature [
	^ self ffiCall: #(agpu_error agpuSetShaderSignature (agpu_command_list* command_list , agpu_shader_signature* signature) )
]

{ #category : #'command_list' }
AGPUCBindings >> setViewport_command_list: command_list x: x y: y w: w h: h [
	^ self ffiCall: #(agpu_error agpuSetViewport (agpu_command_list* command_list , agpu_int x , agpu_int y , agpu_int w , agpu_int h) )
]

{ #category : #'command_list' }
AGPUCBindings >> setScissor_command_list: command_list x: x y: y w: w h: h [
	^ self ffiCall: #(agpu_error agpuSetScissor (agpu_command_list* command_list , agpu_int x , agpu_int y , agpu_int w , agpu_int h) )
]

{ #category : #'command_list' }
AGPUCBindings >> usePipelineState_command_list: command_list pipeline: pipeline [
	^ self ffiCall: #(agpu_error agpuUsePipelineState (agpu_command_list* command_list , agpu_pipeline_state* pipeline) )
]

{ #category : #'command_list' }
AGPUCBindings >> useVertexBinding_command_list: command_list vertex_binding: vertex_binding [
	^ self ffiCall: #(agpu_error agpuUseVertexBinding (agpu_command_list* command_list , agpu_vertex_binding* vertex_binding) )
]

{ #category : #'command_list' }
AGPUCBindings >> useIndexBuffer_command_list: command_list index_buffer: index_buffer [
	^ self ffiCall: #(agpu_error agpuUseIndexBuffer (agpu_command_list* command_list , agpu_buffer* index_buffer) )
]

{ #category : #'command_list' }
AGPUCBindings >> useIndexBufferAt_command_list: command_list index_buffer: index_buffer offset: offset index_size: index_size [
	^ self ffiCall: #(agpu_error agpuUseIndexBufferAt (agpu_command_list* command_list , agpu_buffer* index_buffer , agpu_size offset , agpu_size index_size) )
]

{ #category : #'command_list' }
AGPUCBindings >> useDrawIndirectBuffer_command_list: command_list draw_buffer: draw_buffer [
	^ self ffiCall: #(agpu_error agpuUseDrawIndirectBuffer (agpu_command_list* command_list , agpu_buffer* draw_buffer) )
]

{ #category : #'command_list' }
AGPUCBindings >> useComputeDispatchIndirectBuffer_command_list: command_list buffer: buffer [
	^ self ffiCall: #(agpu_error agpuUseComputeDispatchIndirectBuffer (agpu_command_list* command_list , agpu_buffer* buffer) )
]

{ #category : #'command_list' }
AGPUCBindings >> useShaderResources_command_list: command_list binding: binding [
	^ self ffiCall: #(agpu_error agpuUseShaderResources (agpu_command_list* command_list , agpu_shader_resource_binding* binding) )
]

{ #category : #'command_list' }
AGPUCBindings >> useShaderResourcesInSlot_command_list: command_list binding: binding slot: slot [
	^ self ffiCall: #(agpu_error agpuUseShaderResourcesInSlot (agpu_command_list* command_list , agpu_shader_resource_binding* binding , agpu_uint slot) )
]

{ #category : #'command_list' }
AGPUCBindings >> useComputeShaderResources_command_list: command_list binding: binding [
	^ self ffiCall: #(agpu_error agpuUseComputeShaderResources (agpu_command_list* command_list , agpu_shader_resource_binding* binding) )
]

{ #category : #'command_list' }
AGPUCBindings >> useComputeShaderResourcesInSlot_command_list: command_list binding: binding slot: slot [
	^ self ffiCall: #(agpu_error agpuUseComputeShaderResourcesInSlot (agpu_command_list* command_list , agpu_shader_resource_binding* binding , agpu_uint slot) )
]

{ #category : #'command_list' }
AGPUCBindings >> drawArrays_command_list: command_list vertex_count: vertex_count instance_count: instance_count first_vertex: first_vertex base_instance: base_instance [
	^ self ffiCall: #(agpu_error agpuDrawArrays (agpu_command_list* command_list , agpu_uint vertex_count , agpu_uint instance_count , agpu_uint first_vertex , agpu_uint base_instance) )
]

{ #category : #'command_list' }
AGPUCBindings >> drawArraysIndirect_command_list: command_list offset: offset drawcount: drawcount [
	^ self ffiCall: #(agpu_error agpuDrawArraysIndirect (agpu_command_list* command_list , agpu_size offset , agpu_size drawcount) )
]

{ #category : #'command_list' }
AGPUCBindings >> drawElements_command_list: command_list index_count: index_count instance_count: instance_count first_index: first_index base_vertex: base_vertex base_instance: base_instance [
	^ self ffiCall: #(agpu_error agpuDrawElements (agpu_command_list* command_list , agpu_uint index_count , agpu_uint instance_count , agpu_uint first_index , agpu_int base_vertex , agpu_uint base_instance) )
]

{ #category : #'command_list' }
AGPUCBindings >> drawElementsIndirect_command_list: command_list offset: offset drawcount: drawcount [
	^ self ffiCall: #(agpu_error agpuDrawElementsIndirect (agpu_command_list* command_list , agpu_size offset , agpu_size drawcount) )
]

{ #category : #'command_list' }
AGPUCBindings >> dispatchCompute_command_list: command_list group_count_x: group_count_x group_count_y: group_count_y group_count_z: group_count_z [
	^ self ffiCall: #(agpu_error agpuDispatchCompute (agpu_command_list* command_list , agpu_uint group_count_x , agpu_uint group_count_y , agpu_uint group_count_z) )
]

{ #category : #'command_list' }
AGPUCBindings >> dispatchComputeIndirect_command_list: command_list offset: offset [
	^ self ffiCall: #(agpu_error agpuDispatchComputeIndirect (agpu_command_list* command_list , agpu_size offset) )
]

{ #category : #'command_list' }
AGPUCBindings >> setStencilReference_command_list: command_list reference: reference [
	^ self ffiCall: #(agpu_error agpuSetStencilReference (agpu_command_list* command_list , agpu_uint reference) )
]

{ #category : #'command_list' }
AGPUCBindings >> executeBundle_command_list: command_list bundle: bundle [
	^ self ffiCall: #(agpu_error agpuExecuteBundle (agpu_command_list* command_list , agpu_command_list* bundle) )
]

{ #category : #'command_list' }
AGPUCBindings >> close_command_list: command_list [
	^ self ffiCall: #(agpu_error agpuCloseCommandList (agpu_command_list* command_list) )
]

{ #category : #'command_list' }
AGPUCBindings >> reset_command_list: command_list allocator: allocator initial_pipeline_state: initial_pipeline_state [
	^ self ffiCall: #(agpu_error agpuResetCommandList (agpu_command_list* command_list , agpu_command_allocator* allocator , agpu_pipeline_state* initial_pipeline_state) )
]

{ #category : #'command_list' }
AGPUCBindings >> resetBundle_command_list: command_list allocator: allocator initial_pipeline_state: initial_pipeline_state inheritance_info: inheritance_info [
	^ self ffiCall: #(agpu_error agpuResetBundleCommandList (agpu_command_list* command_list , agpu_command_allocator* allocator , agpu_pipeline_state* initial_pipeline_state , agpu_inheritance_info* inheritance_info) )
]

{ #category : #'command_list' }
AGPUCBindings >> beginRenderPass_command_list: command_list renderpass: renderpass framebuffer: framebuffer bundle_content: bundle_content [
	^ self ffiCall: #(agpu_error agpuBeginRenderPass (agpu_command_list* command_list , agpu_renderpass* renderpass , agpu_framebuffer* framebuffer , agpu_bool bundle_content) )
]

{ #category : #'command_list' }
AGPUCBindings >> endRenderPass_command_list: command_list [
	^ self ffiCall: #(agpu_error agpuEndRenderPass (agpu_command_list* command_list) )
]

{ #category : #'command_list' }
AGPUCBindings >> resolveFramebuffer_command_list: command_list destFramebuffer: destFramebuffer sourceFramebuffer: sourceFramebuffer [
	^ self ffiCall: #(agpu_error agpuResolveFramebuffer (agpu_command_list* command_list , agpu_framebuffer* destFramebuffer , agpu_framebuffer* sourceFramebuffer) )
]

{ #category : #'command_list' }
AGPUCBindings >> resolveTexture_command_list: command_list sourceTexture: sourceTexture sourceLevel: sourceLevel sourceLayer: sourceLayer destTexture: destTexture destLevel: destLevel destLayer: destLayer levelCount: levelCount layerCount: layerCount aspect: aspect [
	^ self ffiCall: #(agpu_error agpuResolveTexture (agpu_command_list* command_list , agpu_texture* sourceTexture , agpu_uint sourceLevel , agpu_uint sourceLayer , agpu_texture* destTexture , agpu_uint destLevel , agpu_uint destLayer , agpu_uint levelCount , agpu_uint layerCount , agpu_texture_aspect aspect) )
]

{ #category : #'command_list' }
AGPUCBindings >> pushConstants_command_list: command_list offset: offset size: size values: values [
	^ self ffiCall: #(agpu_error agpuPushConstants (agpu_command_list* command_list , agpu_uint offset , agpu_uint size , agpu_pointer values) )
]

{ #category : #'command_list' }
AGPUCBindings >> memoryBarrier_command_list: command_list source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses [
	^ self ffiCall: #(agpu_error agpuMemoryBarrier (agpu_command_list* command_list , agpu_pipeline_stage_flags source_stage , agpu_pipeline_stage_flags dest_stage , agpu_access_flags source_accesses , agpu_access_flags dest_accesses) )
]

{ #category : #'command_list' }
AGPUCBindings >> bufferMemoryBarrier_command_list: command_list buffer: buffer source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses offset: offset size: size [
	^ self ffiCall: #(agpu_error agpuBufferMemoryBarrier (agpu_command_list* command_list , agpu_buffer* buffer , agpu_pipeline_stage_flags source_stage , agpu_pipeline_stage_flags dest_stage , agpu_access_flags source_accesses , agpu_access_flags dest_accesses , agpu_size offset , agpu_size size) )
]

{ #category : #'command_list' }
AGPUCBindings >> textureMemoryBarrier_command_list: command_list texture: texture source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses old_usage: old_usage new_usage: new_usage subresource_range: subresource_range [
	^ self ffiCall: #(agpu_error agpuTextureMemoryBarrier (agpu_command_list* command_list , agpu_texture* texture , agpu_pipeline_stage_flags source_stage , agpu_pipeline_stage_flags dest_stage , agpu_access_flags source_accesses , agpu_access_flags dest_accesses , agpu_texture_usage_mode_mask old_usage , agpu_texture_usage_mode_mask new_usage , agpu_texture_subresource_range* subresource_range) )
]

{ #category : #'command_list' }
AGPUCBindings >> pushBufferTransitionBarrier_command_list: command_list buffer: buffer old_usage: old_usage new_usage: new_usage [
	^ self ffiCall: #(agpu_error agpuPushBufferTransitionBarrier (agpu_command_list* command_list , agpu_buffer* buffer , agpu_buffer_usage_mask old_usage , agpu_buffer_usage_mask new_usage) )
]

{ #category : #'command_list' }
AGPUCBindings >> pushTextureTransitionBarrier_command_list: command_list texture: texture old_usage: old_usage new_usage: new_usage subresource_range: subresource_range [
	^ self ffiCall: #(agpu_error agpuPushTextureTransitionBarrier (agpu_command_list* command_list , agpu_texture* texture , agpu_texture_usage_mode_mask old_usage , agpu_texture_usage_mode_mask new_usage , agpu_texture_subresource_range* subresource_range) )
]

{ #category : #'command_list' }
AGPUCBindings >> popBufferTransitionBarrier_command_list: command_list [
	^ self ffiCall: #(agpu_error agpuPopBufferTransitionBarrier (agpu_command_list* command_list) )
]

{ #category : #'command_list' }
AGPUCBindings >> popTextureTransitionBarrier_command_list: command_list [
	^ self ffiCall: #(agpu_error agpuPopTextureTransitionBarrier (agpu_command_list* command_list) )
]

{ #category : #'command_list' }
AGPUCBindings >> copyBuffer_command_list: command_list source_buffer: source_buffer source_offset: source_offset dest_buffer: dest_buffer dest_offset: dest_offset copy_size: copy_size [
	^ self ffiCall: #(agpu_error agpuCopyBuffer (agpu_command_list* command_list , agpu_buffer* source_buffer , agpu_size source_offset , agpu_buffer* dest_buffer , agpu_size dest_offset , agpu_size copy_size) )
]

{ #category : #'command_list' }
AGPUCBindings >> copyBufferToTexture_command_list: command_list buffer: buffer texture: texture copy_region: copy_region [
	^ self ffiCall: #(agpu_error agpuCopyBufferToTexture (agpu_command_list* command_list , agpu_buffer* buffer , agpu_texture* texture , agpu_buffer_image_copy_region* copy_region) )
]

{ #category : #'command_list' }
AGPUCBindings >> copyTextureToBuffer_command_list: command_list texture: texture buffer: buffer copy_region: copy_region [
	^ self ffiCall: #(agpu_error agpuCopyTextureToBuffer (agpu_command_list* command_list , agpu_texture* texture , agpu_buffer* buffer , agpu_buffer_image_copy_region* copy_region) )
]

{ #category : #'command_list' }
AGPUCBindings >> copyTexture_command_list: command_list source_texture: source_texture dest_texture: dest_texture copy_region: copy_region [
	^ self ffiCall: #(agpu_error agpuCopyTexture (agpu_command_list* command_list , agpu_texture* source_texture , agpu_texture* dest_texture , agpu_image_copy_region* copy_region) )
]

{ #category : #'texture' }
AGPUCBindings >> addReference_texture: texture [
	^ self ffiCall: #(agpu_error agpuAddTextureReference (agpu_texture* texture) )
]

{ #category : #'texture' }
AGPUCBindings >> release_texture: texture [
	^ self ffiCall: #(agpu_error agpuReleaseTexture (agpu_texture* texture) )
]

{ #category : #'texture' }
AGPUCBindings >> getDescription_texture: texture description: description [
	^ self ffiCall: #(agpu_error agpuGetTextureDescription (agpu_texture* texture , agpu_texture_description* description) )
]

{ #category : #'texture' }
AGPUCBindings >> mapLevel_texture: texture level: level arrayIndex: arrayIndex flags: flags region: region [
	^ self ffiCall: #(agpu_pointer agpuMapTextureLevel (agpu_texture* texture , agpu_int level , agpu_int arrayIndex , agpu_mapping_access flags , agpu_region3d* region) )
]

{ #category : #'texture' }
AGPUCBindings >> unmapLevel_texture: texture [
	^ self ffiCall: #(agpu_error agpuUnmapTextureLevel (agpu_texture* texture) )
]

{ #category : #'texture' }
AGPUCBindings >> readTextureData_texture: texture level: level arrayIndex: arrayIndex pitch: pitch slicePitch: slicePitch buffer: buffer [
	^ self ffiCall: #(agpu_error agpuReadTextureData (agpu_texture* texture , agpu_int level , agpu_int arrayIndex , agpu_int pitch , agpu_int slicePitch , agpu_pointer buffer) )
]

{ #category : #'texture' }
AGPUCBindings >> readTextureSubData_texture: texture level: level arrayIndex: arrayIndex pitch: pitch slicePitch: slicePitch sourceRegion: sourceRegion destSize: destSize buffer: buffer [
	^ self ffiCall: #(agpu_error agpuReadTextureSubData (agpu_texture* texture , agpu_int level , agpu_int arrayIndex , agpu_int pitch , agpu_int slicePitch , agpu_region3d* sourceRegion , agpu_size3d* destSize , agpu_pointer buffer) )
]

{ #category : #'texture' }
AGPUCBindings >> uploadTextureData_texture: texture level: level arrayIndex: arrayIndex pitch: pitch slicePitch: slicePitch data: data [
	^ self ffiCall: #(agpu_error agpuUploadTextureData (agpu_texture* texture , agpu_int level , agpu_int arrayIndex , agpu_int pitch , agpu_int slicePitch , agpu_pointer data) )
]

{ #category : #'texture' }
AGPUCBindings >> uploadTextureSubData_texture: texture level: level arrayIndex: arrayIndex pitch: pitch slicePitch: slicePitch sourceSize: sourceSize destRegion: destRegion data: data [
	^ self ffiCall: #(agpu_error agpuUploadTextureSubData (agpu_texture* texture , agpu_int level , agpu_int arrayIndex , agpu_int pitch , agpu_int slicePitch , agpu_size3d* sourceSize , agpu_region3d* destRegion , agpu_pointer data) )
]

{ #category : #'texture' }
AGPUCBindings >> getFullViewDescription_texture: texture result: result [
	^ self ffiCall: #(agpu_error agpuGetTextureFullViewDescription (agpu_texture* texture , agpu_texture_view_description* result) )
]

{ #category : #'texture' }
AGPUCBindings >> createView_texture: texture description: description [
	^ self ffiCall: #(agpu_texture_view* agpuCreateTextureView (agpu_texture* texture , agpu_texture_view_description* description) )
]

{ #category : #'texture' }
AGPUCBindings >> getOrCreateFullView_texture: texture [
	^ self ffiCall: #(agpu_texture_view* agpuGetOrCreateFullTextureView (agpu_texture* texture) )
]

{ #category : #'texture_view' }
AGPUCBindings >> addReference_texture_view: texture_view [
	^ self ffiCall: #(agpu_error agpuAddTextureViewReference (agpu_texture_view* texture_view) )
]

{ #category : #'texture_view' }
AGPUCBindings >> release_texture_view: texture_view [
	^ self ffiCall: #(agpu_error agpuReleaseTextureView (agpu_texture_view* texture_view) )
]

{ #category : #'texture_view' }
AGPUCBindings >> getTexture_texture_view: texture_view [
	^ self ffiCall: #(agpu_texture* agpuGetTextureFromView (agpu_texture_view* texture_view) )
]

{ #category : #'sampler' }
AGPUCBindings >> addReference_sampler: sampler [
	^ self ffiCall: #(agpu_error agpuAddSamplerReference (agpu_sampler* sampler) )
]

{ #category : #'sampler' }
AGPUCBindings >> release_sampler: sampler [
	^ self ffiCall: #(agpu_error agpuReleaseSampler (agpu_sampler* sampler) )
]

{ #category : #'buffer' }
AGPUCBindings >> addReference_buffer: buffer [
	^ self ffiCall: #(agpu_error agpuAddBufferReference (agpu_buffer* buffer) )
]

{ #category : #'buffer' }
AGPUCBindings >> release_buffer: buffer [
	^ self ffiCall: #(agpu_error agpuReleaseBuffer (agpu_buffer* buffer) )
]

{ #category : #'buffer' }
AGPUCBindings >> mapBuffer_buffer: buffer flags: flags [
	^ self ffiCall: #(agpu_pointer agpuMapBuffer (agpu_buffer* buffer , agpu_mapping_access flags) )
]

{ #category : #'buffer' }
AGPUCBindings >> unmapBuffer_buffer: buffer [
	^ self ffiCall: #(agpu_error agpuUnmapBuffer (agpu_buffer* buffer) )
]

{ #category : #'buffer' }
AGPUCBindings >> getDescription_buffer: buffer description: description [
	^ self ffiCall: #(agpu_error agpuGetBufferDescription (agpu_buffer* buffer , agpu_buffer_description* description) )
]

{ #category : #'buffer' }
AGPUCBindings >> uploadBufferData_buffer: buffer offset: offset size: size data: data [
	^ self ffiCall: #(agpu_error agpuUploadBufferData (agpu_buffer* buffer , agpu_size offset , agpu_size size , agpu_pointer data) )
]

{ #category : #'buffer' }
AGPUCBindings >> readBufferData_buffer: buffer offset: offset size: size data: data [
	^ self ffiCall: #(agpu_error agpuReadBufferData (agpu_buffer* buffer , agpu_size offset , agpu_size size , agpu_pointer data) )
]

{ #category : #'buffer' }
AGPUCBindings >> flushWholeBuffer_buffer: buffer [
	^ self ffiCall: #(agpu_error agpuFlushWholeBuffer (agpu_buffer* buffer) )
]

{ #category : #'buffer' }
AGPUCBindings >> invalidateWholeBuffer_buffer: buffer [
	^ self ffiCall: #(agpu_error agpuInvalidateWholeBuffer (agpu_buffer* buffer) )
]

{ #category : #'vertex_binding' }
AGPUCBindings >> addReference_vertex_binding: vertex_binding [
	^ self ffiCall: #(agpu_error agpuAddVertexBindingReference (agpu_vertex_binding* vertex_binding) )
]

{ #category : #'vertex_binding' }
AGPUCBindings >> release_vertex_binding: vertex_binding [
	^ self ffiCall: #(agpu_error agpuReleaseVertexBinding (agpu_vertex_binding* vertex_binding) )
]

{ #category : #'vertex_binding' }
AGPUCBindings >> bindVertexBuffers_vertex_binding: vertex_binding count: count vertex_buffers: vertex_buffers [
	^ self ffiCall: #(agpu_error agpuBindVertexBuffers (agpu_vertex_binding* vertex_binding , agpu_uint count , agpu_buffer* vertex_buffers) )
]

{ #category : #'vertex_binding' }
AGPUCBindings >> bindVertexBuffersWithOffsets_vertex_binding: vertex_binding count: count vertex_buffers: vertex_buffers offsets: offsets [
	^ self ffiCall: #(agpu_error agpuBindVertexBuffersWithOffsets (agpu_vertex_binding* vertex_binding , agpu_uint count , agpu_buffer* vertex_buffers , agpu_size* offsets) )
]

{ #category : #'vertex_layout' }
AGPUCBindings >> addReference_vertex_layout: vertex_layout [
	^ self ffiCall: #(agpu_error agpuAddVertexLayoutReference (agpu_vertex_layout* vertex_layout) )
]

{ #category : #'vertex_layout' }
AGPUCBindings >> release_vertex_layout: vertex_layout [
	^ self ffiCall: #(agpu_error agpuReleaseVertexLayout (agpu_vertex_layout* vertex_layout) )
]

{ #category : #'vertex_layout' }
AGPUCBindings >> addVertexAttributeBindings_vertex_layout: vertex_layout vertex_buffer_count: vertex_buffer_count vertex_strides: vertex_strides attribute_count: attribute_count attributes: attributes [
	^ self ffiCall: #(agpu_error agpuAddVertexAttributeBindings (agpu_vertex_layout* vertex_layout , agpu_uint vertex_buffer_count , agpu_size* vertex_strides , agpu_size attribute_count , agpu_vertex_attrib_description* attributes) )
]

{ #category : #'shader' }
AGPUCBindings >> addReference_shader: shader [
	^ self ffiCall: #(agpu_error agpuAddShaderReference (agpu_shader* shader) )
]

{ #category : #'shader' }
AGPUCBindings >> release_shader: shader [
	^ self ffiCall: #(agpu_error agpuReleaseShader (agpu_shader* shader) )
]

{ #category : #'shader' }
AGPUCBindings >> setShaderSource_shader: shader language: language sourceText: sourceText sourceTextLength: sourceTextLength [
	^ self ffiCall: #(agpu_error agpuSetShaderSource (agpu_shader* shader , agpu_shader_language language , agpu_string sourceText , agpu_string_length sourceTextLength) )
]

{ #category : #'shader' }
AGPUCBindings >> compileShader_shader: shader options: options [
	^ self ffiCall: #(agpu_error agpuCompileShader (agpu_shader* shader , agpu_cstring options) )
]

{ #category : #'shader' }
AGPUCBindings >> getCompilationLogLength_shader: shader [
	^ self ffiCall: #(agpu_size agpuGetShaderCompilationLogLength (agpu_shader* shader) )
]

{ #category : #'shader' }
AGPUCBindings >> getCompilationLog_shader: shader buffer_size: buffer_size buffer: buffer [
	^ self ffiCall: #(agpu_error agpuGetShaderCompilationLog (agpu_shader* shader , agpu_size buffer_size , agpu_string_buffer buffer) )
]

{ #category : #'framebuffer' }
AGPUCBindings >> addReference_framebuffer: framebuffer [
	^ self ffiCall: #(agpu_error agpuAddFramebufferReference (agpu_framebuffer* framebuffer) )
]

{ #category : #'framebuffer' }
AGPUCBindings >> release_framebuffer: framebuffer [
	^ self ffiCall: #(agpu_error agpuReleaseFramebuffer (agpu_framebuffer* framebuffer) )
]

{ #category : #'framebuffer' }
AGPUCBindings >> getWidth_framebuffer: framebuffer [
	^ self ffiCall: #(agpu_uint agpuGetFramebufferWidth (agpu_framebuffer* framebuffer) )
]

{ #category : #'framebuffer' }
AGPUCBindings >> getHeight_framebuffer: framebuffer [
	^ self ffiCall: #(agpu_uint agpuGetFramebufferHeight (agpu_framebuffer* framebuffer) )
]

{ #category : #'renderpass' }
AGPUCBindings >> addReference_renderpass: renderpass [
	^ self ffiCall: #(agpu_error agpuAddRenderPassReference (agpu_renderpass* renderpass) )
]

{ #category : #'renderpass' }
AGPUCBindings >> release_renderpass: renderpass [
	^ self ffiCall: #(agpu_error agpuReleaseRenderPass (agpu_renderpass* renderpass) )
]

{ #category : #'renderpass' }
AGPUCBindings >> setDepthStencilClearValue_renderpass: renderpass value: value [
	^ self ffiCall: #(agpu_error agpuSetDepthStencilClearValue (agpu_renderpass* renderpass , agpu_depth_stencil_value value) )
]

{ #category : #'renderpass' }
AGPUCBindings >> setColorClearValue_renderpass: renderpass attachment_index: attachment_index value: value [
	^ self ffiCall: #(agpu_error agpuSetColorClearValue (agpu_renderpass* renderpass , agpu_uint attachment_index , agpu_color4f value) )
]

{ #category : #'renderpass' }
AGPUCBindings >> setColorClearValueFrom_renderpass: renderpass attachment_index: attachment_index value: value [
	^ self ffiCall: #(agpu_error agpuSetColorClearValueFrom (agpu_renderpass* renderpass , agpu_uint attachment_index , agpu_color4f* value) )
]

{ #category : #'renderpass' }
AGPUCBindings >> getColorAttachmentFormats_renderpass: renderpass color_attachment_count: color_attachment_count formats: formats [
	^ self ffiCall: #(agpu_error agpuGetRenderPassColorAttachmentFormats (agpu_renderpass* renderpass , agpu_uint* color_attachment_count , agpu_texture_format* formats) )
]

{ #category : #'renderpass' }
AGPUCBindings >> getDepthStencilAttachmentFormat_renderpass: renderpass [
	^ self ffiCall: #(agpu_texture_format agpuGetRenderPassDepthStencilAttachmentFormat (agpu_renderpass* renderpass) )
]

{ #category : #'renderpass' }
AGPUCBindings >> getSampleCount_renderpass: renderpass [
	^ self ffiCall: #(agpu_uint agpuGetRenderPassSampleCount (agpu_renderpass* renderpass) )
]

{ #category : #'renderpass' }
AGPUCBindings >> getSampleQuality_renderpass: renderpass [
	^ self ffiCall: #(agpu_uint agpuGetRenderPassSampleQuality (agpu_renderpass* renderpass) )
]

{ #category : #'shader_signature_builder' }
AGPUCBindings >> addReference_shader_signature_builder: shader_signature_builder [
	^ self ffiCall: #(agpu_error agpuAddShaderSignatureBuilderReference (agpu_shader_signature_builder* shader_signature_builder) )
]

{ #category : #'shader_signature_builder' }
AGPUCBindings >> release_shader_signature_builder: shader_signature_builder [
	^ self ffiCall: #(agpu_error agpuReleaseShaderSignatureBuilder (agpu_shader_signature_builder* shader_signature_builder) )
]

{ #category : #'shader_signature_builder' }
AGPUCBindings >> build_shader_signature_builder: shader_signature_builder [
	^ self ffiCall: #(agpu_shader_signature* agpuBuildShaderSignature (agpu_shader_signature_builder* shader_signature_builder) )
]

{ #category : #'shader_signature_builder' }
AGPUCBindings >> addBindingConstant_shader_signature_builder: shader_signature_builder [
	^ self ffiCall: #(agpu_error agpuAddShaderSignatureBindingConstant (agpu_shader_signature_builder* shader_signature_builder) )
]

{ #category : #'shader_signature_builder' }
AGPUCBindings >> addBindingElement_shader_signature_builder: shader_signature_builder type: type maxBindings: maxBindings [
	^ self ffiCall: #(agpu_error agpuAddShaderSignatureBindingElement (agpu_shader_signature_builder* shader_signature_builder , agpu_shader_binding_type type , agpu_uint maxBindings) )
]

{ #category : #'shader_signature_builder' }
AGPUCBindings >> beginBindingBank_shader_signature_builder: shader_signature_builder maxBindings: maxBindings [
	^ self ffiCall: #(agpu_error agpuBeginShaderSignatureBindingBank (agpu_shader_signature_builder* shader_signature_builder , agpu_uint maxBindings) )
]

{ #category : #'shader_signature_builder' }
AGPUCBindings >> addBindingBankElement_shader_signature_builder: shader_signature_builder type: type bindingPointCount: bindingPointCount [
	^ self ffiCall: #(agpu_error agpuAddShaderSignatureBindingBankElement (agpu_shader_signature_builder* shader_signature_builder , agpu_shader_binding_type type , agpu_uint bindingPointCount) )
]

{ #category : #'shader_signature_builder' }
AGPUCBindings >> addBindingBankArray_shader_signature_builder: shader_signature_builder type: type size: size [
	^ self ffiCall: #(agpu_error agpuAddShaderSignatureBindingBankArray (agpu_shader_signature_builder* shader_signature_builder , agpu_shader_binding_type type , agpu_uint size) )
]

{ #category : #'shader_signature' }
AGPUCBindings >> addReference_shader_signature: shader_signature [
	^ self ffiCall: #(agpu_error agpuAddShaderSignature (agpu_shader_signature* shader_signature) )
]

{ #category : #'shader_signature' }
AGPUCBindings >> release_shader_signature: shader_signature [
	^ self ffiCall: #(agpu_error agpuReleaseShaderSignature (agpu_shader_signature* shader_signature) )
]

{ #category : #'shader_signature' }
AGPUCBindings >> createShaderResourceBinding_shader_signature: shader_signature element: element [
	^ self ffiCall: #(agpu_shader_resource_binding* agpuCreateShaderResourceBinding (agpu_shader_signature* shader_signature , agpu_uint element) )
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> addReference_shader_resource_binding: shader_resource_binding [
	^ self ffiCall: #(agpu_error agpuAddShaderResourceBindingReference (agpu_shader_resource_binding* shader_resource_binding) )
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> release_shader_resource_binding: shader_resource_binding [
	^ self ffiCall: #(agpu_error agpuReleaseShaderResourceBinding (agpu_shader_resource_binding* shader_resource_binding) )
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> bindUniformBuffer_shader_resource_binding: shader_resource_binding location: location uniform_buffer: uniform_buffer [
	^ self ffiCall: #(agpu_error agpuBindUniformBuffer (agpu_shader_resource_binding* shader_resource_binding , agpu_int location , agpu_buffer* uniform_buffer) )
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> bindUniformBufferRange_shader_resource_binding: shader_resource_binding location: location uniform_buffer: uniform_buffer offset: offset size: size [
	^ self ffiCall: #(agpu_error agpuBindUniformBufferRange (agpu_shader_resource_binding* shader_resource_binding , agpu_int location , agpu_buffer* uniform_buffer , agpu_size offset , agpu_size size) )
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> bindStorageBuffer_shader_resource_binding: shader_resource_binding location: location storage_buffer: storage_buffer [
	^ self ffiCall: #(agpu_error agpuBindStorageBuffer (agpu_shader_resource_binding* shader_resource_binding , agpu_int location , agpu_buffer* storage_buffer) )
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> bindStorageBufferRange_shader_resource_binding: shader_resource_binding location: location storage_buffer: storage_buffer offset: offset size: size [
	^ self ffiCall: #(agpu_error agpuBindStorageBufferRange (agpu_shader_resource_binding* shader_resource_binding , agpu_int location , agpu_buffer* storage_buffer , agpu_size offset , agpu_size size) )
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> bindSampledTextureView_shader_resource_binding: shader_resource_binding location: location view: view [
	^ self ffiCall: #(agpu_error agpuBindSampledTextureView (agpu_shader_resource_binding* shader_resource_binding , agpu_int location , agpu_texture_view* view) )
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> bindStorageImageView_shader_resource_binding: shader_resource_binding location: location view: view [
	^ self ffiCall: #(agpu_error agpuBindStorageImageView (agpu_shader_resource_binding* shader_resource_binding , agpu_int location , agpu_texture_view* view) )
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> bindSampler_shader_resource_binding: shader_resource_binding location: location sampler: sampler [
	^ self ffiCall: #(agpu_error agpuBindSampler (agpu_shader_resource_binding* shader_resource_binding , agpu_int location , agpu_sampler* sampler) )
]

{ #category : #'fence' }
AGPUCBindings >> addReference_fence: fence [
	^ self ffiCall: #(agpu_error agpuAddFenceReference (agpu_fence* fence) )
]

{ #category : #'fence' }
AGPUCBindings >> release_fence: fence [
	^ self ffiCall: #(agpu_error agpuReleaseFenceReference (agpu_fence* fence) )
]

{ #category : #'fence' }
AGPUCBindings >> waitOnClient_fence: fence [
	^ self ffiCall: #(agpu_error agpuWaitOnClient (agpu_fence* fence) )
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> addReference_offline_shader_compiler: offline_shader_compiler [
	^ self ffiCall: #(agpu_error agpuAddOfflineShaderCompilerReference (agpu_offline_shader_compiler* offline_shader_compiler) )
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> release_offline_shader_compiler: offline_shader_compiler [
	^ self ffiCall: #(agpu_error agpuReleaseOfflineShaderCompiler (agpu_offline_shader_compiler* offline_shader_compiler) )
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> isShaderLanguageSupported_offline_shader_compiler: offline_shader_compiler language: language [
	^ self ffiCall: #(agpu_bool agpuIsShaderLanguageSupportedByOfflineCompiler (agpu_offline_shader_compiler* offline_shader_compiler , agpu_shader_language language) )
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> isTargetShaderLanguageSupported_offline_shader_compiler: offline_shader_compiler language: language [
	^ self ffiCall: #(agpu_bool agpuIsTargetShaderLanguageSupportedByOfflineCompiler (agpu_offline_shader_compiler* offline_shader_compiler , agpu_shader_language language) )
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> setShaderSource_offline_shader_compiler: offline_shader_compiler language: language stage: stage sourceText: sourceText sourceTextLength: sourceTextLength [
	^ self ffiCall: #(agpu_error agpuSetOfflineShaderCompilerSource (agpu_offline_shader_compiler* offline_shader_compiler , agpu_shader_language language , agpu_shader_type stage , agpu_string sourceText , agpu_string_length sourceTextLength) )
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> compileShader_offline_shader_compiler: offline_shader_compiler target_language: target_language options: options [
	^ self ffiCall: #(agpu_error agpuCompileOfflineShader (agpu_offline_shader_compiler* offline_shader_compiler , agpu_shader_language target_language , agpu_cstring options) )
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> getCompilationLogLength_offline_shader_compiler: offline_shader_compiler [
	^ self ffiCall: #(agpu_size agpuGetOfflineShaderCompilationLogLength (agpu_offline_shader_compiler* offline_shader_compiler) )
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> getCompilationLog_offline_shader_compiler: offline_shader_compiler buffer_size: buffer_size buffer: buffer [
	^ self ffiCall: #(agpu_error agpuGetOfflineShaderCompilationLog (agpu_offline_shader_compiler* offline_shader_compiler , agpu_size buffer_size , agpu_string_buffer buffer) )
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> getCompilationResultLength_offline_shader_compiler: offline_shader_compiler [
	^ self ffiCall: #(agpu_size agpuGetOfflineShaderCompilationResultLength (agpu_offline_shader_compiler* offline_shader_compiler) )
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> getCompilationResult_offline_shader_compiler: offline_shader_compiler buffer_size: buffer_size buffer: buffer [
	^ self ffiCall: #(agpu_error agpuGetOfflineShaderCompilationResult (agpu_offline_shader_compiler* offline_shader_compiler , agpu_size buffer_size , agpu_string_buffer buffer) )
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> getResultAsShader_offline_shader_compiler: offline_shader_compiler [
	^ self ffiCall: #(agpu_shader* agpuGetOfflineShaderCompilerResultAsShader (agpu_offline_shader_compiler* offline_shader_compiler) )
]

{ #category : #'state_tracker_cache' }
AGPUCBindings >> addReference_state_tracker_cache: state_tracker_cache [
	^ self ffiCall: #(agpu_error agpuAddStateTrackerCacheReference (agpu_state_tracker_cache* state_tracker_cache) )
]

{ #category : #'state_tracker_cache' }
AGPUCBindings >> release_state_tracker_cache: state_tracker_cache [
	^ self ffiCall: #(agpu_error agpuReleaseStateTrackerCacheReference (agpu_state_tracker_cache* state_tracker_cache) )
]

{ #category : #'state_tracker_cache' }
AGPUCBindings >> createStateTracker_state_tracker_cache: state_tracker_cache type: type command_queue: command_queue [
	^ self ffiCall: #(agpu_state_tracker* agpuCreateStateTracker (agpu_state_tracker_cache* state_tracker_cache , agpu_command_list_type type , agpu_command_queue* command_queue) )
]

{ #category : #'state_tracker_cache' }
AGPUCBindings >> createStateTrackerWithCommandAllocator_state_tracker_cache: state_tracker_cache type: type command_queue: command_queue command_allocator: command_allocator [
	^ self ffiCall: #(agpu_state_tracker* agpuCreateStateTrackerWithCommandAllocator (agpu_state_tracker_cache* state_tracker_cache , agpu_command_list_type type , agpu_command_queue* command_queue , agpu_command_allocator* command_allocator) )
]

{ #category : #'state_tracker_cache' }
AGPUCBindings >> createStateTrackerWithFrameBuffering_state_tracker_cache: state_tracker_cache type: type command_queue: command_queue framebuffering_count: framebuffering_count [
	^ self ffiCall: #(agpu_state_tracker* agpuCreateStateTrackerWithFrameBuffering (agpu_state_tracker_cache* state_tracker_cache , agpu_command_list_type type , agpu_command_queue* command_queue , agpu_uint framebuffering_count) )
]

{ #category : #'state_tracker_cache' }
AGPUCBindings >> createImmediateRenderer_state_tracker_cache: state_tracker_cache [
	^ self ffiCall: #(agpu_immediate_renderer* agpuCreateImmediateRenderer (agpu_state_tracker_cache* state_tracker_cache) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> addReference_state_tracker: state_tracker [
	^ self ffiCall: #(agpu_error agpuAddStateTrackerReference (agpu_state_tracker* state_tracker) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> release_state_tracker: state_tracker [
	^ self ffiCall: #(agpu_error agpuReleaseStateTrackerReference (agpu_state_tracker* state_tracker) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> beginRecordingCommands_state_tracker: state_tracker [
	^ self ffiCall: #(agpu_error agpuStateTrackerBeginRecordingCommands (agpu_state_tracker* state_tracker) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> endRecordingCommands_state_tracker: state_tracker [
	^ self ffiCall: #(agpu_command_list* agpuStateTrackerEndRecordingCommands (agpu_state_tracker* state_tracker) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> endRecordingAndFlushCommands_state_tracker: state_tracker [
	^ self ffiCall: #(agpu_error agpuStateTrackerEndRecordingAndFlushCommands (agpu_state_tracker* state_tracker) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> reset_state_tracker: state_tracker [
	^ self ffiCall: #(agpu_error agpuStateTrackerReset (agpu_state_tracker* state_tracker) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> resetGraphicsPipeline_state_tracker: state_tracker [
	^ self ffiCall: #(agpu_error agpuStateTrackerResetGraphicsPipeline (agpu_state_tracker* state_tracker) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> resetComputePipeline_state_tracker: state_tracker [
	^ self ffiCall: #(agpu_error agpuStateTrackerResetComputePipeline (agpu_state_tracker* state_tracker) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setComputeStage_state_tracker: state_tracker shader: shader entryPoint: entryPoint [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetComputeStage (agpu_state_tracker* state_tracker , agpu_shader* shader , agpu_cstring entryPoint) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setComputeStageWithMain_state_tracker: state_tracker shader: shader [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetComputeStageWithMain (agpu_state_tracker* state_tracker , agpu_shader* shader) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setVertexStage_state_tracker: state_tracker shader: shader entryPoint: entryPoint [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetVertexStage (agpu_state_tracker* state_tracker , agpu_shader* shader , agpu_cstring entryPoint) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setVertexStageWithMain_state_tracker: state_tracker shader: shader [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetVertexStageWithMain (agpu_state_tracker* state_tracker , agpu_shader* shader) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setFragmentStage_state_tracker: state_tracker shader: shader entryPoint: entryPoint [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetFragmentStage (agpu_state_tracker* state_tracker , agpu_shader* shader , agpu_cstring entryPoint) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setFragmentStageWithMain_state_tracker: state_tracker shader: shader [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetFragmentStageWithMain (agpu_state_tracker* state_tracker , agpu_shader* shader) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setGeometryStage_state_tracker: state_tracker shader: shader entryPoint: entryPoint [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetGeometryStage (agpu_state_tracker* state_tracker , agpu_shader* shader , agpu_cstring entryPoint) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setGeometryStageWithMain_state_tracker: state_tracker shader: shader [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetGeometryStageWithMain (agpu_state_tracker* state_tracker , agpu_shader* shader) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setTessellationControlStage_state_tracker: state_tracker shader: shader entryPoint: entryPoint [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetTessellationControlStage (agpu_state_tracker* state_tracker , agpu_shader* shader , agpu_cstring entryPoint) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setTessellationControlStageWithMain_state_tracker: state_tracker shader: shader [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetTessellationControlStageWithMain (agpu_state_tracker* state_tracker , agpu_shader* shader) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setTessellationEvaluationStage_state_tracker: state_tracker shader: shader entryPoint: entryPoint [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetTessellationEvaluationStage (agpu_state_tracker* state_tracker , agpu_shader* shader , agpu_cstring entryPoint) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setTessellationEvaluationStageWithMain_state_tracker: state_tracker shader: shader [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetTessellationEvaluationStageWithMain (agpu_state_tracker* state_tracker , agpu_shader* shader) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setBlendState_state_tracker: state_tracker renderTargetMask: renderTargetMask enabled: enabled [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetBlendState (agpu_state_tracker* state_tracker , agpu_int renderTargetMask , agpu_bool enabled) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setBlendFunction_state_tracker: state_tracker renderTargetMask: renderTargetMask sourceFactor: sourceFactor destFactor: destFactor colorOperation: colorOperation sourceAlphaFactor: sourceAlphaFactor destAlphaFactor: destAlphaFactor alphaOperation: alphaOperation [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetBlendFunction (agpu_state_tracker* state_tracker , agpu_int renderTargetMask , agpu_blending_factor sourceFactor , agpu_blending_factor destFactor , agpu_blending_operation colorOperation , agpu_blending_factor sourceAlphaFactor , agpu_blending_factor destAlphaFactor , agpu_blending_operation alphaOperation) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setColorMask_state_tracker: state_tracker renderTargetMask: renderTargetMask redEnabled: redEnabled greenEnabled: greenEnabled blueEnabled: blueEnabled alphaEnabled: alphaEnabled [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetColorMask (agpu_state_tracker* state_tracker , agpu_int renderTargetMask , agpu_bool redEnabled , agpu_bool greenEnabled , agpu_bool blueEnabled , agpu_bool alphaEnabled) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setFrontFace_state_tracker: state_tracker winding: winding [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetFrontFace (agpu_state_tracker* state_tracker , agpu_face_winding winding) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setCullMode_state_tracker: state_tracker mode: mode [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetCullMode (agpu_state_tracker* state_tracker , agpu_cull_mode mode) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setDepthBias_state_tracker: state_tracker constant_factor: constant_factor clamp: clamp slope_factor: slope_factor [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetDepthBias (agpu_state_tracker* state_tracker , agpu_float constant_factor , agpu_float clamp , agpu_float slope_factor) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setDepthState_state_tracker: state_tracker enabled: enabled writeMask: writeMask function: function [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetDepthState (agpu_state_tracker* state_tracker , agpu_bool enabled , agpu_bool writeMask , agpu_compare_function function) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setPolygonMode_state_tracker: state_tracker mode: mode [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetPolygonMode (agpu_state_tracker* state_tracker , agpu_polygon_mode mode) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setStencilState_state_tracker: state_tracker enabled: enabled writeMask: writeMask readMask: readMask [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetStencilState (agpu_state_tracker* state_tracker , agpu_bool enabled , agpu_int writeMask , agpu_int readMask) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setStencilFrontFace_state_tracker: state_tracker stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetStencilFrontFace (agpu_state_tracker* state_tracker , agpu_stencil_operation stencilFailOperation , agpu_stencil_operation depthFailOperation , agpu_stencil_operation stencilDepthPassOperation , agpu_compare_function stencilFunction) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setStencilBackFace_state_tracker: state_tracker stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetStencilBackFace (agpu_state_tracker* state_tracker , agpu_stencil_operation stencilFailOperation , agpu_stencil_operation depthFailOperation , agpu_stencil_operation stencilDepthPassOperation , agpu_compare_function stencilFunction) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setPrimitiveType_state_tracker: state_tracker type: type [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetPrimitiveType (agpu_state_tracker* state_tracker , agpu_primitive_topology type) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setVertexLayout_state_tracker: state_tracker layout: layout [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetVertexLayout (agpu_state_tracker* state_tracker , agpu_vertex_layout* layout) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setShaderSignature_state_tracker: state_tracker signature: signature [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetShaderSignature (agpu_state_tracker* state_tracker , agpu_shader_signature* signature) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setSampleDescription_state_tracker: state_tracker sample_count: sample_count sample_quality: sample_quality [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetSampleDescription (agpu_state_tracker* state_tracker , agpu_uint sample_count , agpu_uint sample_quality) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setViewport_state_tracker: state_tracker x: x y: y w: w h: h [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetViewport (agpu_state_tracker* state_tracker , agpu_int x , agpu_int y , agpu_int w , agpu_int h) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setScissor_state_tracker: state_tracker x: x y: y w: w h: h [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetScissor (agpu_state_tracker* state_tracker , agpu_int x , agpu_int y , agpu_int w , agpu_int h) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> useVertexBinding_state_tracker: state_tracker vertex_binding: vertex_binding [
	^ self ffiCall: #(agpu_error agpuStateTrackerUseVertexBinding (agpu_state_tracker* state_tracker , agpu_vertex_binding* vertex_binding) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> useIndexBuffer_state_tracker: state_tracker index_buffer: index_buffer [
	^ self ffiCall: #(agpu_error agpuStateTrackerUseIndexBuffer (agpu_state_tracker* state_tracker , agpu_buffer* index_buffer) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> useIndexBufferAt_state_tracker: state_tracker index_buffer: index_buffer offset: offset index_size: index_size [
	^ self ffiCall: #(agpu_error agpuStateTrackerUseIndexBufferAt (agpu_state_tracker* state_tracker , agpu_buffer* index_buffer , agpu_size offset , agpu_size index_size) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> useDrawIndirectBuffer_state_tracker: state_tracker draw_buffer: draw_buffer [
	^ self ffiCall: #(agpu_error agpuStateTrackerUseDrawIndirectBuffer (agpu_state_tracker* state_tracker , agpu_buffer* draw_buffer) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> useComputeDispatchIndirectBuffer_state_tracker: state_tracker buffer: buffer [
	^ self ffiCall: #(agpu_error agpuStateTrackerUseComputeDispatchIndirectBuffer (agpu_state_tracker* state_tracker , agpu_buffer* buffer) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> useShaderResources_state_tracker: state_tracker binding: binding [
	^ self ffiCall: #(agpu_error agpuStateTrackerUseShaderResources (agpu_state_tracker* state_tracker , agpu_shader_resource_binding* binding) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> useShaderResourcesInSlot_state_tracker: state_tracker binding: binding slot: slot [
	^ self ffiCall: #(agpu_error agpuStateTrackerUseShaderResourcesInSlot (agpu_state_tracker* state_tracker , agpu_shader_resource_binding* binding , agpu_uint slot) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> useComputeShaderResources_state_tracker: state_tracker binding: binding [
	^ self ffiCall: #(agpu_error agpuStateTrackerUseComputeShaderResources (agpu_state_tracker* state_tracker , agpu_shader_resource_binding* binding) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> useComputeShaderResourcesInSlot_state_tracker: state_tracker binding: binding slot: slot [
	^ self ffiCall: #(agpu_error agpuStateTrackerUseComputeShaderResourcesInSlot (agpu_state_tracker* state_tracker , agpu_shader_resource_binding* binding , agpu_uint slot) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> drawArrays_state_tracker: state_tracker vertex_count: vertex_count instance_count: instance_count first_vertex: first_vertex base_instance: base_instance [
	^ self ffiCall: #(agpu_error agpuStateTrackerDrawArrays (agpu_state_tracker* state_tracker , agpu_uint vertex_count , agpu_uint instance_count , agpu_uint first_vertex , agpu_uint base_instance) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> drawArraysIndirect_state_tracker: state_tracker offset: offset drawcount: drawcount [
	^ self ffiCall: #(agpu_error agpuStateTrackerDrawArraysIndirect (agpu_state_tracker* state_tracker , agpu_size offset , agpu_size drawcount) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> drawElements_state_tracker: state_tracker index_count: index_count instance_count: instance_count first_index: first_index base_vertex: base_vertex base_instance: base_instance [
	^ self ffiCall: #(agpu_error agpuStateTrackerDrawElements (agpu_state_tracker* state_tracker , agpu_uint index_count , agpu_uint instance_count , agpu_uint first_index , agpu_int base_vertex , agpu_uint base_instance) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> drawElementsIndirect_state_tracker: state_tracker offset: offset drawcount: drawcount [
	^ self ffiCall: #(agpu_error agpuStateTrackerDrawElementsIndirect (agpu_state_tracker* state_tracker , agpu_size offset , agpu_size drawcount) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> dispatchCompute_state_tracker: state_tracker group_count_x: group_count_x group_count_y: group_count_y group_count_z: group_count_z [
	^ self ffiCall: #(agpu_error agpuStateTrackerDispatchCompute (agpu_state_tracker* state_tracker , agpu_uint group_count_x , agpu_uint group_count_y , agpu_uint group_count_z) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> dispatchComputeIndirect_state_tracker: state_tracker offset: offset [
	^ self ffiCall: #(agpu_error agpuStateTrackerDispatchComputeIndirect (agpu_state_tracker* state_tracker , agpu_size offset) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> setStencilReference_state_tracker: state_tracker reference: reference [
	^ self ffiCall: #(agpu_error agpuStateTrackerSetStencilReference (agpu_state_tracker* state_tracker , agpu_uint reference) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> executeBundle_state_tracker: state_tracker bundle: bundle [
	^ self ffiCall: #(agpu_error agpuStateTrackerExecuteBundle (agpu_state_tracker* state_tracker , agpu_command_list* bundle) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> beginRenderPass_state_tracker: state_tracker renderpass: renderpass framebuffer: framebuffer bundle_content: bundle_content [
	^ self ffiCall: #(agpu_error agpuStateTrackerBeginRenderPass (agpu_state_tracker* state_tracker , agpu_renderpass* renderpass , agpu_framebuffer* framebuffer , agpu_bool bundle_content) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> endRenderPass_state_tracker: state_tracker [
	^ self ffiCall: #(agpu_error agpuStateTrackerEndRenderPass (agpu_state_tracker* state_tracker) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> resolveFramebuffer_state_tracker: state_tracker destFramebuffer: destFramebuffer sourceFramebuffer: sourceFramebuffer [
	^ self ffiCall: #(agpu_error agpuStateTrackerResolveFramebuffer (agpu_state_tracker* state_tracker , agpu_framebuffer* destFramebuffer , agpu_framebuffer* sourceFramebuffer) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> resolveTexture_state_tracker: state_tracker sourceTexture: sourceTexture sourceLevel: sourceLevel sourceLayer: sourceLayer destTexture: destTexture destLevel: destLevel destLayer: destLayer levelCount: levelCount layerCount: layerCount aspect: aspect [
	^ self ffiCall: #(agpu_error agpuStateTrackerResolveTexture (agpu_state_tracker* state_tracker , agpu_texture* sourceTexture , agpu_uint sourceLevel , agpu_uint sourceLayer , agpu_texture* destTexture , agpu_uint destLevel , agpu_uint destLayer , agpu_uint levelCount , agpu_uint layerCount , agpu_texture_aspect aspect) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> pushConstants_state_tracker: state_tracker offset: offset size: size values: values [
	^ self ffiCall: #(agpu_error agpuStateTrackerPushConstants (agpu_state_tracker* state_tracker , agpu_uint offset , agpu_uint size , agpu_pointer values) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> memoryBarrier_state_tracker: state_tracker source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses [
	^ self ffiCall: #(agpu_error agpuStateTrackerMemoryBarrier (agpu_state_tracker* state_tracker , agpu_pipeline_stage_flags source_stage , agpu_pipeline_stage_flags dest_stage , agpu_access_flags source_accesses , agpu_access_flags dest_accesses) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> bufferMemoryBarrier_state_tracker: state_tracker buffer: buffer source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses offset: offset size: size [
	^ self ffiCall: #(agpu_error agpuStateTrackerBufferMemoryBarrier (agpu_state_tracker* state_tracker , agpu_buffer* buffer , agpu_pipeline_stage_flags source_stage , agpu_pipeline_stage_flags dest_stage , agpu_access_flags source_accesses , agpu_access_flags dest_accesses , agpu_size offset , agpu_size size) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> textureMemoryBarrier_state_tracker: state_tracker texture: texture source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses old_usage: old_usage new_usage: new_usage subresource_range: subresource_range [
	^ self ffiCall: #(agpu_error agpuStateTrackerTextureMemoryBarrier (agpu_state_tracker* state_tracker , agpu_texture* texture , agpu_pipeline_stage_flags source_stage , agpu_pipeline_stage_flags dest_stage , agpu_access_flags source_accesses , agpu_access_flags dest_accesses , agpu_texture_usage_mode_mask old_usage , agpu_texture_usage_mode_mask new_usage , agpu_texture_subresource_range* subresource_range) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> pushBufferTransitionBarrier_state_tracker: state_tracker buffer: buffer old_usage: old_usage new_usage: new_usage [
	^ self ffiCall: #(agpu_error agpuStateTrackerPushBufferTransitionBarrier (agpu_state_tracker* state_tracker , agpu_buffer* buffer , agpu_buffer_usage_mask old_usage , agpu_buffer_usage_mask new_usage) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> pushTextureTransitionBarrier_state_tracker: state_tracker texture: texture old_usage: old_usage new_usage: new_usage subresource_range: subresource_range [
	^ self ffiCall: #(agpu_error agpuStateTrackerPushTextureTransitionBarrier (agpu_state_tracker* state_tracker , agpu_texture* texture , agpu_texture_usage_mode_mask old_usage , agpu_texture_usage_mode_mask new_usage , agpu_texture_subresource_range* subresource_range) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> popBufferTransitionBarrier_state_tracker: state_tracker [
	^ self ffiCall: #(agpu_error agpuStateTrackerPopBufferTransitionBarrier (agpu_state_tracker* state_tracker) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> popTextureTransitionBarrier_state_tracker: state_tracker [
	^ self ffiCall: #(agpu_error agpuStateTrackerPopTextureTransitionBarrier (agpu_state_tracker* state_tracker) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> copyBuffer_state_tracker: state_tracker source_buffer: source_buffer source_offset: source_offset dest_buffer: dest_buffer dest_offset: dest_offset copy_size: copy_size [
	^ self ffiCall: #(agpu_error agpuStateTrackerCopyBuffer (agpu_state_tracker* state_tracker , agpu_buffer* source_buffer , agpu_size source_offset , agpu_buffer* dest_buffer , agpu_size dest_offset , agpu_size copy_size) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> copyBufferToTexture_state_tracker: state_tracker buffer: buffer texture: texture copy_region: copy_region [
	^ self ffiCall: #(agpu_error agpuStateTrackerCopyBufferToTexture (agpu_state_tracker* state_tracker , agpu_buffer* buffer , agpu_texture* texture , agpu_buffer_image_copy_region* copy_region) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> copyTextureToBuffer_state_tracker: state_tracker texture: texture buffer: buffer copy_region: copy_region [
	^ self ffiCall: #(agpu_error agpuStateTrackerCopyTextureToBuffer (agpu_state_tracker* state_tracker , agpu_texture* texture , agpu_buffer* buffer , agpu_buffer_image_copy_region* copy_region) )
]

{ #category : #'state_tracker' }
AGPUCBindings >> copyTexture_state_tracker: state_tracker source_texture: source_texture dest_texture: dest_texture copy_region: copy_region [
	^ self ffiCall: #(agpu_error agpuStateTrackerCopyTexture (agpu_state_tracker* state_tracker , agpu_texture* source_texture , agpu_texture* dest_texture , agpu_image_copy_region* copy_region) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> addReference_immediate_renderer: immediate_renderer [
	^ self ffiCall: #(agpu_error agpuAddImmediateRendererReference (agpu_immediate_renderer* immediate_renderer) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> release_immediate_renderer: immediate_renderer [
	^ self ffiCall: #(agpu_error agpuReleaseImmediateRendererReference (agpu_immediate_renderer* immediate_renderer) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> beginRendering_immediate_renderer: immediate_renderer state_tracker: state_tracker [
	^ self ffiCall: #(agpu_error agpuBeginImmediateRendering (agpu_immediate_renderer* immediate_renderer , agpu_state_tracker* state_tracker) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> endRendering_immediate_renderer: immediate_renderer [
	^ self ffiCall: #(agpu_error agpuEndImmediateRendering (agpu_immediate_renderer* immediate_renderer) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setBlendState_immediate_renderer: immediate_renderer renderTargetMask: renderTargetMask enabled: enabled [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetBlendState (agpu_immediate_renderer* immediate_renderer , agpu_int renderTargetMask , agpu_bool enabled) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setBlendFunction_immediate_renderer: immediate_renderer renderTargetMask: renderTargetMask sourceFactor: sourceFactor destFactor: destFactor colorOperation: colorOperation sourceAlphaFactor: sourceAlphaFactor destAlphaFactor: destAlphaFactor alphaOperation: alphaOperation [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetBlendFunction (agpu_immediate_renderer* immediate_renderer , agpu_int renderTargetMask , agpu_blending_factor sourceFactor , agpu_blending_factor destFactor , agpu_blending_operation colorOperation , agpu_blending_factor sourceAlphaFactor , agpu_blending_factor destAlphaFactor , agpu_blending_operation alphaOperation) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setColorMask_immediate_renderer: immediate_renderer renderTargetMask: renderTargetMask redEnabled: redEnabled greenEnabled: greenEnabled blueEnabled: blueEnabled alphaEnabled: alphaEnabled [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetColorMask (agpu_immediate_renderer* immediate_renderer , agpu_int renderTargetMask , agpu_bool redEnabled , agpu_bool greenEnabled , agpu_bool blueEnabled , agpu_bool alphaEnabled) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setFrontFace_immediate_renderer: immediate_renderer winding: winding [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetFrontFace (agpu_immediate_renderer* immediate_renderer , agpu_face_winding winding) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setCullMode_immediate_renderer: immediate_renderer mode: mode [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetCullMode (agpu_immediate_renderer* immediate_renderer , agpu_cull_mode mode) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setDepthBias_immediate_renderer: immediate_renderer constant_factor: constant_factor clamp: clamp slope_factor: slope_factor [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetDepthBias (agpu_immediate_renderer* immediate_renderer , agpu_float constant_factor , agpu_float clamp , agpu_float slope_factor) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setDepthState_immediate_renderer: immediate_renderer enabled: enabled writeMask: writeMask function: function [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetDepthState (agpu_immediate_renderer* immediate_renderer , agpu_bool enabled , agpu_bool writeMask , agpu_compare_function function) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setPolygonMode_immediate_renderer: immediate_renderer mode: mode [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetPolygonMode (agpu_immediate_renderer* immediate_renderer , agpu_polygon_mode mode) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setStencilState_immediate_renderer: immediate_renderer enabled: enabled writeMask: writeMask readMask: readMask [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetStencilState (agpu_immediate_renderer* immediate_renderer , agpu_bool enabled , agpu_int writeMask , agpu_int readMask) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setStencilFrontFace_immediate_renderer: immediate_renderer stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetStencilFrontFace (agpu_immediate_renderer* immediate_renderer , agpu_stencil_operation stencilFailOperation , agpu_stencil_operation depthFailOperation , agpu_stencil_operation stencilDepthPassOperation , agpu_compare_function stencilFunction) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setStencilBackFace_immediate_renderer: immediate_renderer stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetStencilBackFace (agpu_immediate_renderer* immediate_renderer , agpu_stencil_operation stencilFailOperation , agpu_stencil_operation depthFailOperation , agpu_stencil_operation stencilDepthPassOperation , agpu_compare_function stencilFunction) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setSamplingMode_immediate_renderer: immediate_renderer filter: filter maxAnisotropy: maxAnisotropy addressU: addressU addressV: addressV addressW: addressW [
	^ self ffiCall: #(agpu_error agpuImmediateSetSamplingMode (agpu_immediate_renderer* immediate_renderer , agpu_filter filter , agpu_float maxAnisotropy , agpu_texture_address_mode addressU , agpu_texture_address_mode addressV , agpu_texture_address_mode addressW) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setViewport_immediate_renderer: immediate_renderer x: x y: y w: w h: h [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetViewport (agpu_immediate_renderer* immediate_renderer , agpu_int x , agpu_int y , agpu_int w , agpu_int h) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setScissor_immediate_renderer: immediate_renderer x: x y: y w: w h: h [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetScissor (agpu_immediate_renderer* immediate_renderer , agpu_int x , agpu_int y , agpu_int w , agpu_int h) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setStencilReference_immediate_renderer: immediate_renderer reference: reference [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetStencilReference (agpu_immediate_renderer* immediate_renderer , agpu_uint reference) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> projectionMatrixMode_immediate_renderer: immediate_renderer [
	^ self ffiCall: #(agpu_error agpuImmediateRendererProjectionMatrixMode (agpu_immediate_renderer* immediate_renderer) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> modelViewMatrixMode_immediate_renderer: immediate_renderer [
	^ self ffiCall: #(agpu_error agpuImmediateRendererModelViewMatrixMode (agpu_immediate_renderer* immediate_renderer) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> textureMatrixMode_immediate_renderer: immediate_renderer [
	^ self ffiCall: #(agpu_error agpuImmediateRendererTextureMatrixMode (agpu_immediate_renderer* immediate_renderer) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> loadIdentity_immediate_renderer: immediate_renderer [
	^ self ffiCall: #(agpu_error agpuImmediateRendererIdentity (agpu_immediate_renderer* immediate_renderer) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> pushMatrix_immediate_renderer: immediate_renderer [
	^ self ffiCall: #(agpu_error agpuImmediateRendererPushMatrix (agpu_immediate_renderer* immediate_renderer) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> popMatrix_immediate_renderer: immediate_renderer [
	^ self ffiCall: #(agpu_error agpuImmediateRendererPopMatrix (agpu_immediate_renderer* immediate_renderer) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> loadMatrix_immediate_renderer: immediate_renderer elements: elements [
	^ self ffiCall: #(agpu_error agpuImmediateRendererLoadMatrix (agpu_immediate_renderer* immediate_renderer , agpu_float* elements) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> loadTransposeMatrix_immediate_renderer: immediate_renderer elements: elements [
	^ self ffiCall: #(agpu_error agpuImmediateRendererLoadTransposeMatrix (agpu_immediate_renderer* immediate_renderer , agpu_float* elements) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> multiplyMatrix_immediate_renderer: immediate_renderer elements: elements [
	^ self ffiCall: #(agpu_error agpuImmediateRendererMultiplyMatrix (agpu_immediate_renderer* immediate_renderer , agpu_float* elements) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> multiplyTransposeMatrix_immediate_renderer: immediate_renderer elements: elements [
	^ self ffiCall: #(agpu_error agpuImmediateRendererMultiplyTransposeMatrix (agpu_immediate_renderer* immediate_renderer , agpu_float* elements) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> ortho_immediate_renderer: immediate_renderer left: left right: right bottom: bottom top: top near: near far: far [
	^ self ffiCall: #(agpu_error agpuImmediateRendererOrtho (agpu_immediate_renderer* immediate_renderer , agpu_float left , agpu_float right , agpu_float bottom , agpu_float top , agpu_float near , agpu_float far) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> frustum_immediate_renderer: immediate_renderer left: left right: right bottom: bottom top: top near: near far: far [
	^ self ffiCall: #(agpu_error agpuImmediateRendererFrustum (agpu_immediate_renderer* immediate_renderer , agpu_float left , agpu_float right , agpu_float bottom , agpu_float top , agpu_float near , agpu_float far) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> perspective_immediate_renderer: immediate_renderer fovy: fovy aspect: aspect near: near far: far [
	^ self ffiCall: #(agpu_error agpuImmediateRendererPerspective (agpu_immediate_renderer* immediate_renderer , agpu_float fovy , agpu_float aspect , agpu_float near , agpu_float far) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> rotate_immediate_renderer: immediate_renderer angle: angle x: x y: y z: z [
	^ self ffiCall: #(agpu_error agpuImmediateRendererRotate (agpu_immediate_renderer* immediate_renderer , agpu_float angle , agpu_float x , agpu_float y , agpu_float z) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> translate_immediate_renderer: immediate_renderer x: x y: y z: z [
	^ self ffiCall: #(agpu_error agpuImmediateRendererTranslate (agpu_immediate_renderer* immediate_renderer , agpu_float x , agpu_float y , agpu_float z) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> scale_immediate_renderer: immediate_renderer x: x y: y z: z [
	^ self ffiCall: #(agpu_error agpuImmediateRendererScale (agpu_immediate_renderer* immediate_renderer , agpu_float x , agpu_float y , agpu_float z) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setFlatShading_immediate_renderer: immediate_renderer enabled: enabled [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetFlatShading (agpu_immediate_renderer* immediate_renderer , agpu_bool enabled) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setLightingEnabled_immediate_renderer: immediate_renderer enabled: enabled [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetLightingEnabled (agpu_immediate_renderer* immediate_renderer , agpu_bool enabled) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setLightingModel_immediate_renderer: immediate_renderer model: model [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetLightingModel (agpu_immediate_renderer* immediate_renderer , agpu_immediate_renderer_lighting_model model) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> clearLights_immediate_renderer: immediate_renderer [
	^ self ffiCall: #(agpu_error agpuImmediateRendererClearLights (agpu_immediate_renderer* immediate_renderer) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setAmbientLighting_immediate_renderer: immediate_renderer r: r g: g b: b a: a [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetAmbientLighting (agpu_immediate_renderer* immediate_renderer , agpu_float r , agpu_float g , agpu_float b , agpu_float a) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setLight_immediate_renderer: immediate_renderer index: index enabled: enabled state: state [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetLight (agpu_immediate_renderer* immediate_renderer , agpu_uint index , agpu_bool enabled , agpu_immediate_renderer_light* state) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setMaterial_immediate_renderer: immediate_renderer state: state [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetMaterial (agpu_immediate_renderer* immediate_renderer , agpu_immediate_renderer_material* state) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setSkinningEnabled_immediate_renderer: immediate_renderer enabled: enabled [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetSkinningEnabled (agpu_immediate_renderer* immediate_renderer , agpu_bool enabled) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setSkinBones_immediate_renderer: immediate_renderer count: count matrices: matrices transpose: transpose [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetSkinBones (agpu_immediate_renderer* immediate_renderer , agpu_uint count , agpu_float* matrices , agpu_bool transpose) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setTexturingEnabled_immediate_renderer: immediate_renderer enabled: enabled [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetTextureEnabled (agpu_immediate_renderer* immediate_renderer , agpu_bool enabled) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setTangentSpaceEnabled_immediate_renderer: immediate_renderer enabled: enabled [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetTangentSpaceEnabled (agpu_immediate_renderer* immediate_renderer , agpu_bool enabled) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> bindTexture_immediate_renderer: immediate_renderer texture: texture [
	^ self ffiCall: #(agpu_error agpuImmediateRendererBindTexture (agpu_immediate_renderer* immediate_renderer , agpu_texture* texture) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> bindTextureIn_immediate_renderer: immediate_renderer texture: texture binding: binding [
	^ self ffiCall: #(agpu_error agpuImmediateRendererBindTextureIn (agpu_immediate_renderer* immediate_renderer , agpu_texture* texture , agpu_immediate_renderer_texture_binding binding) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setClipPlane_immediate_renderer: immediate_renderer index: index enabled: enabled p1: p1 p2: p2 p3: p3 p4: p4 [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetClipPlane (agpu_immediate_renderer* immediate_renderer , agpu_uint index , agpu_bool enabled , agpu_float p1 , agpu_float p2 , agpu_float p3 , agpu_float p4) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setFogMode_immediate_renderer: immediate_renderer mode: mode [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetFogMode (agpu_immediate_renderer* immediate_renderer , agpu_immediate_renderer_fog_mode mode) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setFogColor_immediate_renderer: immediate_renderer r: r g: g b: b a: a [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetFogColor (agpu_immediate_renderer* immediate_renderer , agpu_float r , agpu_float g , agpu_float b , agpu_float a) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setFogDistances_immediate_renderer: immediate_renderer start: start end: end [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetFogDistances (agpu_immediate_renderer* immediate_renderer , agpu_float start , agpu_float end) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setFogDensity_immediate_renderer: immediate_renderer density: density [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetFogDensity (agpu_immediate_renderer* immediate_renderer , agpu_float density) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> beginPrimitives_immediate_renderer: immediate_renderer type: type [
	^ self ffiCall: #(agpu_error agpuBeginImmediateRendererPrimitives (agpu_immediate_renderer* immediate_renderer , agpu_primitive_topology type) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> endPrimitives_immediate_renderer: immediate_renderer [
	^ self ffiCall: #(agpu_error agpuEndImmediateRendererPrimitives (agpu_immediate_renderer* immediate_renderer) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> color_immediate_renderer: immediate_renderer r: r g: g b: b a: a [
	^ self ffiCall: #(agpu_error agpuSetImmediateRendererColor (agpu_immediate_renderer* immediate_renderer , agpu_float r , agpu_float g , agpu_float b , agpu_float a) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> texcoord_immediate_renderer: immediate_renderer x: x y: y [
	^ self ffiCall: #(agpu_error agpuSetImmediateRendererTexcoord (agpu_immediate_renderer* immediate_renderer , agpu_float x , agpu_float y) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> normal_immediate_renderer: immediate_renderer x: x y: y z: z [
	^ self ffiCall: #(agpu_error agpuSetImmediateRendererNormal (agpu_immediate_renderer* immediate_renderer , agpu_float x , agpu_float y , agpu_float z) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> vertex_immediate_renderer: immediate_renderer x: x y: y z: z [
	^ self ffiCall: #(agpu_error agpuAddImmediateRendererVertex (agpu_immediate_renderer* immediate_renderer , agpu_float x , agpu_float y , agpu_float z) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> beginMeshWithVertices_immediate_renderer: immediate_renderer vertexCount: vertexCount stride: stride elementCount: elementCount vertices: vertices [
	^ self ffiCall: #(agpu_error agpuBeginImmediateRendererMeshWithVertices (agpu_immediate_renderer* immediate_renderer , agpu_size vertexCount , agpu_size stride , agpu_size elementCount , agpu_pointer vertices) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> beginMeshWithVertexBinding_immediate_renderer: immediate_renderer layout: layout vertices: vertices [
	^ self ffiCall: #(agpu_error agpuBeginImmediateRendererMeshWithVertexBinding (agpu_immediate_renderer* immediate_renderer , agpu_vertex_layout* layout , agpu_vertex_binding* vertices) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> useIndexBuffer_immediate_renderer: immediate_renderer index_buffer: index_buffer [
	^ self ffiCall: #(agpu_error agpuImmediateRendererUseIndexBuffer (agpu_immediate_renderer* immediate_renderer , agpu_buffer* index_buffer) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> useIndexBufferAt_immediate_renderer: immediate_renderer index_buffer: index_buffer offset: offset index_size: index_size [
	^ self ffiCall: #(agpu_error agpuImmediateRendererUseIndexBufferAt (agpu_immediate_renderer* immediate_renderer , agpu_buffer* index_buffer , agpu_size offset , agpu_size index_size) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setCurrentMeshColors_immediate_renderer: immediate_renderer stride: stride elementCount: elementCount colors: colors [
	^ self ffiCall: #(agpu_error agpuSetImmediateRendererCurrentMeshColors (agpu_immediate_renderer* immediate_renderer , agpu_size stride , agpu_size elementCount , agpu_pointer colors) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setCurrentMeshNormals_immediate_renderer: immediate_renderer stride: stride elementCount: elementCount normals: normals [
	^ self ffiCall: #(agpu_error agpuSetImmediateRendererCurrentMeshNormals (agpu_immediate_renderer* immediate_renderer , agpu_size stride , agpu_size elementCount , agpu_pointer normals) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setCurrentMeshTexCoords_immediate_renderer: immediate_renderer stride: stride elementCount: elementCount texcoords: texcoords [
	^ self ffiCall: #(agpu_error agpuSetImmediateRendererCurrentMeshTexCoords (agpu_immediate_renderer* immediate_renderer , agpu_size stride , agpu_size elementCount , agpu_pointer texcoords) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setPrimitiveType_immediate_renderer: immediate_renderer type: type [
	^ self ffiCall: #(agpu_error agpuImmediateRendererSetPrimitiveType (agpu_immediate_renderer* immediate_renderer , agpu_primitive_topology type) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> drawArrays_immediate_renderer: immediate_renderer vertex_count: vertex_count instance_count: instance_count first_vertex: first_vertex base_instance: base_instance [
	^ self ffiCall: #(agpu_error agpuImmediateRendererDrawArrays (agpu_immediate_renderer* immediate_renderer , agpu_uint vertex_count , agpu_uint instance_count , agpu_uint first_vertex , agpu_uint base_instance) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> drawElements_immediate_renderer: immediate_renderer index_count: index_count instance_count: instance_count first_index: first_index base_vertex: base_vertex base_instance: base_instance [
	^ self ffiCall: #(agpu_error agpuImmediateRendererDrawElements (agpu_immediate_renderer* immediate_renderer , agpu_uint index_count , agpu_uint instance_count , agpu_uint first_index , agpu_int base_vertex , agpu_uint base_instance) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> drawElementsWithIndices_immediate_renderer: immediate_renderer mode: mode indices: indices index_count: index_count instance_count: instance_count first_index: first_index base_vertex: base_vertex base_instance: base_instance [
	^ self ffiCall: #(agpu_error agpuImmediateRendererDrawElementsWithIndices (agpu_immediate_renderer* immediate_renderer , agpu_primitive_topology mode , agpu_pointer indices , agpu_uint index_count , agpu_uint instance_count , agpu_uint first_index , agpu_int base_vertex , agpu_uint base_instance) )
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> endMesh_immediate_renderer: immediate_renderer [
	^ self ffiCall: #(agpu_error agpuEndImmediateRendererMesh (agpu_immediate_renderer* immediate_renderer) )
]

{ #category : #'global c functions' }
AGPUCBindings >> getPlatforms_numplatforms: numplatforms platforms: platforms ret_numplatforms: ret_numplatforms [
	^ self ffiCall: #(agpu_error agpuGetPlatforms (agpu_size numplatforms , agpu_platform* platforms , agpu_size* ret_numplatforms) )
]

