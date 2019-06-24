Class {
	#name : #AGPUVector4f,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUVector4f class >> fields [
	"
	self defineFields
	"
    ^ #(
		(x 'float')
		(y 'float')
		(z 'float')
		(w 'float')
	)
]

