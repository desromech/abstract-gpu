Class {
	#name : #AGPUCBindings,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #AGPUCBindingsBase,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'platform' }
AGPUCBindings >> openDevice_platform: platform openInfo: openInfo [
	<cdecl: void* 'agpuOpenDevice' (void* AGPUDeviceOpenInfo*)>
	^ self externalCallFailed
]

{ #category : #'platform' }
AGPUCBindings >> getName_platform: platform [
	<cdecl: char* 'agpuGetPlatformName' (void*)>
	^ self externalCallFailed
]

{ #category : #'platform' }
AGPUCBindings >> getGpuCount_platform: platform [
	<cdecl: ulong 'agpuGetPlatformGpuCount' (void*)>
	^ self externalCallFailed
]

{ #category : #'platform' }
AGPUCBindings >> getGpuName_platform: platform gpu_index: gpu_index [
	<cdecl: char* 'agpuGetPlatformGpuName' (void* ulong)>
	^ self externalCallFailed
]

{ #category : #'platform' }
AGPUCBindings >> getVersion_platform: platform [
	<cdecl: long 'agpuGetPlatformVersion' (void*)>
	^ self externalCallFailed
]

{ #category : #'platform' }
AGPUCBindings >> getImplementationVersion_platform: platform [
	<cdecl: long 'agpuGetPlatformImplementationVersion' (void*)>
	^ self externalCallFailed
]

{ #category : #'platform' }
AGPUCBindings >> hasRealMultithreading_platform: platform [
	<cdecl: long 'agpuPlatformHasRealMultithreading' (void*)>
	^ self externalCallFailed
]

{ #category : #'platform' }
AGPUCBindings >> isNative_platform: platform [
	<cdecl: long 'agpuIsNativePlatform' (void*)>
	^ self externalCallFailed
]

{ #category : #'platform' }
AGPUCBindings >> isCrossPlatform_platform: platform [
	<cdecl: long 'agpuIsCrossPlatform' (void*)>
	^ self externalCallFailed
]

{ #category : #'platform' }
AGPUCBindings >> createOfflineShaderCompiler_platform: platform [
	<cdecl: void* 'agpuCreateOfflineShaderCompiler' (void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> addReference_device: device [
	<cdecl: long 'agpuAddDeviceReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> release_device: device [
	<cdecl: long 'agpuReleaseDevice' (void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> getDefaultCommandQueue_device: device [
	<cdecl: void* 'agpuGetDefaultCommandQueue' (void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> createSwapChain_device: device commandQueue: commandQueue swapChainInfo: swapChainInfo [
	<cdecl: void* 'agpuCreateSwapChain' (void* void* AGPUSwapChainCreateInfo*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> createBuffer_device: device description: description initial_data: initial_data [
	<cdecl: void* 'agpuCreateBuffer' (void* AGPUBufferDescription* void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> createVertexLayout_device: device [
	<cdecl: void* 'agpuCreateVertexLayout' (void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> createVertexBinding_device: device layout: layout [
	<cdecl: void* 'agpuCreateVertexBinding' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> createShader_device: device type: type [
	<cdecl: void* 'agpuCreateShader' (void* long)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> createShaderSignatureBuilder_device: device [
	<cdecl: void* 'agpuCreateShaderSignatureBuilder' (void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> createPipelineBuilder_device: device [
	<cdecl: void* 'agpuCreatePipelineBuilder' (void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> createComputePipelineBuilder_device: device [
	<cdecl: void* 'agpuCreateComputePipelineBuilder' (void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> createCommandAllocator_device: device type: type queue: queue [
	<cdecl: void* 'agpuCreateCommandAllocator' (void* long void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> createCommandList_device: device type: type allocator: allocator initial_pipeline_state: initial_pipeline_state [
	<cdecl: void* 'agpuCreateCommandList' (void* long void* void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> getPreferredShaderLanguage_device: device [
	<cdecl: long 'agpuGetPreferredShaderLanguage' (void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> getPreferredIntermediateShaderLanguage_device: device [
	<cdecl: long 'agpuGetPreferredIntermediateShaderLanguage' (void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> getPreferredHighLevelShaderLanguage_device: device [
	<cdecl: long 'agpuGetPreferredHighLevelShaderLanguage' (void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> createFrameBuffer_device: device width: width height: height colorCount: colorCount colorViews: colorViews depthStencilView: depthStencilView [
	<cdecl: void* 'agpuCreateFrameBuffer' (void* ulong ulong ulong void* void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> createRenderPass_device: device description: description [
	<cdecl: void* 'agpuCreateRenderPass' (void* AGPURenderpassDescription*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> createTexture_device: device description: description [
	<cdecl: void* 'agpuCreateTexture' (void* AGPUTextureDescription*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> createSampler_device: device description: description [
	<cdecl: void* 'agpuCreateSampler' (void* AGPUSamplerDescription*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> createFence_device: device [
	<cdecl: void* 'agpuCreateFence' (void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> getMultiSampleQualityLevels_device: device format: format sample_count: sample_count [
	<cdecl: long 'agpuGetMultiSampleQualityLevels' (void* long ulong)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> hasTopLeftNdcOrigin_device: device [
	<cdecl: long 'agpuHasTopLeftNdcOrigin' (void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> hasBottomLeftTextureCoordinates_device: device [
	<cdecl: long 'agpuHasBottomLeftTextureCoordinates' (void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> isFeatureSupported_device: device feature: feature [
	<cdecl: long 'agpuIsFeatureSupportedOnDevice' (void* long)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> getLimitValue_device: device limit: limit [
	<cdecl: long 'agpuGetLimitValue' (void* long)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> getVRSystem_device: device [
	<cdecl: void* 'agpuGetVRSystem' (void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> createOfflineShaderCompiler_device: device [
	<cdecl: void* 'agpuCreateOfflineShaderCompilerForDevice' (void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> createStateTrackerCache_device: device command_queue_family: command_queue_family [
	<cdecl: void* 'agpuCreateStateTrackerCache' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'device' }
AGPUCBindings >> finishExecution_device: device [
	<cdecl: long 'agpuFinishDeviceExecution' (void*)>
	^ self externalCallFailed
]

{ #category : #'vr_system' }
AGPUCBindings >> addReference_vr_system: vr_system [
	<cdecl: long 'agpuAddVRSystemReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'vr_system' }
AGPUCBindings >> release_vr_system: vr_system [
	<cdecl: long 'agpuReleaseVRSystem' (void*)>
	^ self externalCallFailed
]

{ #category : #'vr_system' }
AGPUCBindings >> getVRSystemName_vr_system: vr_system [
	<cdecl: char* 'agpuGetVRSystemName' (void*)>
	^ self externalCallFailed
]

{ #category : #'vr_system' }
AGPUCBindings >> getNativeHandle_vr_system: vr_system [
	<cdecl: void* 'agpuGetVRSystemNativeHandle' (void*)>
	^ self externalCallFailed
]

{ #category : #'vr_system' }
AGPUCBindings >> getRecommendedRenderTargetSize_vr_system: vr_system size: size [
	<cdecl: long 'agpuGetVRRecommendedRenderTargetSize' (void* AGPUSize2d*)>
	^ self externalCallFailed
]

{ #category : #'vr_system' }
AGPUCBindings >> getEyeToHeadTransform_vr_system: vr_system eye: eye transform: transform [
	<cdecl: long 'agpuGetVREyeToHeadTransformInto' (void* long AGPUMatrix4x4f*)>
	^ self externalCallFailed
]

{ #category : #'vr_system' }
AGPUCBindings >> getProjectionMatrix_vr_system: vr_system eye: eye near_distance: near_distance far_distance: far_distance projection_matrix: projection_matrix [
	<cdecl: long 'agpuGetVRProjectionMatrix' (void* long float float AGPUMatrix4x4f*)>
	^ self externalCallFailed
]

{ #category : #'vr_system' }
AGPUCBindings >> getProjectionFrustumTangents_vr_system: vr_system eye: eye frustum: frustum [
	<cdecl: long 'agpuGetVRProjectionFrustumTangents' (void* long AGPUFrustumTangents*)>
	^ self externalCallFailed
]

{ #category : #'vr_system' }
AGPUCBindings >> submitEyeRenderTargets_vr_system: vr_system left_eye: left_eye right_eye: right_eye [
	<cdecl: long 'agpuSubmitVREyeRenderTargets' (void* void* void*)>
	^ self externalCallFailed
]

{ #category : #'vr_system' }
AGPUCBindings >> waitAndFetchPoses_vr_system: vr_system [
	<cdecl: long 'agpuWaitAndFetchVRPoses' (void*)>
	^ self externalCallFailed
]

{ #category : #'vr_system' }
AGPUCBindings >> getMaxTrackedDevicePoseCount_vr_system: vr_system [
	<cdecl: ulong 'agpuGetMaxVRTrackedDevicePoseCount' (void*)>
	^ self externalCallFailed
]

{ #category : #'vr_system' }
AGPUCBindings >> getCurrentTrackedDevicePoseCount_vr_system: vr_system [
	<cdecl: ulong 'agpuGetCurrentVRTrackedDevicePoseCount' (void*)>
	^ self externalCallFailed
]

{ #category : #'vr_system' }
AGPUCBindings >> getCurrentTrackedDevicePoseInto_vr_system: vr_system index: index dest: dest [
	<cdecl: long 'agpuGetCurrentVRTrackedDevicePoseInto' (void* ulong AGPUVrTrackedDevicePose*)>
	^ self externalCallFailed
]

{ #category : #'vr_system' }
AGPUCBindings >> getMaxRenderTrackedDevicePoseCount_vr_system: vr_system [
	<cdecl: ulong 'agpuGetMaxVRRenderTrackedDevicePoseCount' (void*)>
	^ self externalCallFailed
]

{ #category : #'vr_system' }
AGPUCBindings >> getCurrentRenderTrackedDevicePoseCount_vr_system: vr_system [
	<cdecl: ulong 'agpuGetCurrentVRRenderTrackedDevicePoseCount' (void*)>
	^ self externalCallFailed
]

{ #category : #'vr_system' }
AGPUCBindings >> getCurrentRenderTrackedDevicePoseInto_vr_system: vr_system index: index dest: dest [
	<cdecl: long 'agpuGetCurrentVRRenderTrackedDevicePoseInto' (void* ulong AGPUVrTrackedDevicePose*)>
	^ self externalCallFailed
]

{ #category : #'vr_system' }
AGPUCBindings >> pollEvent_vr_system: vr_system event: event [
	<cdecl: long 'agpuPollVREvent' (void* AGPUVrEvent*)>
	^ self externalCallFailed
]

{ #category : #'swap_chain' }
AGPUCBindings >> addReference_swap_chain: swap_chain [
	<cdecl: long 'agpuAddSwapChainReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'swap_chain' }
AGPUCBindings >> release_swap_chain: swap_chain [
	<cdecl: long 'agpuReleaseSwapChain' (void*)>
	^ self externalCallFailed
]

{ #category : #'swap_chain' }
AGPUCBindings >> swapBuffers_swap_chain: swap_chain [
	<cdecl: long 'agpuSwapBuffers' (void*)>
	^ self externalCallFailed
]

{ #category : #'swap_chain' }
AGPUCBindings >> getCurrentBackBuffer_swap_chain: swap_chain [
	<cdecl: void* 'agpuGetCurrentBackBuffer' (void*)>
	^ self externalCallFailed
]

{ #category : #'swap_chain' }
AGPUCBindings >> getCurrentBackBufferForLayer_swap_chain: swap_chain layer: layer [
	<cdecl: void* 'agpuGetCurrentBackBufferForLayer' (void* ulong)>
	^ self externalCallFailed
]

{ #category : #'swap_chain' }
AGPUCBindings >> getCurrentBackBufferIndex_swap_chain: swap_chain [
	<cdecl: ulong 'agpuGetCurrentBackBufferIndex' (void*)>
	^ self externalCallFailed
]

{ #category : #'swap_chain' }
AGPUCBindings >> getFramebufferCount_swap_chain: swap_chain [
	<cdecl: ulong 'agpuGetFramebufferCount' (void*)>
	^ self externalCallFailed
]

{ #category : #'swap_chain' }
AGPUCBindings >> getWidth_swap_chain: swap_chain [
	<cdecl: ulong 'agpuGetSwapChainWidth' (void*)>
	^ self externalCallFailed
]

{ #category : #'swap_chain' }
AGPUCBindings >> getHeight_swap_chain: swap_chain [
	<cdecl: ulong 'agpuGetSwapChainHeight' (void*)>
	^ self externalCallFailed
]

{ #category : #'swap_chain' }
AGPUCBindings >> getLayerCount_swap_chain: swap_chain [
	<cdecl: ulong 'agpuGetSwapChainLayerCount' (void*)>
	^ self externalCallFailed
]

{ #category : #'swap_chain' }
AGPUCBindings >> setOverlayPosition_swap_chain: swap_chain x: x y: y [
	<cdecl: long 'agpuSetSwapChainOverlayPosition' (void* long long)>
	^ self externalCallFailed
]

{ #category : #'compute_pipeline_builder' }
AGPUCBindings >> addReference_compute_pipeline_builder: compute_pipeline_builder [
	<cdecl: long 'agpuAddComputePipelineBuilderReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'compute_pipeline_builder' }
AGPUCBindings >> release_compute_pipeline_builder: compute_pipeline_builder [
	<cdecl: long 'agpuReleaseComputePipelineBuilder' (void*)>
	^ self externalCallFailed
]

{ #category : #'compute_pipeline_builder' }
AGPUCBindings >> build_compute_pipeline_builder: compute_pipeline_builder [
	<cdecl: void* 'agpuBuildComputePipelineState' (void*)>
	^ self externalCallFailed
]

{ #category : #'compute_pipeline_builder' }
AGPUCBindings >> attachShader_compute_pipeline_builder: compute_pipeline_builder shader: shader [
	<cdecl: long 'agpuAttachComputeShader' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'compute_pipeline_builder' }
AGPUCBindings >> attachShaderWithEntryPoint_compute_pipeline_builder: compute_pipeline_builder shader: shader type: type entry_point: entry_point [
	<cdecl: long 'agpuAttachComputeShaderWithEntryPoint' (void* void* long byte*)>
	^ self externalCallFailed
]

{ #category : #'compute_pipeline_builder' }
AGPUCBindings >> getBuildingLogLength_compute_pipeline_builder: compute_pipeline_builder [
	<cdecl: ulong 'agpuGetComputePipelineBuildingLogLength' (void*)>
	^ self externalCallFailed
]

{ #category : #'compute_pipeline_builder' }
AGPUCBindings >> getBuildingLog_compute_pipeline_builder: compute_pipeline_builder buffer_size: buffer_size buffer: buffer [
	<cdecl: long 'agpuGetComputePipelineBuildingLog' (void* ulong byte*)>
	^ self externalCallFailed
]

{ #category : #'compute_pipeline_builder' }
AGPUCBindings >> setShaderSignature_compute_pipeline_builder: compute_pipeline_builder signature: signature [
	<cdecl: long 'agpuSetComputePipelineShaderSignature' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> addReference_pipeline_builder: pipeline_builder [
	<cdecl: long 'agpuAddPipelineBuilderReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> release_pipeline_builder: pipeline_builder [
	<cdecl: long 'agpuReleasePipelineBuilder' (void*)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> build_pipeline_builder: pipeline_builder [
	<cdecl: void* 'agpuBuildPipelineState' (void*)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> attachShader_pipeline_builder: pipeline_builder shader: shader [
	<cdecl: long 'agpuAttachShader' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> attachShaderWithEntryPoint_pipeline_builder: pipeline_builder shader: shader type: type entry_point: entry_point [
	<cdecl: long 'agpuAttachShaderWithEntryPoint' (void* void* long byte*)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> getBuildingLogLength_pipeline_builder: pipeline_builder [
	<cdecl: ulong 'agpuGetPipelineBuildingLogLength' (void*)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> getBuildingLog_pipeline_builder: pipeline_builder buffer_size: buffer_size buffer: buffer [
	<cdecl: long 'agpuGetPipelineBuildingLog' (void* ulong byte*)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setBlendState_pipeline_builder: pipeline_builder renderTargetMask: renderTargetMask enabled: enabled [
	<cdecl: long 'agpuSetBlendState' (void* long long)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setBlendFunction_pipeline_builder: pipeline_builder renderTargetMask: renderTargetMask sourceFactor: sourceFactor destFactor: destFactor colorOperation: colorOperation sourceAlphaFactor: sourceAlphaFactor destAlphaFactor: destAlphaFactor alphaOperation: alphaOperation [
	<cdecl: long 'agpuSetBlendFunction' (void* long long long long long long long)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setColorMask_pipeline_builder: pipeline_builder renderTargetMask: renderTargetMask redEnabled: redEnabled greenEnabled: greenEnabled blueEnabled: blueEnabled alphaEnabled: alphaEnabled [
	<cdecl: long 'agpuSetColorMask' (void* long long long long long)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setFrontFace_pipeline_builder: pipeline_builder winding: winding [
	<cdecl: long 'agpuSetFrontFace' (void* long)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setCullMode_pipeline_builder: pipeline_builder mode: mode [
	<cdecl: long 'agpuSetCullMode' (void* long)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setDepthBias_pipeline_builder: pipeline_builder constant_factor: constant_factor clamp: clamp slope_factor: slope_factor [
	<cdecl: long 'agpuSetDepthBias' (void* float float float)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setDepthState_pipeline_builder: pipeline_builder enabled: enabled writeMask: writeMask function: function [
	<cdecl: long 'agpuSetDepthState' (void* long long long)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setPolygonMode_pipeline_builder: pipeline_builder mode: mode [
	<cdecl: long 'agpuSetPolygonMode' (void* long)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setStencilState_pipeline_builder: pipeline_builder enabled: enabled writeMask: writeMask readMask: readMask [
	<cdecl: long 'agpuSetStencilState' (void* long long long)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setStencilFrontFace_pipeline_builder: pipeline_builder stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	<cdecl: long 'agpuSetStencilFrontFace' (void* long long long long)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setStencilBackFace_pipeline_builder: pipeline_builder stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	<cdecl: long 'agpuSetStencilBackFace' (void* long long long long)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setRenderTargetCount_pipeline_builder: pipeline_builder count: count [
	<cdecl: long 'agpuSetRenderTargetCount' (void* long)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setRenderTargetFormat_pipeline_builder: pipeline_builder index: index format: format [
	<cdecl: long 'agpuSetRenderTargetFormat' (void* ulong long)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setDepthStencilFormat_pipeline_builder: pipeline_builder format: format [
	<cdecl: long 'agpuSetDepthStencilFormat' (void* long)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setPrimitiveType_pipeline_builder: pipeline_builder type: type [
	<cdecl: long 'agpuSetPrimitiveType' (void* long)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setVertexLayout_pipeline_builder: pipeline_builder layout: layout [
	<cdecl: long 'agpuSetVertexLayout' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setShaderSignature_pipeline_builder: pipeline_builder signature: signature [
	<cdecl: long 'agpuSetPipelineShaderSignature' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'pipeline_builder' }
AGPUCBindings >> setSampleDescription_pipeline_builder: pipeline_builder sample_count: sample_count sample_quality: sample_quality [
	<cdecl: long 'agpuSetSampleDescription' (void* ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'pipeline_state' }
AGPUCBindings >> addReference_pipeline_state: pipeline_state [
	<cdecl: long 'agpuAddPipelineStateReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'pipeline_state' }
AGPUCBindings >> release_pipeline_state: pipeline_state [
	<cdecl: long 'agpuReleasePipelineState' (void*)>
	^ self externalCallFailed
]

{ #category : #'command_queue' }
AGPUCBindings >> addReference_command_queue: command_queue [
	<cdecl: long 'agpuAddCommandQueueReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'command_queue' }
AGPUCBindings >> release_command_queue: command_queue [
	<cdecl: long 'agpuReleaseCommandQueue' (void*)>
	^ self externalCallFailed
]

{ #category : #'command_queue' }
AGPUCBindings >> addCommandList_command_queue: command_queue command_list: command_list [
	<cdecl: long 'agpuAddCommandList' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'command_queue' }
AGPUCBindings >> finishExecution_command_queue: command_queue [
	<cdecl: long 'agpuFinishQueueExecution' (void*)>
	^ self externalCallFailed
]

{ #category : #'command_queue' }
AGPUCBindings >> signalFence_command_queue: command_queue fence: fence [
	<cdecl: long 'agpuSignalFence' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'command_queue' }
AGPUCBindings >> waitFence_command_queue: command_queue fence: fence [
	<cdecl: long 'agpuWaitFence' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'command_allocator' }
AGPUCBindings >> addReference_command_allocator: command_allocator [
	<cdecl: long 'agpuAddCommandAllocatorReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'command_allocator' }
AGPUCBindings >> release_command_allocator: command_allocator [
	<cdecl: long 'agpuReleaseCommandAllocator' (void*)>
	^ self externalCallFailed
]

{ #category : #'command_allocator' }
AGPUCBindings >> reset_command_allocator: command_allocator [
	<cdecl: long 'agpuResetCommandAllocator' (void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> addReference_command_list: command_list [
	<cdecl: long 'agpuAddCommandListReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> release_command_list: command_list [
	<cdecl: long 'agpuReleaseCommandList' (void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> setShaderSignature_command_list: command_list signature: signature [
	<cdecl: long 'agpuSetShaderSignature' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> setViewport_command_list: command_list x: x y: y w: w h: h [
	<cdecl: long 'agpuSetViewport' (void* long long long long)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> setScissor_command_list: command_list x: x y: y w: w h: h [
	<cdecl: long 'agpuSetScissor' (void* long long long long)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> usePipelineState_command_list: command_list pipeline: pipeline [
	<cdecl: long 'agpuUsePipelineState' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> useVertexBinding_command_list: command_list vertex_binding: vertex_binding [
	<cdecl: long 'agpuUseVertexBinding' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> useIndexBuffer_command_list: command_list index_buffer: index_buffer [
	<cdecl: long 'agpuUseIndexBuffer' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> useIndexBufferAt_command_list: command_list index_buffer: index_buffer offset: offset index_size: index_size [
	<cdecl: long 'agpuUseIndexBufferAt' (void* void* ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> useDrawIndirectBuffer_command_list: command_list draw_buffer: draw_buffer [
	<cdecl: long 'agpuUseDrawIndirectBuffer' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> useComputeDispatchIndirectBuffer_command_list: command_list buffer: buffer [
	<cdecl: long 'agpuUseComputeDispatchIndirectBuffer' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> useShaderResources_command_list: command_list binding: binding [
	<cdecl: long 'agpuUseShaderResources' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> useComputeShaderResources_command_list: command_list binding: binding [
	<cdecl: long 'agpuUseComputeShaderResources' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> drawArrays_command_list: command_list vertex_count: vertex_count instance_count: instance_count first_vertex: first_vertex base_instance: base_instance [
	<cdecl: long 'agpuDrawArrays' (void* ulong ulong ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> drawArraysIndirect_command_list: command_list offset: offset drawcount: drawcount [
	<cdecl: long 'agpuDrawArraysIndirect' (void* ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> drawElements_command_list: command_list index_count: index_count instance_count: instance_count first_index: first_index base_vertex: base_vertex base_instance: base_instance [
	<cdecl: long 'agpuDrawElements' (void* ulong ulong ulong long ulong)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> drawElementsIndirect_command_list: command_list offset: offset drawcount: drawcount [
	<cdecl: long 'agpuDrawElementsIndirect' (void* ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> dispatchCompute_command_list: command_list group_count_x: group_count_x group_count_y: group_count_y group_count_z: group_count_z [
	<cdecl: long 'agpuDispatchCompute' (void* ulong ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> dispatchComputeIndirect_command_list: command_list offset: offset [
	<cdecl: long 'agpuDispatchComputeIndirect' (void* ulong)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> setStencilReference_command_list: command_list reference: reference [
	<cdecl: long 'agpuSetStencilReference' (void* ulong)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> executeBundle_command_list: command_list bundle: bundle [
	<cdecl: long 'agpuExecuteBundle' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> close_command_list: command_list [
	<cdecl: long 'agpuCloseCommandList' (void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> reset_command_list: command_list allocator: allocator initial_pipeline_state: initial_pipeline_state [
	<cdecl: long 'agpuResetCommandList' (void* void* void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> resetBundle_command_list: command_list allocator: allocator initial_pipeline_state: initial_pipeline_state inheritance_info: inheritance_info [
	<cdecl: long 'agpuResetBundleCommandList' (void* void* void* AGPUInheritanceInfo*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> beginRenderPass_command_list: command_list renderpass: renderpass framebuffer: framebuffer bundle_content: bundle_content [
	<cdecl: long 'agpuBeginRenderPass' (void* void* void* long)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> endRenderPass_command_list: command_list [
	<cdecl: long 'agpuEndRenderPass' (void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> resolveFramebuffer_command_list: command_list destFramebuffer: destFramebuffer sourceFramebuffer: sourceFramebuffer [
	<cdecl: long 'agpuResolveFramebuffer' (void* void* void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> resolveTexture_command_list: command_list sourceTexture: sourceTexture sourceLevel: sourceLevel sourceLayer: sourceLayer destTexture: destTexture destLevel: destLevel destLayer: destLayer levelCount: levelCount layerCount: layerCount aspect: aspect [
	<cdecl: long 'agpuResolveTexture' (void* void* ulong ulong void* ulong ulong ulong ulong long)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> pushConstants_command_list: command_list offset: offset size: size values: values [
	<cdecl: long 'agpuPushConstants' (void* ulong ulong void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> memoryBarrier_command_list: command_list source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses [
	<cdecl: long 'agpuMemoryBarrier' (void* long long long long)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> bufferMemoryBarrier_command_list: command_list buffer: buffer source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses offset: offset size: size [
	<cdecl: long 'agpuBufferMemoryBarrier' (void* void* long long long long ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> textureMemoryBarrier_command_list: command_list texture: texture source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses subresource_range: subresource_range [
	<cdecl: long 'agpuTextureMemoryBarrier' (void* void* long long long long AGPUSubresourceRange*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> pushBufferTransitionBarrier_command_list: command_list buffer: buffer new_usage: new_usage [
	<cdecl: long 'agpuPushBufferTransitionBarrier' (void* void* long)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> pushTextureTransitionBarrier_command_list: command_list texture: texture new_usage: new_usage subresource_range: subresource_range [
	<cdecl: long 'agpuPushTextureTransitionBarrier' (void* void* long AGPUSubresourceRange*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> popBufferTransitionBarrier_command_list: command_list [
	<cdecl: long 'agpuPopBufferTransitionBarrier' (void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> popTextureTransitionBarrier_command_list: command_list [
	<cdecl: long 'agpuPopTextureTransitionBarrier' (void*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> copyBuffer_command_list: command_list source_buffer: source_buffer source_offset: source_offset dest_buffer: dest_buffer dest_offset: dest_offset copy_size: copy_size [
	<cdecl: long 'agpuCopyBuffer' (void* void* ulong void* ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> copyBufferToTexture_command_list: command_list buffer: buffer texture: texture copy_region: copy_region [
	<cdecl: long 'agpuCopyBufferToTexture' (void* void* void* AGPUBufferImageCopyRegion*)>
	^ self externalCallFailed
]

{ #category : #'command_list' }
AGPUCBindings >> copyTextureToBuffer_command_list: command_list texture: texture buffer: buffer copy_region: copy_region [
	<cdecl: long 'agpuCopyTextureToBuffer' (void* void* void* AGPUBufferImageCopyRegion*)>
	^ self externalCallFailed
]

{ #category : #'texture' }
AGPUCBindings >> addReference_texture: texture [
	<cdecl: long 'agpuAddTextureReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'texture' }
AGPUCBindings >> release_texture: texture [
	<cdecl: long 'agpuReleaseTexture' (void*)>
	^ self externalCallFailed
]

{ #category : #'texture' }
AGPUCBindings >> getDescription_texture: texture description: description [
	<cdecl: long 'agpuGetTextureDescription' (void* AGPUTextureDescription*)>
	^ self externalCallFailed
]

{ #category : #'texture' }
AGPUCBindings >> mapLevel_texture: texture level: level arrayIndex: arrayIndex flags: flags region: region [
	<cdecl: void* 'agpuMapTextureLevel' (void* long long long AGPURegion3d*)>
	^ self externalCallFailed
]

{ #category : #'texture' }
AGPUCBindings >> unmapLevel_texture: texture [
	<cdecl: long 'agpuUnmapTextureLevel' (void*)>
	^ self externalCallFailed
]

{ #category : #'texture' }
AGPUCBindings >> readTextureData_texture: texture level: level arrayIndex: arrayIndex pitch: pitch slicePitch: slicePitch buffer: buffer [
	<cdecl: long 'agpuReadTextureData' (void* long long long long void*)>
	^ self externalCallFailed
]

{ #category : #'texture' }
AGPUCBindings >> readTextureSubData_texture: texture level: level arrayIndex: arrayIndex pitch: pitch slicePitch: slicePitch sourceRegion: sourceRegion destSize: destSize buffer: buffer [
	<cdecl: long 'agpuReadTextureSubData' (void* long long long long AGPURegion3d* AGPUSize3d* void*)>
	^ self externalCallFailed
]

{ #category : #'texture' }
AGPUCBindings >> uploadTextureData_texture: texture level: level arrayIndex: arrayIndex pitch: pitch slicePitch: slicePitch data: data [
	<cdecl: long 'agpuUploadTextureData' (void* long long long long void*)>
	^ self externalCallFailed
]

{ #category : #'texture' }
AGPUCBindings >> uploadTextureSubData_texture: texture level: level arrayIndex: arrayIndex pitch: pitch slicePitch: slicePitch sourceSize: sourceSize destRegion: destRegion data: data [
	<cdecl: long 'agpuUploadTextureSubData' (void* long long long long AGPUSize3d* AGPURegion3d* void*)>
	^ self externalCallFailed
]

{ #category : #'texture' }
AGPUCBindings >> getFullViewDescription_texture: texture result: result [
	<cdecl: long 'agpuGetTextureFullViewDescription' (void* AGPUTextureViewDescription*)>
	^ self externalCallFailed
]

{ #category : #'texture' }
AGPUCBindings >> createView_texture: texture description: description [
	<cdecl: void* 'agpuCreateTextureView' (void* AGPUTextureViewDescription*)>
	^ self externalCallFailed
]

{ #category : #'texture' }
AGPUCBindings >> getOrCreateFullView_texture: texture [
	<cdecl: void* 'agpuGetOrCreateFullTextureView' (void*)>
	^ self externalCallFailed
]

{ #category : #'texture_view' }
AGPUCBindings >> addReference_texture_view: texture_view [
	<cdecl: long 'agpuAddTextureViewReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'texture_view' }
AGPUCBindings >> release_texture_view: texture_view [
	<cdecl: long 'agpuReleaseTextureView' (void*)>
	^ self externalCallFailed
]

{ #category : #'texture_view' }
AGPUCBindings >> getTexture_texture_view: texture_view [
	<cdecl: void* 'agpuGetTextureFromView' (void*)>
	^ self externalCallFailed
]

{ #category : #'sampler' }
AGPUCBindings >> addReference_sampler: sampler [
	<cdecl: long 'agpuAddSamplerReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'sampler' }
AGPUCBindings >> release_sampler: sampler [
	<cdecl: long 'agpuReleaseSampler' (void*)>
	^ self externalCallFailed
]

{ #category : #'buffer' }
AGPUCBindings >> addReference_buffer: buffer [
	<cdecl: long 'agpuAddBufferReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'buffer' }
AGPUCBindings >> release_buffer: buffer [
	<cdecl: long 'agpuReleaseBuffer' (void*)>
	^ self externalCallFailed
]

{ #category : #'buffer' }
AGPUCBindings >> mapBuffer_buffer: buffer flags: flags [
	<cdecl: void* 'agpuMapBuffer' (void* long)>
	^ self externalCallFailed
]

{ #category : #'buffer' }
AGPUCBindings >> unmapBuffer_buffer: buffer [
	<cdecl: long 'agpuUnmapBuffer' (void*)>
	^ self externalCallFailed
]

{ #category : #'buffer' }
AGPUCBindings >> getDescription_buffer: buffer description: description [
	<cdecl: long 'agpuGetBufferDescription' (void* AGPUBufferDescription*)>
	^ self externalCallFailed
]

{ #category : #'buffer' }
AGPUCBindings >> uploadBufferData_buffer: buffer offset: offset size: size data: data [
	<cdecl: long 'agpuUploadBufferData' (void* ulong ulong void*)>
	^ self externalCallFailed
]

{ #category : #'buffer' }
AGPUCBindings >> readBufferData_buffer: buffer offset: offset size: size data: data [
	<cdecl: long 'agpuReadBufferData' (void* ulong ulong void*)>
	^ self externalCallFailed
]

{ #category : #'buffer' }
AGPUCBindings >> flushWholeBuffer_buffer: buffer [
	<cdecl: long 'agpuFlushWholeBuffer' (void*)>
	^ self externalCallFailed
]

{ #category : #'buffer' }
AGPUCBindings >> invalidateWholeBuffer_buffer: buffer [
	<cdecl: long 'agpuInvalidateWholeBuffer' (void*)>
	^ self externalCallFailed
]

{ #category : #'vertex_binding' }
AGPUCBindings >> addReference_vertex_binding: vertex_binding [
	<cdecl: long 'agpuAddVertexBindingReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'vertex_binding' }
AGPUCBindings >> release_vertex_binding: vertex_binding [
	<cdecl: long 'agpuReleaseVertexBinding' (void*)>
	^ self externalCallFailed
]

{ #category : #'vertex_binding' }
AGPUCBindings >> bindVertexBuffers_vertex_binding: vertex_binding count: count vertex_buffers: vertex_buffers [
	<cdecl: long 'agpuBindVertexBuffers' (void* ulong void*)>
	^ self externalCallFailed
]

{ #category : #'vertex_binding' }
AGPUCBindings >> bindVertexBuffersWithOffsets_vertex_binding: vertex_binding count: count vertex_buffers: vertex_buffers offsets: offsets [
	<cdecl: long 'agpuBindVertexBuffersWithOffsets' (void* ulong void* ulong*)>
	^ self externalCallFailed
]

{ #category : #'vertex_layout' }
AGPUCBindings >> addReference_vertex_layout: vertex_layout [
	<cdecl: long 'agpuAddVertexLayoutReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'vertex_layout' }
AGPUCBindings >> release_vertex_layout: vertex_layout [
	<cdecl: long 'agpuReleaseVertexLayout' (void*)>
	^ self externalCallFailed
]

{ #category : #'vertex_layout' }
AGPUCBindings >> addVertexAttributeBindings_vertex_layout: vertex_layout vertex_buffer_count: vertex_buffer_count vertex_strides: vertex_strides attribute_count: attribute_count attributes: attributes [
	<cdecl: long 'agpuAddVertexAttributeBindings' (void* ulong ulong* ulong AGPUVertexAttribDescription*)>
	^ self externalCallFailed
]

{ #category : #'shader' }
AGPUCBindings >> addReference_shader: shader [
	<cdecl: long 'agpuAddShaderReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'shader' }
AGPUCBindings >> release_shader: shader [
	<cdecl: long 'agpuReleaseShader' (void*)>
	^ self externalCallFailed
]

{ #category : #'shader' }
AGPUCBindings >> setShaderSource_shader: shader language: language sourceText: sourceText sourceTextLength: sourceTextLength [
	<cdecl: long 'agpuSetShaderSource' (void* long byte* long)>
	^ self externalCallFailed
]

{ #category : #'shader' }
AGPUCBindings >> compileShader_shader: shader options: options [
	<cdecl: long 'agpuCompileShader' (void* byte*)>
	^ self externalCallFailed
]

{ #category : #'shader' }
AGPUCBindings >> getCompilationLogLength_shader: shader [
	<cdecl: ulong 'agpuGetShaderCompilationLogLength' (void*)>
	^ self externalCallFailed
]

{ #category : #'shader' }
AGPUCBindings >> getCompilationLog_shader: shader buffer_size: buffer_size buffer: buffer [
	<cdecl: long 'agpuGetShaderCompilationLog' (void* ulong byte*)>
	^ self externalCallFailed
]

{ #category : #'framebuffer' }
AGPUCBindings >> addReference_framebuffer: framebuffer [
	<cdecl: long 'agpuAddFramebufferReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'framebuffer' }
AGPUCBindings >> release_framebuffer: framebuffer [
	<cdecl: long 'agpuReleaseFramebuffer' (void*)>
	^ self externalCallFailed
]

{ #category : #'framebuffer' }
AGPUCBindings >> getWidth_framebuffer: framebuffer [
	<cdecl: ulong 'agpuGetFramebufferWidth' (void*)>
	^ self externalCallFailed
]

{ #category : #'framebuffer' }
AGPUCBindings >> getHeight_framebuffer: framebuffer [
	<cdecl: ulong 'agpuGetFramebufferHeight' (void*)>
	^ self externalCallFailed
]

{ #category : #'renderpass' }
AGPUCBindings >> addReference_renderpass: renderpass [
	<cdecl: long 'agpuAddRenderPassReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'renderpass' }
AGPUCBindings >> release_renderpass: renderpass [
	<cdecl: long 'agpuReleaseRenderPass' (void*)>
	^ self externalCallFailed
]

{ #category : #'renderpass' }
AGPUCBindings >> setDepthStencilClearValue_renderpass: renderpass value: value [
	<cdecl: long 'agpuSetDepthStencilClearValue' (void* AGPUDepthStencilValue)>
	^ self externalCallFailed
]

{ #category : #'renderpass' }
AGPUCBindings >> setColorClearValue_renderpass: renderpass attachment_index: attachment_index value: value [
	<cdecl: long 'agpuSetColorClearValue' (void* ulong AGPUColor4f)>
	^ self externalCallFailed
]

{ #category : #'renderpass' }
AGPUCBindings >> setColorClearValueFrom_renderpass: renderpass attachment_index: attachment_index value: value [
	<cdecl: long 'agpuSetColorClearValueFrom' (void* ulong AGPUColor4f*)>
	^ self externalCallFailed
]

{ #category : #'renderpass' }
AGPUCBindings >> getColorAttachmentFormats_renderpass: renderpass color_attachment_count: color_attachment_count formats: formats [
	<cdecl: long 'agpuGetRenderPassColorAttachmentFormats' (void* ulong* long*)>
	^ self externalCallFailed
]

{ #category : #'renderpass' }
AGPUCBindings >> getDepthStencilAttachmentFormat_renderpass: renderpass [
	<cdecl: long 'agpuGetRenderPassDepthStencilAttachmentFormat' (void*)>
	^ self externalCallFailed
]

{ #category : #'renderpass' }
AGPUCBindings >> getSampleCount_renderpass: renderpass [
	<cdecl: ulong 'agpuGetRenderPassSampleCount' (void*)>
	^ self externalCallFailed
]

{ #category : #'renderpass' }
AGPUCBindings >> getSampleQuality_renderpass: renderpass [
	<cdecl: ulong 'agpuGetRenderPassSampleQuality' (void*)>
	^ self externalCallFailed
]

{ #category : #'shader_signature_builder' }
AGPUCBindings >> addReference_shader_signature_builder: shader_signature_builder [
	<cdecl: long 'agpuAddShaderSignatureBuilderReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'shader_signature_builder' }
AGPUCBindings >> release_shader_signature_builder: shader_signature_builder [
	<cdecl: long 'agpuReleaseShaderSignatureBuilder' (void*)>
	^ self externalCallFailed
]

{ #category : #'shader_signature_builder' }
AGPUCBindings >> build_shader_signature_builder: shader_signature_builder [
	<cdecl: void* 'agpuBuildShaderSignature' (void*)>
	^ self externalCallFailed
]

{ #category : #'shader_signature_builder' }
AGPUCBindings >> addBindingConstant_shader_signature_builder: shader_signature_builder [
	<cdecl: long 'agpuAddShaderSignatureBindingConstant' (void*)>
	^ self externalCallFailed
]

{ #category : #'shader_signature_builder' }
AGPUCBindings >> addBindingElement_shader_signature_builder: shader_signature_builder type: type maxBindings: maxBindings [
	<cdecl: long 'agpuAddShaderSignatureBindingElement' (void* long ulong)>
	^ self externalCallFailed
]

{ #category : #'shader_signature_builder' }
AGPUCBindings >> beginBindingBank_shader_signature_builder: shader_signature_builder maxBindings: maxBindings [
	<cdecl: long 'agpuBeginShaderSignatureBindingBank' (void* ulong)>
	^ self externalCallFailed
]

{ #category : #'shader_signature_builder' }
AGPUCBindings >> addBindingBankElement_shader_signature_builder: shader_signature_builder type: type bindingPointCount: bindingPointCount [
	<cdecl: long 'agpuAddShaderSignatureBindingBankElement' (void* long ulong)>
	^ self externalCallFailed
]

{ #category : #'shader_signature' }
AGPUCBindings >> addReference_shader_signature: shader_signature [
	<cdecl: long 'agpuAddShaderSignature' (void*)>
	^ self externalCallFailed
]

{ #category : #'shader_signature' }
AGPUCBindings >> release_shader_signature: shader_signature [
	<cdecl: long 'agpuReleaseShaderSignature' (void*)>
	^ self externalCallFailed
]

{ #category : #'shader_signature' }
AGPUCBindings >> createShaderResourceBinding_shader_signature: shader_signature element: element [
	<cdecl: void* 'agpuCreateShaderResourceBinding' (void* ulong)>
	^ self externalCallFailed
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> addReference_shader_resource_binding: shader_resource_binding [
	<cdecl: long 'agpuAddShaderResourceBindingReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> release_shader_resource_binding: shader_resource_binding [
	<cdecl: long 'agpuReleaseShaderResourceBinding' (void*)>
	^ self externalCallFailed
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> bindUniformBuffer_shader_resource_binding: shader_resource_binding location: location uniform_buffer: uniform_buffer [
	<cdecl: long 'agpuBindUniformBuffer' (void* long void*)>
	^ self externalCallFailed
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> bindUniformBufferRange_shader_resource_binding: shader_resource_binding location: location uniform_buffer: uniform_buffer offset: offset size: size [
	<cdecl: long 'agpuBindUniformBufferRange' (void* long void* ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> bindStorageBuffer_shader_resource_binding: shader_resource_binding location: location storage_buffer: storage_buffer [
	<cdecl: long 'agpuBindStorageBuffer' (void* long void*)>
	^ self externalCallFailed
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> bindStorageBufferRange_shader_resource_binding: shader_resource_binding location: location storage_buffer: storage_buffer offset: offset size: size [
	<cdecl: long 'agpuBindStorageBufferRange' (void* long void* ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> bindSampledTextureView_shader_resource_binding: shader_resource_binding location: location view: view [
	<cdecl: long 'agpuBindSampledTextureView' (void* long void*)>
	^ self externalCallFailed
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> bindStorageImageView_shader_resource_binding: shader_resource_binding location: location view: view [
	<cdecl: long 'agpuBindStorageImageView' (void* long void*)>
	^ self externalCallFailed
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> bindSampler_shader_resource_binding: shader_resource_binding location: location sampler: sampler [
	<cdecl: long 'agpuBindSampler' (void* long void*)>
	^ self externalCallFailed
]

{ #category : #'fence' }
AGPUCBindings >> addReference_fence: fence [
	<cdecl: long 'agpuAddFenceReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'fence' }
AGPUCBindings >> release_fence: fence [
	<cdecl: long 'agpuReleaseFenceReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'fence' }
AGPUCBindings >> waitOnClient_fence: fence [
	<cdecl: long 'agpuWaitOnClient' (void*)>
	^ self externalCallFailed
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> addReference_offline_shader_compiler: offline_shader_compiler [
	<cdecl: long 'agpuAddOfflineShaderCompilerReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> release_offline_shader_compiler: offline_shader_compiler [
	<cdecl: long 'agpuReleaseOfflineShaderCompiler' (void*)>
	^ self externalCallFailed
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> isShaderLanguageSupported_offline_shader_compiler: offline_shader_compiler language: language [
	<cdecl: long 'agpuIsShaderLanguageSupportedByOfflineCompiler' (void* long)>
	^ self externalCallFailed
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> isTargetShaderLanguageSupported_offline_shader_compiler: offline_shader_compiler language: language [
	<cdecl: long 'agpuIsTargetShaderLanguageSupportedByOfflineCompiler' (void* long)>
	^ self externalCallFailed
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> setShaderSource_offline_shader_compiler: offline_shader_compiler language: language stage: stage sourceText: sourceText sourceTextLength: sourceTextLength [
	<cdecl: long 'agpuSetOfflineShaderCompilerSource' (void* long long byte* long)>
	^ self externalCallFailed
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> compileShader_offline_shader_compiler: offline_shader_compiler target_language: target_language options: options [
	<cdecl: long 'agpuCompileOfflineShader' (void* long byte*)>
	^ self externalCallFailed
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> getCompilationLogLength_offline_shader_compiler: offline_shader_compiler [
	<cdecl: ulong 'agpuGetOfflineShaderCompilationLogLength' (void*)>
	^ self externalCallFailed
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> getCompilationLog_offline_shader_compiler: offline_shader_compiler buffer_size: buffer_size buffer: buffer [
	<cdecl: long 'agpuGetOfflineShaderCompilationLog' (void* ulong byte*)>
	^ self externalCallFailed
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> getCompilationResultLength_offline_shader_compiler: offline_shader_compiler [
	<cdecl: ulong 'agpuGetOfflineShaderCompilationResultLength' (void*)>
	^ self externalCallFailed
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> getCompilationResult_offline_shader_compiler: offline_shader_compiler buffer_size: buffer_size buffer: buffer [
	<cdecl: long 'agpuGetOfflineShaderCompilationResult' (void* ulong byte*)>
	^ self externalCallFailed
]

{ #category : #'offline_shader_compiler' }
AGPUCBindings >> getResultAsShader_offline_shader_compiler: offline_shader_compiler [
	<cdecl: void* 'agpuGetOfflineShaderCompilerResultAsShader' (void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker_cache' }
AGPUCBindings >> addReference_state_tracker_cache: state_tracker_cache [
	<cdecl: long 'agpuAddStateTrackerCacheReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker_cache' }
AGPUCBindings >> release_state_tracker_cache: state_tracker_cache [
	<cdecl: long 'agpuReleaseStateTrackerCacheReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker_cache' }
AGPUCBindings >> createStateTracker_state_tracker_cache: state_tracker_cache type: type command_queue: command_queue [
	<cdecl: void* 'agpuCreateStateTracker' (void* long void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker_cache' }
AGPUCBindings >> createStateTrackerWithCommandAllocator_state_tracker_cache: state_tracker_cache type: type command_queue: command_queue command_allocator: command_allocator [
	<cdecl: void* 'agpuCreateStateTrackerWithCommandAllocator' (void* long void* void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker_cache' }
AGPUCBindings >> createStateTrackerWithFrameBuffering_state_tracker_cache: state_tracker_cache type: type command_queue: command_queue framebuffering_count: framebuffering_count [
	<cdecl: void* 'agpuCreateStateTrackerWithFrameBuffering' (void* long void* ulong)>
	^ self externalCallFailed
]

{ #category : #'state_tracker_cache' }
AGPUCBindings >> createImmediateRenderer_state_tracker_cache: state_tracker_cache [
	<cdecl: void* 'agpuCreateImmediateRenderer' (void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> addReference_state_tracker: state_tracker [
	<cdecl: long 'agpuAddStateTrackerReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> release_state_tracker: state_tracker [
	<cdecl: long 'agpuReleaseStateTrackerReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> beginRecordingCommands_state_tracker: state_tracker [
	<cdecl: long 'agpuStateTrackerBeginRecordingCommands' (void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> endRecordingCommands_state_tracker: state_tracker [
	<cdecl: void* 'agpuStateTrackerEndRecordingCommands' (void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> endRecordingAndFlushCommands_state_tracker: state_tracker [
	<cdecl: long 'agpuStateTrackerEndRecordingAndFlushCommands' (void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> reset_state_tracker: state_tracker [
	<cdecl: long 'agpuStateTrackerReset' (void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> resetGraphicsPipeline_state_tracker: state_tracker [
	<cdecl: long 'agpuStateTrackerResetGraphicsPipeline' (void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> resetComputePipeline_state_tracker: state_tracker [
	<cdecl: long 'agpuStateTrackerResetComputePipeline' (void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setComputeStage_state_tracker: state_tracker shader: shader entryPoint: entryPoint [
	<cdecl: long 'agpuStateTrackerSetComputeStage' (void* void* byte*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setVertexStage_state_tracker: state_tracker shader: shader entryPoint: entryPoint [
	<cdecl: long 'agpuStateTrackerSetVertexStage' (void* void* byte*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setFragmentStage_state_tracker: state_tracker shader: shader entryPoint: entryPoint [
	<cdecl: long 'agpuStateTrackerSetFragmentStage' (void* void* byte*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setGeometryStage_state_tracker: state_tracker shader: shader entryPoint: entryPoint [
	<cdecl: long 'agpuStateTrackerSetGeometryStage' (void* void* byte*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setTessellationControlStage_state_tracker: state_tracker shader: shader entryPoint: entryPoint [
	<cdecl: long 'agpuStateTrackerSetTessellationControlStage' (void* void* byte*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setTessellationEvaluationStage_state_tracker: state_tracker shader: shader entryPoint: entryPoint [
	<cdecl: long 'agpuStateTrackerSetTessellationEvaluationStage' (void* void* byte*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setBlendState_state_tracker: state_tracker renderTargetMask: renderTargetMask enabled: enabled [
	<cdecl: long 'agpuStateTrackerSetBlendState' (void* long long)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setBlendFunction_state_tracker: state_tracker renderTargetMask: renderTargetMask sourceFactor: sourceFactor destFactor: destFactor colorOperation: colorOperation sourceAlphaFactor: sourceAlphaFactor destAlphaFactor: destAlphaFactor alphaOperation: alphaOperation [
	<cdecl: long 'agpuStateTrackerSetBlendFunction' (void* long long long long long long long)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setColorMask_state_tracker: state_tracker renderTargetMask: renderTargetMask redEnabled: redEnabled greenEnabled: greenEnabled blueEnabled: blueEnabled alphaEnabled: alphaEnabled [
	<cdecl: long 'agpuStateTrackerSetColorMask' (void* long long long long long)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setFrontFace_state_tracker: state_tracker winding: winding [
	<cdecl: long 'agpuStateTrackerSetFrontFace' (void* long)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setCullMode_state_tracker: state_tracker mode: mode [
	<cdecl: long 'agpuStateTrackerSetCullMode' (void* long)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setDepthBias_state_tracker: state_tracker constant_factor: constant_factor clamp: clamp slope_factor: slope_factor [
	<cdecl: long 'agpuStateTrackerSetDepthBias' (void* float float float)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setDepthState_state_tracker: state_tracker enabled: enabled writeMask: writeMask function: function [
	<cdecl: long 'agpuStateTrackerSetDepthState' (void* long long long)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setPolygonMode_state_tracker: state_tracker mode: mode [
	<cdecl: long 'agpuStateTrackerSetPolygonMode' (void* long)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setStencilState_state_tracker: state_tracker enabled: enabled writeMask: writeMask readMask: readMask [
	<cdecl: long 'agpuStateTrackerSetStencilState' (void* long long long)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setStencilFrontFace_state_tracker: state_tracker stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	<cdecl: long 'agpuStateTrackerSetStencilFrontFace' (void* long long long long)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setStencilBackFace_state_tracker: state_tracker stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	<cdecl: long 'agpuStateTrackerSetStencilBackFace' (void* long long long long)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setPrimitiveType_state_tracker: state_tracker type: type [
	<cdecl: long 'agpuStateTrackerSetPrimitiveType' (void* long)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setVertexLayout_state_tracker: state_tracker layout: layout [
	<cdecl: long 'agpuStateTrackerSetVertexLayout' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setShaderSignature_state_tracker: state_tracker signature: signature [
	<cdecl: long 'agpuStateTrackerSetShaderSignature' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setSampleDescription_state_tracker: state_tracker sample_count: sample_count sample_quality: sample_quality [
	<cdecl: long 'agpuStateTrackerSetSampleDescription' (void* ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setViewport_state_tracker: state_tracker x: x y: y w: w h: h [
	<cdecl: long 'agpuStateTrackerSetViewport' (void* long long long long)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setScissor_state_tracker: state_tracker x: x y: y w: w h: h [
	<cdecl: long 'agpuStateTrackerSetScissor' (void* long long long long)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> useVertexBinding_state_tracker: state_tracker vertex_binding: vertex_binding [
	<cdecl: long 'agpuStateTrackerUseVertexBinding' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> useIndexBuffer_state_tracker: state_tracker index_buffer: index_buffer [
	<cdecl: long 'agpuStateTrackerUseIndexBuffer' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> useIndexBufferAt_state_tracker: state_tracker index_buffer: index_buffer offset: offset index_size: index_size [
	<cdecl: long 'agpuStateTrackerUseIndexBufferAt' (void* void* ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> useDrawIndirectBuffer_state_tracker: state_tracker draw_buffer: draw_buffer [
	<cdecl: long 'agpuStateTrackerUseDrawIndirectBuffer' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> useComputeDispatchIndirectBuffer_state_tracker: state_tracker buffer: buffer [
	<cdecl: long 'agpuStateTrackerUseComputeDispatchIndirectBuffer' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> useShaderResources_state_tracker: state_tracker binding: binding [
	<cdecl: long 'agpuStateTrackerUseShaderResources' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> useComputeShaderResources_state_tracker: state_tracker binding: binding [
	<cdecl: long 'agpuStateTrackerUseComputeShaderResources' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> drawArrays_state_tracker: state_tracker vertex_count: vertex_count instance_count: instance_count first_vertex: first_vertex base_instance: base_instance [
	<cdecl: long 'agpuStateTrackerDrawArrays' (void* ulong ulong ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> drawArraysIndirect_state_tracker: state_tracker offset: offset drawcount: drawcount [
	<cdecl: long 'agpuStateTrackerDrawArraysIndirect' (void* ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> drawElements_state_tracker: state_tracker index_count: index_count instance_count: instance_count first_index: first_index base_vertex: base_vertex base_instance: base_instance [
	<cdecl: long 'agpuStateTrackerDrawElements' (void* ulong ulong ulong long ulong)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> drawElementsIndirect_state_tracker: state_tracker offset: offset drawcount: drawcount [
	<cdecl: long 'agpuStateTrackerDrawElementsIndirect' (void* ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> dispatchCompute_state_tracker: state_tracker group_count_x: group_count_x group_count_y: group_count_y group_count_z: group_count_z [
	<cdecl: long 'agpuStateTrackerDispatchCompute' (void* ulong ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> dispatchComputeIndirect_state_tracker: state_tracker offset: offset [
	<cdecl: long 'agpuStateTrackerDispatchComputeIndirect' (void* ulong)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> setStencilReference_state_tracker: state_tracker reference: reference [
	<cdecl: long 'agpuStateTrackerSetStencilReference' (void* ulong)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> executeBundle_state_tracker: state_tracker bundle: bundle [
	<cdecl: long 'agpuStateTrackerExecuteBundle' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> beginRenderPass_state_tracker: state_tracker renderpass: renderpass framebuffer: framebuffer bundle_content: bundle_content [
	<cdecl: long 'agpuStateTrackerBeginRenderPass' (void* void* void* long)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> endRenderPass_state_tracker: state_tracker [
	<cdecl: long 'agpuStateTrackerEndRenderPass' (void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> resolveFramebuffer_state_tracker: state_tracker destFramebuffer: destFramebuffer sourceFramebuffer: sourceFramebuffer [
	<cdecl: long 'agpuStateTrackerResolveFramebuffer' (void* void* void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> resolveTexture_state_tracker: state_tracker sourceTexture: sourceTexture sourceLevel: sourceLevel sourceLayer: sourceLayer destTexture: destTexture destLevel: destLevel destLayer: destLayer levelCount: levelCount layerCount: layerCount aspect: aspect [
	<cdecl: long 'agpuStateTrackerResolveTexture' (void* void* ulong ulong void* ulong ulong ulong ulong long)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> pushConstants_state_tracker: state_tracker offset: offset size: size values: values [
	<cdecl: long 'agpuStateTrackerPushConstants' (void* ulong ulong void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> memoryBarrier_state_tracker: state_tracker source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses [
	<cdecl: long 'agpuStateTrackerMemoryBarrier' (void* long long long long)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> bufferMemoryBarrier_state_tracker: state_tracker buffer: buffer source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses offset: offset size: size [
	<cdecl: long 'agpuStateTrackerBufferMemoryBarrier' (void* void* long long long long ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> textureMemoryBarrier_state_tracker: state_tracker texture: texture source_stage: source_stage dest_stage: dest_stage source_accesses: source_accesses dest_accesses: dest_accesses subresource_range: subresource_range [
	<cdecl: long 'agpuStateTrackerTextureMemoryBarrier' (void* void* long long long long AGPUSubresourceRange*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> pushBufferTransitionBarrier_state_tracker: state_tracker buffer: buffer new_usage: new_usage [
	<cdecl: long 'agpuStateTrackerPushBufferTransitionBarrier' (void* void* long)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> pushTextureTransitionBarrier_state_tracker: state_tracker texture: texture new_usage: new_usage subresource_range: subresource_range [
	<cdecl: long 'agpuStateTrackerPushTextureTransitionBarrier' (void* void* long AGPUSubresourceRange*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> popBufferTransitionBarrier_state_tracker: state_tracker [
	<cdecl: long 'agpuStateTrackerPopBufferTransitionBarrier' (void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> popTextureTransitionBarrier_state_tracker: state_tracker [
	<cdecl: long 'agpuStateTrackerPopTextureTransitionBarrier' (void*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> copyBuffer_state_tracker: state_tracker source_buffer: source_buffer source_offset: source_offset dest_buffer: dest_buffer dest_offset: dest_offset copy_size: copy_size [
	<cdecl: long 'agpuStateTrackerCopyBuffer' (void* void* ulong void* ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> copyBufferToTexture_state_tracker: state_tracker buffer: buffer texture: texture copy_region: copy_region [
	<cdecl: long 'agpuStateTrackerCopyBufferToTexture' (void* void* void* AGPUBufferImageCopyRegion*)>
	^ self externalCallFailed
]

{ #category : #'state_tracker' }
AGPUCBindings >> copyTextureToBuffer_state_tracker: state_tracker texture: texture buffer: buffer copy_region: copy_region [
	<cdecl: long 'agpuStateTrackerCopyTextureToBuffer' (void* void* void* AGPUBufferImageCopyRegion*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> addReference_immediate_renderer: immediate_renderer [
	<cdecl: long 'agpuAddImmediateRendererReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> release_immediate_renderer: immediate_renderer [
	<cdecl: long 'agpuReleaseImmediateRendererReference' (void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> beginRendering_immediate_renderer: immediate_renderer state_tracker: state_tracker [
	<cdecl: long 'agpuBeginImmediateRendering' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> endRendering_immediate_renderer: immediate_renderer [
	<cdecl: long 'agpuEndImmediateRendering' (void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setBlendState_immediate_renderer: immediate_renderer renderTargetMask: renderTargetMask enabled: enabled [
	<cdecl: long 'agpuImmediateRendererSetBlendState' (void* long long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setBlendFunction_immediate_renderer: immediate_renderer renderTargetMask: renderTargetMask sourceFactor: sourceFactor destFactor: destFactor colorOperation: colorOperation sourceAlphaFactor: sourceAlphaFactor destAlphaFactor: destAlphaFactor alphaOperation: alphaOperation [
	<cdecl: long 'agpuImmediateRendererSetBlendFunction' (void* long long long long long long long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setColorMask_immediate_renderer: immediate_renderer renderTargetMask: renderTargetMask redEnabled: redEnabled greenEnabled: greenEnabled blueEnabled: blueEnabled alphaEnabled: alphaEnabled [
	<cdecl: long 'agpuImmediateRendererSetColorMask' (void* long long long long long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setFrontFace_immediate_renderer: immediate_renderer winding: winding [
	<cdecl: long 'agpuImmediateRendererSetFrontFace' (void* long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setCullMode_immediate_renderer: immediate_renderer mode: mode [
	<cdecl: long 'agpuImmediateRendererSetCullMode' (void* long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setDepthBias_immediate_renderer: immediate_renderer constant_factor: constant_factor clamp: clamp slope_factor: slope_factor [
	<cdecl: long 'agpuImmediateRendererSetDepthBias' (void* float float float)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setDepthState_immediate_renderer: immediate_renderer enabled: enabled writeMask: writeMask function: function [
	<cdecl: long 'agpuImmediateRendererSetDepthState' (void* long long long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setPolygonMode_immediate_renderer: immediate_renderer mode: mode [
	<cdecl: long 'agpuImmediateRendererSetPolygonMode' (void* long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setStencilState_immediate_renderer: immediate_renderer enabled: enabled writeMask: writeMask readMask: readMask [
	<cdecl: long 'agpuImmediateRendererSetStencilState' (void* long long long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setStencilFrontFace_immediate_renderer: immediate_renderer stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	<cdecl: long 'agpuImmediateRendererSetStencilFrontFace' (void* long long long long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setStencilBackFace_immediate_renderer: immediate_renderer stencilFailOperation: stencilFailOperation depthFailOperation: depthFailOperation stencilDepthPassOperation: stencilDepthPassOperation stencilFunction: stencilFunction [
	<cdecl: long 'agpuImmediateRendererSetStencilBackFace' (void* long long long long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setSamplingMode_immediate_renderer: immediate_renderer filter: filter maxAnisotropy: maxAnisotropy addressU: addressU addressV: addressV addressW: addressW [
	<cdecl: long 'agpuImmediateSetSamplingMode' (void* long float long long long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setViewport_immediate_renderer: immediate_renderer x: x y: y w: w h: h [
	<cdecl: long 'agpuImmediateRendererSetViewport' (void* long long long long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setScissor_immediate_renderer: immediate_renderer x: x y: y w: w h: h [
	<cdecl: long 'agpuImmediateRendererSetScissor' (void* long long long long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setStencilReference_immediate_renderer: immediate_renderer reference: reference [
	<cdecl: long 'agpuImmediateRendererSetStencilReference' (void* ulong)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> projectionMatrixMode_immediate_renderer: immediate_renderer [
	<cdecl: long 'agpuImmediateRendererProjectionMatrixMode' (void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> modelViewMatrixMode_immediate_renderer: immediate_renderer [
	<cdecl: long 'agpuImmediateRendererModelViewMatrixMode' (void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> textureMatrixMode_immediate_renderer: immediate_renderer [
	<cdecl: long 'agpuImmediateRendererTextureMatrixMode' (void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> loadIdentity_immediate_renderer: immediate_renderer [
	<cdecl: long 'agpuImmediateRendererIdentity' (void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> pushMatrix_immediate_renderer: immediate_renderer [
	<cdecl: long 'agpuImmediateRendererPushMatrix' (void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> popMatrix_immediate_renderer: immediate_renderer [
	<cdecl: long 'agpuImmediateRendererPopMatrix' (void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> loadMatrix_immediate_renderer: immediate_renderer elements: elements [
	<cdecl: long 'agpuImmediateRendererLoadMatrix' (void* float*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> loadTransposeMatrix_immediate_renderer: immediate_renderer elements: elements [
	<cdecl: long 'agpuImmediateRendererLoadTransposeMatrix' (void* float*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> multiplyMatrix_immediate_renderer: immediate_renderer elements: elements [
	<cdecl: long 'agpuImmediateRendererMultiplyMatrix' (void* float*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> multiplyTransposeMatrix_immediate_renderer: immediate_renderer elements: elements [
	<cdecl: long 'agpuImmediateRendererMultiplyTransposeMatrix' (void* float*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> ortho_immediate_renderer: immediate_renderer left: left right: right bottom: bottom top: top near: near far: far [
	<cdecl: long 'agpuImmediateRendererOrtho' (void* float float float float float float)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> frustum_immediate_renderer: immediate_renderer left: left right: right bottom: bottom top: top near: near far: far [
	<cdecl: long 'agpuImmediateRendererFrustum' (void* float float float float float float)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> perspective_immediate_renderer: immediate_renderer fovy: fovy aspect: aspect near: near far: far [
	<cdecl: long 'agpuImmediateRendererPerspective' (void* float float float float)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> rotate_immediate_renderer: immediate_renderer angle: angle x: x y: y z: z [
	<cdecl: long 'agpuImmediateRendererRotate' (void* float float float float)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> translate_immediate_renderer: immediate_renderer x: x y: y z: z [
	<cdecl: long 'agpuImmediateRendererTranslate' (void* float float float)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> scale_immediate_renderer: immediate_renderer x: x y: y z: z [
	<cdecl: long 'agpuImmediateRendererScale' (void* float float float)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setFlatShading_immediate_renderer: immediate_renderer enabled: enabled [
	<cdecl: long 'agpuImmediateRendererSetFlatShading' (void* long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setLightingEnabled_immediate_renderer: immediate_renderer enabled: enabled [
	<cdecl: long 'agpuImmediateRendererSetLightingEnabled' (void* long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setLightingModel_immediate_renderer: immediate_renderer model: model [
	<cdecl: long 'agpuImmediateRendererSetLightingModel' (void* long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> clearLights_immediate_renderer: immediate_renderer [
	<cdecl: long 'agpuImmediateRendererClearLights' (void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setAmbientLighting_immediate_renderer: immediate_renderer r: r g: g b: b a: a [
	<cdecl: long 'agpuImmediateRendererSetAmbientLighting' (void* float float float float)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setLight_immediate_renderer: immediate_renderer index: index enabled: enabled state: state [
	<cdecl: long 'agpuImmediateRendererSetLight' (void* ulong long AGPUImmediateRendererLight*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setMaterial_immediate_renderer: immediate_renderer state: state [
	<cdecl: long 'agpuImmediateRendererSetMaterial' (void* AGPUImmediateRendererMaterial*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setSkinningEnabled_immediate_renderer: immediate_renderer enabled: enabled [
	<cdecl: long 'agpuImmediateRendererSetSkinningEnabled' (void* long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setSkinBones_immediate_renderer: immediate_renderer count: count matrices: matrices transpose: transpose [
	<cdecl: long 'agpuImmediateRendererSetSkinBones' (void* ulong float* long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setTexturingEnabled_immediate_renderer: immediate_renderer enabled: enabled [
	<cdecl: long 'agpuImmediateRendererSetTextureEnabled' (void* long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setTangentSpaceEnabled_immediate_renderer: immediate_renderer enabled: enabled [
	<cdecl: long 'agpuImmediateRendererSetTangentSpaceEnabled' (void* long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> bindTexture_immediate_renderer: immediate_renderer texture: texture [
	<cdecl: long 'agpuImmediateRendererBindTexture' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> bindTextureIn_immediate_renderer: immediate_renderer texture: texture binding: binding [
	<cdecl: long 'agpuImmediateRendererBindTextureIn' (void* void* long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setClipPlane_immediate_renderer: immediate_renderer index: index enabled: enabled p1: p1 p2: p2 p3: p3 p4: p4 [
	<cdecl: long 'agpuImmediateRendererSetClipPlane' (void* ulong long float float float float)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setFogMode_immediate_renderer: immediate_renderer mode: mode [
	<cdecl: long 'agpuImmediateRendererSetFogMode' (void* long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setFogColor_immediate_renderer: immediate_renderer r: r g: g b: b a: a [
	<cdecl: long 'agpuImmediateRendererSetFogColor' (void* float float float float)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setFogDistances_immediate_renderer: immediate_renderer start: start end: end [
	<cdecl: long 'agpuImmediateRendererSetFogDistances' (void* float float)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setFogDensity_immediate_renderer: immediate_renderer density: density [
	<cdecl: long 'agpuImmediateRendererSetFogDensity' (void* float)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> beginPrimitives_immediate_renderer: immediate_renderer type: type [
	<cdecl: long 'agpuBeginImmediateRendererPrimitives' (void* long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> endPrimitives_immediate_renderer: immediate_renderer [
	<cdecl: long 'agpuEndImmediateRendererPrimitives' (void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> color_immediate_renderer: immediate_renderer r: r g: g b: b a: a [
	<cdecl: long 'agpuSetImmediateRendererColor' (void* float float float float)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> texcoord_immediate_renderer: immediate_renderer x: x y: y [
	<cdecl: long 'agpuSetImmediateRendererTexcoord' (void* float float)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> normal_immediate_renderer: immediate_renderer x: x y: y z: z [
	<cdecl: long 'agpuSetImmediateRendererNormal' (void* float float float)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> vertex_immediate_renderer: immediate_renderer x: x y: y z: z [
	<cdecl: long 'agpuAddImmediateRendererVertex' (void* float float float)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> beginMeshWithVertices_immediate_renderer: immediate_renderer vertexCount: vertexCount stride: stride elementCount: elementCount vertices: vertices [
	<cdecl: long 'agpuBeginImmediateRendererMeshWithVertices' (void* ulong ulong ulong void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> beginMeshWithVertexBinding_immediate_renderer: immediate_renderer layout: layout vertices: vertices [
	<cdecl: long 'agpuBeginImmediateRendererMeshWithVertexBinding' (void* void* void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> useIndexBuffer_immediate_renderer: immediate_renderer index_buffer: index_buffer [
	<cdecl: long 'agpuImmediateRendererUseIndexBuffer' (void* void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> useIndexBufferAt_immediate_renderer: immediate_renderer index_buffer: index_buffer offset: offset index_size: index_size [
	<cdecl: long 'agpuImmediateRendererUseIndexBufferAt' (void* void* ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setCurrentMeshColors_immediate_renderer: immediate_renderer stride: stride elementCount: elementCount colors: colors [
	<cdecl: long 'agpuSetImmediateRendererCurrentMeshColors' (void* ulong ulong void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setCurrentMeshNormals_immediate_renderer: immediate_renderer stride: stride elementCount: elementCount normals: normals [
	<cdecl: long 'agpuSetImmediateRendererCurrentMeshNormals' (void* ulong ulong void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setCurrentMeshTexCoords_immediate_renderer: immediate_renderer stride: stride elementCount: elementCount texcoords: texcoords [
	<cdecl: long 'agpuSetImmediateRendererCurrentMeshTexCoords' (void* ulong ulong void*)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> setPrimitiveType_immediate_renderer: immediate_renderer type: type [
	<cdecl: long 'agpuImmediateRendererSetPrimitiveType' (void* long)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> drawArrays_immediate_renderer: immediate_renderer vertex_count: vertex_count instance_count: instance_count first_vertex: first_vertex base_instance: base_instance [
	<cdecl: long 'agpuImmediateRendererDrawArrays' (void* ulong ulong ulong ulong)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> drawElements_immediate_renderer: immediate_renderer index_count: index_count instance_count: instance_count first_index: first_index base_vertex: base_vertex base_instance: base_instance [
	<cdecl: long 'agpuImmediateRendererDrawElements' (void* ulong ulong ulong long ulong)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> drawElementsWithIndices_immediate_renderer: immediate_renderer mode: mode indices: indices index_count: index_count instance_count: instance_count first_index: first_index base_vertex: base_vertex base_instance: base_instance [
	<cdecl: long 'agpuImmediateRendererDrawElementsWithIndices' (void* long void* ulong ulong ulong long ulong)>
	^ self externalCallFailed
]

{ #category : #'immediate_renderer' }
AGPUCBindings >> endMesh_immediate_renderer: immediate_renderer [
	<cdecl: long 'agpuEndImmediateRendererMesh' (void*)>
	^ self externalCallFailed
]

{ #category : #'global c functions' }
AGPUCBindings >> getPlatforms_numplatforms: numplatforms platforms: platforms ret_numplatforms: ret_numplatforms [
	<cdecl: long 'agpuGetPlatforms' (ulong void* ulong*)>
	^ self externalCallFailed
]

