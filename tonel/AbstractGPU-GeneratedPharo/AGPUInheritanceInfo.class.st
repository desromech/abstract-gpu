Class {
	#name : #AGPUInheritanceInfo,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_FLAT',
		'OFFSET_RENDERPASS'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUInheritanceInfo class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_int flat;
		 agpu_renderpass* renderpass;
	)
]

{ #category : #'accessing - structure variables' }
AGPUInheritanceInfo >> flat [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_FLAT
]

{ #category : #'accessing - structure variables' }
AGPUInheritanceInfo >> flat: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_FLAT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUInheritanceInfo >> renderpass [
	"This method was automatically generated"
	^ExternalData fromHandle: (handle pointerAt: OFFSET_RENDERPASS) type: ExternalType void asPointerType
]

{ #category : #'accessing - structure variables' }
AGPUInheritanceInfo >> renderpass: anObject [
	"This method was automatically generated"
	handle pointerAt: OFFSET_RENDERPASS put: anObject getHandle.
]
