Class {
	#name : #AGPUSubresourceRange,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #AGPUExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUSubresourceRange class >> fields [
	"
	self defineFields
	"
    ^ #(
		(usage_mode 'long')
		(base_miplevel 'ulong')
		(level_count 'ulong')
		(base_arraylayer 'ulong')
		(layer_count 'ulong')
	)
]

