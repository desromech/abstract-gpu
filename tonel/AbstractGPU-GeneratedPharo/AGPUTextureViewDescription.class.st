Class {
	#name : #AGPUTextureViewDescription,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUTextureViewDescription class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_texture_type type;
		 agpu_texture_format format;
		 agpu_uint sample_count;
		 agpu_texture_usage_mode_mask usage_mode;
		 agpu_components_swizzle components;
		 agpu_texture_subresource_range subresource_range;
	)
]

