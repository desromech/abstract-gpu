Class {
	#name : #AGPUMatrix3x3f,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUMatrix3x3f class >> fields [
	"
	self defineFields
	"
    ^ #(
		(c1 'AGPUVector3f')
		(c2 'AGPUVector3f')
		(c3 'AGPUVector3f')
	)
]

