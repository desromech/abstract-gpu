Class {
	#name : #AGPUDepthStencilValue,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'definition' }
AGPUDepthStencilValue class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
	^ #(
		 agpu_float depth;
		 agpu_byte stencil;
		)
]

