Class {
	#name : #AGPUDepthStencilValue,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_DEPTH',
		'OFFSET_STENCIL'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUDepthStencilValue class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_float depth;
		 agpu_byte stencil;
	)
]

{ #category : #'accessing - structure variables' }
AGPUDepthStencilValue >> depth [
	"This method was automatically generated"
	^handle floatAt: OFFSET_DEPTH
]

{ #category : #'accessing - structure variables' }
AGPUDepthStencilValue >> depth: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_DEPTH put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUDepthStencilValue >> stencil [
	"This method was automatically generated"
	^handle unsignedByteAt: OFFSET_STENCIL
]

{ #category : #'accessing - structure variables' }
AGPUDepthStencilValue >> stencil: anObject [
	"This method was automatically generated"
	handle unsignedByteAt: OFFSET_STENCIL put: anObject
]
