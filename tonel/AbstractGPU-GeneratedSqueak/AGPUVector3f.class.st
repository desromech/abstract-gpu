Class {
	#name : #AGPUVector3f,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #AGPUExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUVector3f class >> fields [
	"
	self defineFields
	"
    ^ #(
		(x 'float')
		(y 'float')
		(z 'float')
	)
]

