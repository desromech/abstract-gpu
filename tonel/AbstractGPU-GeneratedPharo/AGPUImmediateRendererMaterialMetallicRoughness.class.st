Class {
	#name : #AGPUImmediateRendererMaterialMetallicRoughness,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUImmediateRendererMaterialMetallicRoughness class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_vector4f emission;
		 agpu_vector4f base_color;
		 agpu_float metallic_factor;
		 agpu_float roughness_factor;
		 agpu_float occlusion_factor;
		 agpu_float alpha_cutoff;
	)
]

