Class {
	#name : #AGPUQuaternionf,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUQuaternionf class >> fields [
	"
	self defineFields
	"
    ^ #(
		(w 'float')
		(x 'float')
		(y 'float')
		(z 'float')
	)
]

