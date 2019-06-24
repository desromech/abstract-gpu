Class {
	#name : #AGPUBufferDescription,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUBufferDescription class >> fields [
	"
	self defineFields
	"
    ^ #(
		(size 'ulong')
		(heap_type 'long')
		(binding 'long')
		(mapping_flags 'ulong')
		(stride 'ulong')
	)
]

