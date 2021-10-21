Class {
	#name : #AGPUVrGenericEvent,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_WORD1',
		'OFFSET_WORD2',
		'OFFSET_WORD3',
		'OFFSET_WORD4',
		'OFFSET_WORD5',
		'OFFSET_WORD6',
		'OFFSET_WORD7',
		'OFFSET_WORD8'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUVrGenericEvent class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint word1;
		 agpu_uint word2;
		 agpu_uint word3;
		 agpu_uint word4;
		 agpu_uint word5;
		 agpu_uint word6;
		 agpu_uint word7;
		 agpu_uint word8;
	)
]

{ #category : #'accessing - structure variables' }
AGPUVrGenericEvent >> word1 [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_WORD1
]

{ #category : #'accessing - structure variables' }
AGPUVrGenericEvent >> word1: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_WORD1 put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVrGenericEvent >> word2 [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_WORD2
]

{ #category : #'accessing - structure variables' }
AGPUVrGenericEvent >> word2: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_WORD2 put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVrGenericEvent >> word3 [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_WORD3
]

{ #category : #'accessing - structure variables' }
AGPUVrGenericEvent >> word3: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_WORD3 put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVrGenericEvent >> word4 [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_WORD4
]

{ #category : #'accessing - structure variables' }
AGPUVrGenericEvent >> word4: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_WORD4 put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVrGenericEvent >> word5 [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_WORD5
]

{ #category : #'accessing - structure variables' }
AGPUVrGenericEvent >> word5: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_WORD5 put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVrGenericEvent >> word6 [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_WORD6
]

{ #category : #'accessing - structure variables' }
AGPUVrGenericEvent >> word6: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_WORD6 put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVrGenericEvent >> word7 [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_WORD7
]

{ #category : #'accessing - structure variables' }
AGPUVrGenericEvent >> word7: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_WORD7 put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVrGenericEvent >> word8 [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_WORD8
]

{ #category : #'accessing - structure variables' }
AGPUVrGenericEvent >> word8: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_WORD8 put: anObject
]
