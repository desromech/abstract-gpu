Class {
	#name : #AGPUVrDualAnalogEvent,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #AGPUExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUVrDualAnalogEvent class >> fields [
	"
	self defineFields
	"
    ^ #(
		(x 'float')
		(y 'float')
		(transformed_x 'float')
		(transformed_y 'float')
		(which 'ulong')
	)
]

