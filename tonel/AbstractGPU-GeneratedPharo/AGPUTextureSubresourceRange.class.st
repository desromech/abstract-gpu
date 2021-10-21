Class {
	#name : #AGPUTextureSubresourceRange,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_ASPECT',
		'OFFSET_BASE_ARRAYLAYER',
		'OFFSET_BASE_MIPLEVEL',
		'OFFSET_LAYER_COUNT',
		'OFFSET_LEVEL_COUNT'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUTextureSubresourceRange class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_texture_aspect aspect;
		 agpu_uint base_miplevel;
		 agpu_uint level_count;
		 agpu_uint base_arraylayer;
		 agpu_uint layer_count;
	)
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceRange >> aspect [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_ASPECT
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceRange >> aspect: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_ASPECT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceRange >> base_arraylayer [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_BASE_ARRAYLAYER
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceRange >> base_arraylayer: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_BASE_ARRAYLAYER put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceRange >> base_miplevel [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_BASE_MIPLEVEL
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceRange >> base_miplevel: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_BASE_MIPLEVEL put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceRange >> layer_count [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_LAYER_COUNT
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceRange >> layer_count: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_LAYER_COUNT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceRange >> level_count [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_LEVEL_COUNT
]

{ #category : #'accessing - structure variables' }
AGPUTextureSubresourceRange >> level_count: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_LEVEL_COUNT put: anObject
]
