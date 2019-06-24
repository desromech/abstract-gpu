Class {
	#name : #AGPUMatrix4x4f,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #AGPUExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUMatrix4x4f class >> fields [
	"
	self defineFields
	"
    ^ #(
		(c1 'AGPUVector4f')
		(c2 'AGPUVector4f')
		(c3 'AGPUVector4f')
		(c4 'AGPUVector4f')
	)
]

