Class {
	#name : #AGPUVrControllerEvent,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUVrControllerEvent class >> fields [
	"
	self defineFields
	"
    ^ #(
		(button 'ulong')
	)
]

