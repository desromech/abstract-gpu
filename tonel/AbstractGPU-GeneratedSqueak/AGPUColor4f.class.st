Class {
	#name : #AGPUColor4f,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUColor4f class >> fields [
	"
	self defineFields
	"
    ^ #(
		(r 'float')
		(g 'float')
		(b 'float')
		(a 'float')
	)
]

