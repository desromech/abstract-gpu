Class {
	#name : #AGPUTextureClearValue,
	#superclass : #FFIExternalUnion,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUTextureClearValue class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_color4f color;
		 agpu_depth_stencil_value depth_stencil;
	)
]

{ #category : #'accessing - structure variables' }
AGPUTextureClearValue >> color [
	"This method was automatically generated"
	^ AGPUColor4f fromHandle: (handle referenceStructAt: 1 length: AGPUColor4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUTextureClearValue >> color: anObject [
	"This method was automatically generated"
	handle structAt: 1 put: anObject getHandle length: 16.
]

{ #category : #'accessing - structure variables' }
AGPUTextureClearValue >> depth_stencil [
	"This method was automatically generated"
	^ AGPUDepthStencilValue fromHandle: (handle referenceStructAt: 1 length: AGPUDepthStencilValue byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUTextureClearValue >> depth_stencil: anObject [
	"This method was automatically generated"
	handle structAt: 1 put: anObject getHandle length: 8.
]
