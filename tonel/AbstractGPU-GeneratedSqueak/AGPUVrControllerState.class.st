Class {
	#name : #AGPUVrControllerState,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUVrControllerState class >> fields [
	"
	self defineFields
	"
    ^ #(
		(buttons_pressed 'ulong')
		(buttons_touched 'ulong')
		(axis0 'AGPUVrControllerAxisState')
		(axis1 'AGPUVrControllerAxisState')
		(axis2 'AGPUVrControllerAxisState')
		(axis3 'AGPUVrControllerAxisState')
		(axis4 'AGPUVrControllerAxisState')
	)
]

