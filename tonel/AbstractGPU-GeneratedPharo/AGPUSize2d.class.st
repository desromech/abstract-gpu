Class {
	#name : #AGPUSize2d,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_HEIGHT',
		'OFFSET_WIDTH'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUSize2d class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint width;
		 agpu_uint height;
	)
]

{ #category : #'accessing - structure variables' }
AGPUSize2d >> height [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_HEIGHT
]

{ #category : #'accessing - structure variables' }
AGPUSize2d >> height: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_HEIGHT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSize2d >> width [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_WIDTH
]

{ #category : #'accessing - structure variables' }
AGPUSize2d >> width: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_WIDTH put: anObject
]
