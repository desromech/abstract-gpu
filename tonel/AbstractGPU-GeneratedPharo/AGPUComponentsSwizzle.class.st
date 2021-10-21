Class {
	#name : #AGPUComponentsSwizzle,
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
AGPUComponentsSwizzle class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_component_swizzle r;
		 agpu_component_swizzle g;
		 agpu_component_swizzle b;
		 agpu_component_swizzle a;
	)
]

{ #category : #'accessing - structure variables' }
AGPUComponentsSwizzle >> a [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_A
]

{ #category : #'accessing - structure variables' }
AGPUComponentsSwizzle >> a: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_A put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUComponentsSwizzle >> b [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_B
]

{ #category : #'accessing - structure variables' }
AGPUComponentsSwizzle >> b: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_B put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUComponentsSwizzle >> g [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_G
]

{ #category : #'accessing - structure variables' }
AGPUComponentsSwizzle >> g: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_G put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUComponentsSwizzle >> r [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_R
]

{ #category : #'accessing - structure variables' }
AGPUComponentsSwizzle >> r: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_R put: anObject
]
