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
		(buttons_pressed 'ulonglong longlong')
		(buttons_touched 'ulonglong longlong')
		(axis0 'AGPUVrControllerAxisState')
		(axis1 'AGPUVrControllerAxisState')
		(axis2 'AGPUVrControllerAxisState')
		(axis3 'AGPUVrControllerAxisState')
		(axis4 'AGPUVrControllerAxisState')
	)
]

