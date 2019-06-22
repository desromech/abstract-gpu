Class {
	#name : #AGPUDepthStencilValue,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #AGPUExternalStructure,
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

