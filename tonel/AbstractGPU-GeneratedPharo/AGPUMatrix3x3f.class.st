Class {
	#name : #AGPUMatrix3x3f,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_C1',
		'OFFSET_C2',
		'OFFSET_C3'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUMatrix3x3f class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_vector3f c1;
		 agpu_vector3f c2;
		 agpu_vector3f c3;
	)
]

{ #category : #'accessing - structure variables' }
AGPUMatrix3x3f >> c1 [
	"This method was automatically generated"
	^ AGPUVector3f fromHandle: (handle referenceStructAt: OFFSET_C1 length: AGPUVector3f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUMatrix3x3f >> c1: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_C1 put: anObject getHandle length: AGPUVector3f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUMatrix3x3f >> c2 [
	"This method was automatically generated"
	^ AGPUVector3f fromHandle: (handle referenceStructAt: OFFSET_C2 length: AGPUVector3f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUMatrix3x3f >> c2: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_C2 put: anObject getHandle length: AGPUVector3f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUMatrix3x3f >> c3 [
	"This method was automatically generated"
	^ AGPUVector3f fromHandle: (handle referenceStructAt: OFFSET_C3 length: AGPUVector3f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUMatrix3x3f >> c3: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_C3 put: anObject getHandle length: AGPUVector3f byteSize
]
