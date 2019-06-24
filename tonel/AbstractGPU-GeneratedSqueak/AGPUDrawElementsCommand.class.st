Class {
	#name : #AGPUDrawElementsCommand,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUDrawElementsCommand class >> fields [
	"
	self defineFields
	"
    ^ #(
		(index_count 'ulong')
		(instance_count 'ulong')
		(first_index 'ulong')
		(base_vertex 'long')
		(base_instance 'ulong')
	)
]

