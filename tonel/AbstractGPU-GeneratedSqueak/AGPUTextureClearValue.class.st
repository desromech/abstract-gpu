Class {
	#name : #AGPUTextureClearValue,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalUnion,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUTextureClearValue class >> fields [
	"
	self defineFields
	"
    ^ #(
		(color 'AGPUColor4f')
		(depth_stencil 'AGPUDepthStencilValue')
	)
]

