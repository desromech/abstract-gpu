Class {
	#name : #AGPUComponentsSwizzle,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #AGPUExternalStructure,
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

