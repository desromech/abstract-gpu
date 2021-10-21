Class {
	#name : #AGPUSize3d,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_DEPTH',
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
AGPUSize3d class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint width;
		 agpu_uint height;
		 agpu_uint depth;
	)
]

{ #category : #'accessing - structure variables' }
AGPUSize3d >> depth [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_DEPTH
]

{ #category : #'accessing - structure variables' }
AGPUSize3d >> depth: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_DEPTH put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSize3d >> height [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_HEIGHT
]

{ #category : #'accessing - structure variables' }
AGPUSize3d >> height: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_HEIGHT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSize3d >> width [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_WIDTH
]

{ #category : #'accessing - structure variables' }
AGPUSize3d >> width: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_WIDTH put: anObject
]
