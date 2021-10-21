Class {
	#name : #AGPUVrDualAnalogEvent,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_TRANSFORMED_X',
		'OFFSET_TRANSFORMED_Y',
		'OFFSET_WHICH',
		'OFFSET_X',
		'OFFSET_Y'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUVrDualAnalogEvent class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_float x;
		 agpu_float y;
		 agpu_float transformed_x;
		 agpu_float transformed_y;
		 agpu_uint which;
	)
]

{ #category : #'accessing - structure variables' }
AGPUVrDualAnalogEvent >> transformed_x [
	"This method was automatically generated"
	^handle floatAt: OFFSET_TRANSFORMED_X
]

{ #category : #'accessing - structure variables' }
AGPUVrDualAnalogEvent >> transformed_x: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_TRANSFORMED_X put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVrDualAnalogEvent >> transformed_y [
	"This method was automatically generated"
	^handle floatAt: OFFSET_TRANSFORMED_Y
]

{ #category : #'accessing - structure variables' }
AGPUVrDualAnalogEvent >> transformed_y: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_TRANSFORMED_Y put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVrDualAnalogEvent >> which [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_WHICH
]

{ #category : #'accessing - structure variables' }
AGPUVrDualAnalogEvent >> which: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_WHICH put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVrDualAnalogEvent >> x [
	"This method was automatically generated"
	^handle floatAt: OFFSET_X
]

{ #category : #'accessing - structure variables' }
AGPUVrDualAnalogEvent >> x: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_X put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVrDualAnalogEvent >> y [
	"This method was automatically generated"
	^handle floatAt: OFFSET_Y
]

{ #category : #'accessing - structure variables' }
AGPUVrDualAnalogEvent >> y: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_Y put: anObject
]
