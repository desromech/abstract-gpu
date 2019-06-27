Class {
	#name : #AGPUTextureView,
	#superclass : #AGPUInterface,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'wrappers' }
AGPUTextureView >> addReference [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance addReference_texture_view: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUTextureView >> primitiveRelease [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance release_texture_view: (self validHandle).
	self checkErrorCode: resultValue_
]

{ #category : #'wrappers' }
AGPUTextureView >> getTexture [
	| resultValue_ |
	resultValue_ := AGPUCBindings uniqueInstance getTexture_texture_view: (self validHandle).
	^ AGPUTexture forHandle: resultValue_
]

