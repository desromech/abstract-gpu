Class {
	#name : #AGPUSize3d,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUSize3d class >> fields [
	"
	self defineFields
	"
    ^ #(
		(width 'ulong')
		(height 'ulong')
		(depth 'ulong')
	)
]

