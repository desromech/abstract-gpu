Class {
	#name : #AGPUImmediateRendererMaterial,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalUnion,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUImmediateRendererMaterial class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_immediate_renderer_material_classic classic;
		 agpu_immediate_renderer_material_metallic_roughness metallic_roughness;
	)
]

