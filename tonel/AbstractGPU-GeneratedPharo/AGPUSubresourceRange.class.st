Class {
	#name : #AGPUSubresourceRange,
	#superclass : #FFIExternalStructure,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUSubresourceRange class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_texture_usage_mode_mask usage_mode;
		 agpu_uint base_miplevel;
		 agpu_uint level_count;
		 agpu_uint base_arraylayer;
		 agpu_uint layer_count;
	)
]
