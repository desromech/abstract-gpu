Class {
	#name : #AGPUVrEvent,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUVrEvent class >> fields [
	"
	self defineFields
	"
    ^ #(
		(type 'ulong')
		(tracked_device_index 'ulong')
		(event_age_seconds 'float')
		(data 'AGPUVrEventData')
	)
]

