Class {
	#name : #AGPUImmediateRendererMaterial,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalUnion,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUImmediateRendererMaterial class >> fields [
	"
	self defineFields
	"
    ^ #(
		(classic 'AGPUImmediateRendererMaterialClassic')
		(metallic_roughness 'AGPUImmediateRendererMaterialMetallicRoughness')
		(flat_color 'AGPUImmediateRendererMaterialFlatColor')
	)
]

