Class {
	#name : #AGPUVrRenderModelTexture,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUVrRenderModelTexture class >> fields [
	"
	self defineFields
	"
    ^ #(
		(width 'ushort')
		(height 'ushort')
		(pitch 'ulong')
		(data 'byte*')
	)
]

