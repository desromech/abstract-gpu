Class {
	#name : #AGPUFramebuffer,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'wrappers' }
AGPUFramebuffer >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_framebuffer: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUFramebuffer >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_framebuffer: (self validHandle).
	self checkErrorCode: resultValue_
]

