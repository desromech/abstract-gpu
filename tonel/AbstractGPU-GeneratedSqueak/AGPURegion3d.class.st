Class {
	#name : #AGPURegion3d,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPURegion3d class >> fields [
	"
	self defineFields
	"
    ^ #(
		(x 'ulong')
		(y 'ulong')
		(z 'ulong')
		(width 'ulong')
		(height 'ulong')
		(depth 'ulong')
	)
]

