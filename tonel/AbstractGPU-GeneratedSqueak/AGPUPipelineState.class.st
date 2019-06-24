Class {
	#name : #AGPUPipelineState,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'wrappers' }
AGPUPipelineState >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_pipeline_state: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUPipelineState >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_pipeline_state: (self validHandle).
	self checkErrorCode: resultValue_
]

