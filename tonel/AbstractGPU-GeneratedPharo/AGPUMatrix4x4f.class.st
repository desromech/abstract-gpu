Class {
	#name : #AGPUMatrix4x4f,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_C1',
		'OFFSET_C2',
		'OFFSET_C3',
		'OFFSET_C4'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUMatrix4x4f class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_vector4f c1;
		 agpu_vector4f c2;
		 agpu_vector4f c3;
		 agpu_vector4f c4;
	)
]

{ #category : #'accessing - structure variables' }
AGPUMatrix4x4f >> c1 [
	"This method was automatically generated"
	^ AGPUVector4f fromHandle: (handle referenceStructAt: OFFSET_C1 length: AGPUVector4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUMatrix4x4f >> c1: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_C1 put: anObject getHandle length: AGPUVector4f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUMatrix4x4f >> c2 [
	"This method was automatically generated"
	^ AGPUVector4f fromHandle: (handle referenceStructAt: OFFSET_C2 length: AGPUVector4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUMatrix4x4f >> c2: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_C2 put: anObject getHandle length: AGPUVector4f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUMatrix4x4f >> c3 [
	"This method was automatically generated"
	^ AGPUVector4f fromHandle: (handle referenceStructAt: OFFSET_C3 length: AGPUVector4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUMatrix4x4f >> c3: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_C3 put: anObject getHandle length: AGPUVector4f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUMatrix4x4f >> c4 [
	"This method was automatically generated"
	^ AGPUVector4f fromHandle: (handle referenceStructAt: OFFSET_C4 length: AGPUVector4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUMatrix4x4f >> c4: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_C4 put: anObject getHandle length: AGPUVector4f byteSize
]
