Class {
	#name : #AGPUVertexAttribDescription,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUVertexAttribDescription class >> fields [
	"
	self defineFields
	"
    ^ #(
		(buffer 'ulong')
		(binding 'ulong')
		(format 'long')
		(rows 'ulong')
		(offset 'ulong')
		(divisor 'ulong')
	)
]

