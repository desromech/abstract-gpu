Class {
	#name : #AGPUCBindings,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #AGPUCBindingsBase,
	#category : 'AbstractGPU-Generated'
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
	^ self ffiCall: #(agpu_framebuffer* agpuCreateFrameBuffer (agpu_device* device , agpu_uint width , agpu_uint height , agpu_uint colorCount , agpu_texture_view_description* colorViews , agpu_texture_view_description* depthStencilView) )
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
AGPUCBindings >> createFence_device: device [
	^ self ffiCall: #(agpu_fence* agpuCreateFence (agpu_device* device) )
]

{ #category : #'device' }
AGPUCBindings >> getMultiSampleQualityLevels_device: device sample_count: sample_count [
	^ self ffiCall: #(agpu_int agpuGetMultiSampleQualityLevels (agpu_device* device , agpu_uint sample_count) )
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
AGPUCBindings >> isFeatureSupported_device: device feature: feature [
	^ self ffiCall: #(agpu_bool agpuIsFeatureSupportedOnDevice (agpu_device* device , agpu_feature feature) )
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
AGPUCBindings >> getCurrentBackBufferIndex_swap_chain: swap_chain [
	^ self ffiCall: #(agpu_size agpuGetCurrentBackBufferIndex (agpu_swap_chain* swap_chain) )
]

{ #category : #'swap_chain' }
AGPUCBindings >> getFramebufferCount_swap_chain: swap_chain [
	^ self ffiCall: #(agpu_size agpuGetFramebufferCount (agpu_swap_chain* swap_chain) )
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
AGPUCBindings >> useComputeShaderResources_command_list: command_list binding: binding [
	^ self ffiCall: #(agpu_error agpuUseComputeShaderResources (agpu_command_list* command_list , agpu_shader_resource_binding* binding) )
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
AGPUCBindings >> pushConstants_command_list: command_list offset: offset size: size values: values [
	^ self ffiCall: #(agpu_error agpuPushConstants (agpu_command_list* command_list , agpu_uint offset , agpu_uint size , agpu_pointer values) )
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
AGPUCBindings >> uploadTextureData_texture: texture level: level arrayIndex: arrayIndex pitch: pitch slicePitch: slicePitch data: data [
	^ self ffiCall: #(agpu_error agpuUploadTextureData (agpu_texture* texture , agpu_int level , agpu_int arrayIndex , agpu_int pitch , agpu_int slicePitch , agpu_pointer data) )
]

{ #category : #'texture' }
AGPUCBindings >> uploadTextureSubData_texture: texture level: level arrayIndex: arrayIndex pitch: pitch slicePitch: slicePitch sourceSize: sourceSize destRegion: destRegion data: data [
	^ self ffiCall: #(agpu_error agpuUploadTextureSubData (agpu_texture* texture , agpu_int level , agpu_int arrayIndex , agpu_int pitch , agpu_int slicePitch , agpu_size3d* sourceSize , agpu_region3d* destRegion , agpu_pointer data) )
]

{ #category : #'texture' }
AGPUCBindings >> discardUploadBuffer_texture: texture [
	^ self ffiCall: #(agpu_error agpuDiscardTextureUploadBuffer (agpu_texture* texture) )
]

{ #category : #'texture' }
AGPUCBindings >> discardReadbackBuffer_texture: texture [
	^ self ffiCall: #(agpu_error agpuDiscardTextureReadbackBuffer (agpu_texture* texture) )
]

{ #category : #'texture' }
AGPUCBindings >> getFullViewDescription_texture: texture result: result [
	^ self ffiCall: #(agpu_error agpuGetTextureFullViewDescription (agpu_texture* texture , agpu_texture_view_description* result) )
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
AGPUCBindings >> bindTexture_shader_resource_binding: shader_resource_binding location: location texture: texture startMiplevel: startMiplevel miplevels: miplevels lodclamp: lodclamp [
	^ self ffiCall: #(agpu_error agpuBindTexture (agpu_shader_resource_binding* shader_resource_binding , agpu_int location , agpu_texture* texture , agpu_uint startMiplevel , agpu_int miplevels , agpu_float lodclamp) )
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> bindTextureArrayRange_shader_resource_binding: shader_resource_binding location: location texture: texture startMiplevel: startMiplevel miplevels: miplevels firstElement: firstElement numberOfElements: numberOfElements lodclamp: lodclamp [
	^ self ffiCall: #(agpu_error agpuBindTextureArrayRange (agpu_shader_resource_binding* shader_resource_binding , agpu_int location , agpu_texture* texture , agpu_uint startMiplevel , agpu_int miplevels , agpu_int firstElement , agpu_int numberOfElements , agpu_float lodclamp) )
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> bindImage_shader_resource_binding: shader_resource_binding location: location texture: texture level: level layer: layer access: access format: format [
	^ self ffiCall: #(agpu_error agpuBindImage (agpu_shader_resource_binding* shader_resource_binding , agpu_int location , agpu_texture* texture , agpu_int level , agpu_int layer , agpu_mapping_access access , agpu_texture_format format) )
]

{ #category : #'shader_resource_binding' }
AGPUCBindings >> createSampler_shader_resource_binding: shader_resource_binding location: location description: description [
	^ self ffiCall: #(agpu_error agpuCreateSampler (agpu_shader_resource_binding* shader_resource_binding , agpu_int location , agpu_sampler_description* description) )
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

{ #category : #'global c functions' }
AGPUCBindings >> getPlatforms_numplatforms: numplatforms platforms: platforms ret_numplatforms: ret_numplatforms [
	^ self ffiCall: #(agpu_error agpuGetPlatforms (agpu_size numplatforms , agpu_platform* platforms , agpu_size* ret_numplatforms) )
]

