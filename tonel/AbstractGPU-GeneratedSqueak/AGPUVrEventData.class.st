Class {
	#name : #AGPUVrEventData,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalUnion,
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

