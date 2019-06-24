Class {
	#name : #AGPUComputePipelineBuilder,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'wrappers' }
AGPUComputePipelineBuilder >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_compute_pipeline_builder: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUComputePipelineBuilder >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_compute_pipeline_builder: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUComputePipelineBuilder >> build [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance build_compute_pipeline_builder: (self validHandle).
	^ AGPUPipelineState forHandle: resultValue_
]

{ #category : #'wrappers' }
AGPUComputePipelineBuilder >> attachShader: shader [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance attachShader_compute_pipeline_builder: (self validHandle) shader: (self validHandleOf: shader).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUComputePipelineBuilder >> attachShaderWithEntryPoint: shader type: type entry_point: entry_point [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance attachShaderWithEntryPoint_compute_pipeline_builder: (self validHandle) shader: (self validHandleOf: shader) type: type entry_point: entry_point.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUComputePipelineBuilder >> getBuildingLogLength [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getBuildingLogLength_compute_pipeline_builder: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUComputePipelineBuilder >> getBuildingLog: buffer_size buffer: buffer [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getBuildingLog_compute_pipeline_builder: (self validHandle) buffer_size: buffer_size buffer: buffer.
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUComputePipelineBuilder >> setShaderSignature: signature [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance setShaderSignature_compute_pipeline_builder: (self validHandle) signature: (self validHandleOf: signature).
	self checkErrorCode: resultValue_
]

