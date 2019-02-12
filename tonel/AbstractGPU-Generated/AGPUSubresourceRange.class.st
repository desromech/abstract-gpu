Class {
	#name : #AGPUSubresourceRange,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'definition' }
AGPUSubresourceRange class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
	^ #(
		 agpu_texture_flags usage_flags;
		 agpu_uint base_miplevel;
		 agpu_uint level_count;
		 agpu_uint base_arraylayer;
		 agpu_uint layer_count;
		)
]

