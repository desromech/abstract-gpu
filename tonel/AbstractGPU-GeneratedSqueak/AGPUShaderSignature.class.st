Class {
	#name : #AGPUShaderSignature,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'wrappers' }
AGPUShaderSignature >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_shader_signature: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShaderSignature >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_shader_signature: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUShaderSignature >> createShaderResourceBinding: element [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance createShaderResourceBinding_shader_signature: (self validHandle) element: element.
	^ AGPUShaderResourceBinding forHandle: resultValue_
]

