Class {
	#name : #AGPURegion3d,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_DEPTH',
		'OFFSET_HEIGHT',
		'OFFSET_WIDTH',
		'OFFSET_X',
		'OFFSET_Y',
		'OFFSET_Z'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPURegion3d class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint x;
		 agpu_uint y;
		 agpu_uint z;
		 agpu_uint width;
		 agpu_uint height;
		 agpu_uint depth;
	)
]

{ #category : #'accessing - structure variables' }
AGPURegion3d >> depth [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_DEPTH
]

{ #category : #'accessing - structure variables' }
AGPURegion3d >> depth: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_DEPTH put: anObject
]

{ #category : #'accessing - structure variables' }
AGPURegion3d >> height [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_HEIGHT
]

{ #category : #'accessing - structure variables' }
AGPURegion3d >> height: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_HEIGHT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPURegion3d >> width [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_WIDTH
]

{ #category : #'accessing - structure variables' }
AGPURegion3d >> width: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_WIDTH put: anObject
]

{ #category : #'accessing - structure variables' }
AGPURegion3d >> x [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_X
]

{ #category : #'accessing - structure variables' }
AGPURegion3d >> x: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_X put: anObject
]

{ #category : #'accessing - structure variables' }
AGPURegion3d >> y [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_Y
]

{ #category : #'accessing - structure variables' }
AGPURegion3d >> y: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_Y put: anObject
]

{ #category : #'accessing - structure variables' }
AGPURegion3d >> z [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_Z
]

{ #category : #'accessing - structure variables' }
AGPURegion3d >> z: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_Z put: anObject
]
