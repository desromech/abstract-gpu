Class {
	#name : #AGPUFramebuffer,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedPharo'
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

{ #category : #'wrappers' }
AGPUFramebuffer >> getWidth [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getWidth_framebuffer: (self validHandle).
	^ resultValue_
]

{ #category : #'wrappers' }
AGPUFramebuffer >> getHeight [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getHeight_framebuffer: (self validHandle).
	^ resultValue_
]

