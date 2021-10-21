Class {
	#name : #AGPUVrEventData,
	#superclass : #FFIExternalUnion,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUVrEventData class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint type;
		 agpu_vr_generic_event generic;
		 agpu_vr_controller_event controller;
		 agpu_vr_dual_analog_event dual_analog;
	)
]

{ #category : #'accessing - structure variables' }
AGPUVrEventData >> controller [
	"This method was automatically generated"
	^ AGPUVrControllerEvent fromHandle: (handle referenceStructAt: 1 length: AGPUVrControllerEvent byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUVrEventData >> controller: anObject [
	"This method was automatically generated"
	handle structAt: 1 put: anObject getHandle length: 4.
]

{ #category : #'accessing - structure variables' }
AGPUVrEventData >> dual_analog [
	"This method was automatically generated"
	^ AGPUVrDualAnalogEvent fromHandle: (handle referenceStructAt: 1 length: AGPUVrDualAnalogEvent byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUVrEventData >> dual_analog: anObject [
	"This method was automatically generated"
	handle structAt: 1 put: anObject getHandle length: 20.
]

{ #category : #'accessing - structure variables' }
AGPUVrEventData >> generic [
	"This method was automatically generated"
	^ AGPUVrGenericEvent fromHandle: (handle referenceStructAt: 1 length: AGPUVrGenericEvent byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUVrEventData >> generic: anObject [
	"This method was automatically generated"
	handle structAt: 1 put: anObject getHandle length: 32.
]

{ #category : #'accessing - structure variables' }
AGPUVrEventData >> type [
	"This method was automatically generated"
	^handle unsignedLongAt: 1
]

{ #category : #'accessing - structure variables' }
AGPUVrEventData >> type: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: 1 put: anObject
]
