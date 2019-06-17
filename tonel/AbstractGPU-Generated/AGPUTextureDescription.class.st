Class {
	#name : #AGPUTextureDescription,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'definition' }
AGPUTextureDescription class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
	^ #(
		 agpu_texture_type type;
		 agpu_uint width;
		 agpu_uint height;
		 agpu_uint depth;
		 agpu_uint layers;
		 agpu_ushort miplevels;
		 agpu_texture_format format;
		 agpu_texture_usage_mode_mask usage_modes;
		 agpu_texture_usage_mode_mask main_usage_mode;
		 agpu_memory_heap_type heap_type;
		 agpu_uint sample_count;
		 agpu_uint sample_quality;
		)
]

