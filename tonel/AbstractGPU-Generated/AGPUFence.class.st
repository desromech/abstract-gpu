Class {
	#name : #AGPUFence,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'wrappers' }
AGPUFence >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_fence: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUFence >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_fence: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUFence >> waitOnClient [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance waitOnClient_fence: (self validHandle).
	self checkErrorCode: resultValue_
]

