Class {
	#name : #AGPUVrEventData,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #AGPUExternalUnion,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUVrEventData class >> fields [
	"
	self defineFields
	"
    ^ #(
		(type 'ulong')
		(generic 'AGPUVrGenericEvent')
		(controller 'AGPUVrControllerEvent')
		(dual_analog 'AGPUVrDualAnalogEvent')
	)
]

