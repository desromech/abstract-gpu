Class {
	#name : #AGPUComponentsSwizzle,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUComponentsSwizzle class >> fields [
	"
	self defineFields
	"
    ^ #(
		(r 'long')
		(g 'long')
		(b 'long')
		(a 'long')
	)
]

