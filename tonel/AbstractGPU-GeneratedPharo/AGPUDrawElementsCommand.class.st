Class {
	#name : #AGPUDrawElementsCommand,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_BASE_INSTANCE',
		'OFFSET_BASE_VERTEX',
		'OFFSET_FIRST_INDEX',
		'OFFSET_INDEX_COUNT',
		'OFFSET_INSTANCE_COUNT'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUDrawElementsCommand class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint index_count;
		 agpu_uint instance_count;
		 agpu_uint first_index;
		 agpu_int base_vertex;
		 agpu_uint base_instance;
	)
]

{ #category : #'accessing - structure variables' }
AGPUDrawElementsCommand >> base_instance [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_BASE_INSTANCE
]

{ #category : #'accessing - structure variables' }
AGPUDrawElementsCommand >> base_instance: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_BASE_INSTANCE put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUDrawElementsCommand >> base_vertex [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_BASE_VERTEX
]

{ #category : #'accessing - structure variables' }
AGPUDrawElementsCommand >> base_vertex: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_BASE_VERTEX put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUDrawElementsCommand >> first_index [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_FIRST_INDEX
]

{ #category : #'accessing - structure variables' }
AGPUDrawElementsCommand >> first_index: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_FIRST_INDEX put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUDrawElementsCommand >> index_count [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_INDEX_COUNT
]

{ #category : #'accessing - structure variables' }
AGPUDrawElementsCommand >> index_count: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_INDEX_COUNT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUDrawElementsCommand >> instance_count [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_INSTANCE_COUNT
]

{ #category : #'accessing - structure variables' }
AGPUDrawElementsCommand >> instance_count: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_INSTANCE_COUNT put: anObject
]
