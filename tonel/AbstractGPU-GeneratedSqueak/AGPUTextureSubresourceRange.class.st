Class {
	#name : #AGPUTextureSubresourceRange,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUTextureSubresourceRange class >> fields [
	"
	self defineFields
	"
    ^ #(
		(aspect 'long')
		(base_miplevel 'ulong')
		(level_count 'ulong')
		(base_arraylayer 'ulong')
		(layer_count 'ulong')
	)
]

