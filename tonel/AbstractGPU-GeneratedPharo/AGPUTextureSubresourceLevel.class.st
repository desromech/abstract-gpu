Class {
	#name : #AGPUTextureSubresourceLevel,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUTextureSubresourceLevel class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_texture_aspect aspect;
		 agpu_uint miplevel;
		 agpu_uint base_arraylayer;
		 agpu_uint layer_count;
	)
]

