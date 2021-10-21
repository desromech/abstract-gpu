Class {
	#name : #AGPUFrustumTangents,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_BOTTOM',
		'OFFSET_LEFT',
		'OFFSET_RIGHT',
		'OFFSET_TOP'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUFrustumTangents class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_float left;
		 agpu_float right;
		 agpu_float top;
		 agpu_float bottom;
	)
]

{ #category : #'accessing - structure variables' }
AGPUFrustumTangents >> bottom [
	"This method was automatically generated"
	^handle floatAt: OFFSET_BOTTOM
]

{ #category : #'accessing - structure variables' }
AGPUFrustumTangents >> bottom: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_BOTTOM put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUFrustumTangents >> left [
	"This method was automatically generated"
	^handle floatAt: OFFSET_LEFT
]

{ #category : #'accessing - structure variables' }
AGPUFrustumTangents >> left: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_LEFT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUFrustumTangents >> right [
	"This method was automatically generated"
	^handle floatAt: OFFSET_RIGHT
]

{ #category : #'accessing - structure variables' }
AGPUFrustumTangents >> right: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_RIGHT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUFrustumTangents >> top [
	"This method was automatically generated"
	^handle floatAt: OFFSET_TOP
]

{ #category : #'accessing - structure variables' }
AGPUFrustumTangents >> top: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_TOP put: anObject
]
