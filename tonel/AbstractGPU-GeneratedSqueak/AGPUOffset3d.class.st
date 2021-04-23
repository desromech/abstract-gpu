Class {
	#name : #AGPUOffset3d,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUOffset3d class >> fields [
	"
	self defineFields
	"
    ^ #(
		(x 'ulong')
		(y 'ulong')
		(z 'ulong')
	)
]

