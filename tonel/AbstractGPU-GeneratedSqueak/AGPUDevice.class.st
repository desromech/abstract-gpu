Class {
	#name : #AGPUDevice,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'wrappers' }
AGPUDevice >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_device: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_device: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> getDefaultCommandQueue [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getDefaultCommandQueue_device: (self validHandle).
	^ AGPUCommandQueue forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createSwapChain: commandQueue swapChainInfo: swapChainInfo [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createSwapChain_device: (self validHandle) commandQueue: (self validHandleOf: commandQueue) swapChainInfo: swapChainInfo.
	^ AGPUSwapChain forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createBuffer: description initial_data: initial_data [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createBuffer_device: (self validHandle) description: description initial_data: initial_data.
	^ AGPUBuffer forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createVertexLayout [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createVertexLayout_device: (self validHandle).
	^ AGPUVertexLayout forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createVertexBinding: layout [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createVertexBinding_device: (self validHandle) layout: (self validHandleOf: layout).
	^ AGPUVertexBinding forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createShader: type [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createShader_device: (self validHandle) type: type.
	^ AGPUShader forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createShaderSignatureBuilder [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createShaderSignatureBuilder_device: (self validHandle).
	^ AGPUShaderSignatureBuilder forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createPipelineBuilder [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createPipelineBuilder_device: (self validHandle).
	^ AGPUPipelineBuilder forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createComputePipelineBuilder [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createComputePipelineBuilder_device: (self validHandle).
	^ AGPUComputePipelineBuilder forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createCommandAllocator: type queue: queue [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createCommandAllocator_device: (self validHandle) type: type queue: (self validHandleOf: queue).
	^ AGPUCommandAllocator forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createCommandList: type allocator: allocator initial_pipeline_state: initial_pipeline_state [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createCommandList_device: (self validHandle) type: type allocator: (self validHandleOf: allocator) initial_pipeline_state: (self validHandleOf: initial_pipeline_state).
	^ AGPUCommandList forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> getPreferredShaderLanguage [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getPreferredShaderLanguage_device: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> getPreferredIntermediateShaderLanguage [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getPreferredIntermediateShaderLanguage_device: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> getPreferredHighLevelShaderLanguage [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getPreferredHighLevelShaderLanguage_device: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createFrameBuffer: width height: height colorCount: colorCount colorViews: colorViews depthStencilView: depthStencilView [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createFrameBuffer_device: (self validHandle) width: width height: height colorCount: colorCount colorViews: colorViews depthStencilView: (self validHandleOf: depthStencilView).
	^ AGPUFramebuffer forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createRenderPass: description [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createRenderPass_device: (self validHandle) description: description.
	^ AGPURenderpass forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createTexture: description [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createTexture_device: (self validHandle) description: description.
	^ AGPUTexture forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createSampler: description [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createSampler_device: (self validHandle) description: description.
	^ AGPUSampler forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createFence [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createFence_device: (self validHandle).
	^ AGPUFence forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> getMultiSampleQualityLevels: format sample_count: sample_count [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getMultiSampleQualityLevels_device: (self validHandle) format: format sample_count: sample_count.
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> hasTopLeftNdcOrigin [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance hasTopLeftNdcOrigin_device: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> hasBottomLeftTextureCoordinates [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance hasBottomLeftTextureCoordinates_device: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> getName [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getName_device: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> getType [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getType_device: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> isFeatureSupported: feature [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance isFeatureSupported_device: (self validHandle) feature: feature.
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> getLimitValue: limit [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getLimitValue_device: (self validHandle) limit: limit.
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> getVRSystem [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getVRSystem_device: (self validHandle).
	^ AGPUVrSystem forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createWindowScraper [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createWindowScraper_device: (self validHandle).
	^ AGPUWindowScraper forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createOfflineShaderCompiler [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createOfflineShaderCompiler_device: (self validHandle).
	^ AGPUOfflineShaderCompiler forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> createStateTrackerCache: command_queue_family [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createStateTrackerCache_device: (self validHandle) command_queue_family: (self validHandleOf: command_queue_family).
	^ AGPUStateTrackerCache forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUDevice >> finishExecution [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance finishExecution_device: (self validHandle).
	self checkErrorCode: resultValue_
]

