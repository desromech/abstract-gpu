Class {
	#name : #AGPUImmediateRendererMaterialFlatColor,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUImmediateRendererMaterialFlatColor class >> fields [
	"
	self defineFields
	"
    ^ #(
		(color 'AGPUVector4f')
	)
]

