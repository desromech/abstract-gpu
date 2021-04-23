Class {
	#name : #AGPUTextureSubresourceLevel,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUTextureSubresourceLevel class >> fields [
	"
	self defineFields
	"
    ^ #(
		(aspect 'long')
		(miplevel 'ulong')
		(base_arraylayer 'ulong')
		(layer_count 'ulong')
	)
]

