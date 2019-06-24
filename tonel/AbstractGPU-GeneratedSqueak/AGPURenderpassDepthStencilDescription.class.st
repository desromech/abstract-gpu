Class {
	#name : #AGPURenderpassDepthStencilDescription,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #AGPUExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPURenderpassDepthStencilDescription class >> fields [
	"
	self defineFields
	"
    ^ #(
		(format 'long')
		(sample_count 'ulong')
		(begin_action 'long')
		(end_action 'long')
		(stencil_begin_action 'long')
		(stencil_end_action 'long')
		(clear_value 'AGPUDepthStencilValue')
	)
]

