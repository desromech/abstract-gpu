Class {
	#name : #AGPUImmediateRendererMaterialMetallicRoughness,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUImmediateRendererMaterialMetallicRoughness class >> fields [
	"
	self defineFields
	"
    ^ #(
		(emission 'AGPUVector4f')
		(base_color 'AGPUVector4f')
		(metallic_factor 'float')
		(roughness_factor 'float')
	)
]

