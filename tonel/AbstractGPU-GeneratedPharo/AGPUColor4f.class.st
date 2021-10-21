Class {
	#name : #AGPUColor4f,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_A',
		'OFFSET_B',
		'OFFSET_G',
		'OFFSET_R'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUColor4f class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_float r;
		 agpu_float g;
		 agpu_float b;
		 agpu_float a;
	)
]

{ #category : #'accessing - structure variables' }
AGPUColor4f >> a [
	"This method was automatically generated"
	^handle floatAt: OFFSET_A
]

{ #category : #'accessing - structure variables' }
AGPUColor4f >> a: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_A put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUColor4f >> b [
	"This method was automatically generated"
	^handle floatAt: OFFSET_B
]

{ #category : #'accessing - structure variables' }
AGPUColor4f >> b: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_B put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUColor4f >> g [
	"This method was automatically generated"
	^handle floatAt: OFFSET_G
]

{ #category : #'accessing - structure variables' }
AGPUColor4f >> g: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_G put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUColor4f >> r [
	"This method was automatically generated"
	^handle floatAt: OFFSET_R
]

{ #category : #'accessing - structure variables' }
AGPUColor4f >> r: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_R put: anObject
]
