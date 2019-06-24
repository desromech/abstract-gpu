Class {
	#name : #AGPUFrustumTangents,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUFrustumTangents class >> fields [
	"
	self defineFields
	"
    ^ #(
		(left 'float')
		(right 'float')
		(top 'float')
		(bottom 'float')
	)
]

