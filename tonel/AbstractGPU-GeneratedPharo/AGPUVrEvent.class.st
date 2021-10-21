Class {
	#name : #AGPUVrEvent,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_DATA',
		'OFFSET_EVENT_AGE_SECONDS',
		'OFFSET_TRACKED_DEVICE_INDEX',
		'OFFSET_TYPE'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUVrEvent class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint type;
		 agpu_uint tracked_device_index;
		 agpu_float event_age_seconds;
		 agpu_vr_event_data data;
	)
]

{ #category : #'accessing - structure variables' }
AGPUVrEvent >> data [
	"This method was automatically generated"
	^ AGPUVrEventData fromHandle: (handle referenceStructAt: OFFSET_DATA length: AGPUVrEventData byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUVrEvent >> data: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_DATA put: anObject getHandle length: AGPUVrEventData byteSize
]

{ #category : #'accessing - structure variables' }
AGPUVrEvent >> event_age_seconds [
	"This method was automatically generated"
	^handle floatAt: OFFSET_EVENT_AGE_SECONDS
]

{ #category : #'accessing - structure variables' }
AGPUVrEvent >> event_age_seconds: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_EVENT_AGE_SECONDS put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVrEvent >> tracked_device_index [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_TRACKED_DEVICE_INDEX
]

{ #category : #'accessing - structure variables' }
AGPUVrEvent >> tracked_device_index: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_TRACKED_DEVICE_INDEX put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVrEvent >> type [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_TYPE
]

{ #category : #'accessing - structure variables' }
AGPUVrEvent >> type: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_TYPE put: anObject
]
