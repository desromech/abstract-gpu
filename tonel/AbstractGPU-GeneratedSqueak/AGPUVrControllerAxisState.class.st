Class {
	#name : #AGPUVrControllerAxisState,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUVrControllerAxisState class >> fields [
	"
	self defineFields
	"
    ^ #(
		(type 'long')
		(x 'float')
		(y 'float')
	)
]

