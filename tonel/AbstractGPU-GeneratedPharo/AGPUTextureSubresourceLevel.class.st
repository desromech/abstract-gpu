Class {
	#name : #AGPUTextureSubresourceLevel,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_ASPECT',
		'OFFSET_BASE_ARRAYLAYER',
		'OFFSET_LAYER_COUNT',
		'OFFSET_MIPLEVEL'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUTextureSubresourceLevel class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_texture_aspect aspect;
		 agpu_uint miplevel;
		 agpu_uint base_arraylayer;
		 agpu_uint layer_count;
	)
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceLevel >> aspect [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_ASPECT
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceLevel >> aspect: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_ASPECT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceLevel >> base_arraylayer [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_BASE_ARRAYLAYER
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceLevel >> base_arraylayer: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_BASE_ARRAYLAYER put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceLevel >> layer_count [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_LAYER_COUNT
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceLevel >> layer_count: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_LAYER_COUNT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceLevel >> miplevel [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_MIPLEVEL
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceLevel >> miplevel: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_MIPLEVEL put: anObject
]
