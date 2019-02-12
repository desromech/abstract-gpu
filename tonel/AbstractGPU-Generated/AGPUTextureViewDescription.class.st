Class {
	#name : #AGPUTextureViewDescription,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'definition' }
AGPUTextureViewDescription class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
	^ #(
		 agpu_texture_type type;
		 agpu_texture* texture;
		 agpu_texture_format format;
		 agpu_uint sample_count;
		 agpu_components_swizzle components;
		 agpu_subresource_range subresource_range;
		)
]

