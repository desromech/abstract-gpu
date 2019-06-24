Class {
	#name : #AGPUDepthStencilValue,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUDepthStencilValue class >> fields [
	"
	self defineFields
	"
    ^ #(
		(depth 'float')
		(stencil 'byte')
	)
]

