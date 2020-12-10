Class {
	#name : #AGPUTextureClearValue,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalUnion,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUTextureClearValue class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_color4f color;
		 agpu_depth_stencil_value depth_stencil;
	)
]

