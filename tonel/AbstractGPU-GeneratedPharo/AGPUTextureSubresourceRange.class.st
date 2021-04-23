Class {
	#name : #AGPUTextureSubresourceRange,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUTextureSubresourceRange class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_texture_aspect aspect;
		 agpu_uint base_miplevel;
		 agpu_uint level_count;
		 agpu_uint base_arraylayer;
		 agpu_uint layer_count;
	)
]

