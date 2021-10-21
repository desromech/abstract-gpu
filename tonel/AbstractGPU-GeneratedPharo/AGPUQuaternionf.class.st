Class {
	#name : #AGPUQuaternionf,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_W',
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
AGPUQuaternionf class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_float w;
		 agpu_float x;
		 agpu_float y;
		 agpu_float z;
	)
]

{ #category : #'accessing - structure variables' }
AGPUQuaternionf >> w [
	"This method was automatically generated"
	^handle floatAt: OFFSET_W
]

{ #category : #'accessing - structure variables' }
AGPUQuaternionf >> w: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_W put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUQuaternionf >> x [
	"This method was automatically generated"
	^handle floatAt: OFFSET_X
]

{ #category : #'accessing - structure variables' }
AGPUQuaternionf >> x: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_X put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUQuaternionf >> y [
	"This method was automatically generated"
	^handle floatAt: OFFSET_Y
]

{ #category : #'accessing - structure variables' }
AGPUQuaternionf >> y: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_Y put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUQuaternionf >> z [
	"This method was automatically generated"
	^handle floatAt: OFFSET_Z
]

{ #category : #'accessing - structure variables' }
AGPUQuaternionf >> z: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_Z put: anObject
]
