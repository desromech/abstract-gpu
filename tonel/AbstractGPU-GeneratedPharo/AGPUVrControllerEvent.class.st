Class {
	#name : #AGPUVrControllerEvent,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_BUTTON'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUVrControllerEvent class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint button;
	)
]

{ #category : #'accessing - structure variables' }
AGPUVrControllerEvent >> button [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_BUTTON
]

{ #category : #'accessing - structure variables' }
AGPUVrControllerEvent >> button: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_BUTTON put: anObject
]
