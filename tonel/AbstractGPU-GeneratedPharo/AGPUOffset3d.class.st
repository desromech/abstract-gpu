Class {
	#name : #AGPUOffset3d,
	#superclass : #FFIExternalStructure,
	#classVars : [
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
AGPUOffset3d class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint x;
		 agpu_uint y;
		 agpu_uint z;
	)
]

{ #category : #'accessing - structure variables' }
AGPUOffset3d >> x [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_X
]

{ #category : #'accessing - structure variables' }
AGPUOffset3d >> x: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_X put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUOffset3d >> y [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_Y
]

{ #category : #'accessing - structure variables' }
AGPUOffset3d >> y: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_Y put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUOffset3d >> z [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_Z
]

{ #category : #'accessing - structure variables' }
AGPUOffset3d >> z: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_Z put: anObject
]
