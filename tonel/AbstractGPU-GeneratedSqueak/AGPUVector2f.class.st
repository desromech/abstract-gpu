Class {
	#name : #AGPUVector2f,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUVector2f class >> fields [
	"
	self defineFields
	"
    ^ #(
		(x 'float')
		(y 'float')
	)
]

