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
		 agpu_ushort depthOrArraySize;
		 agpu_ushort miplevels;
		 agpu_texture_format format;
		 agpu_texture_flags flags;
		 agpu_uint sample_count;
		 agpu_uint sample_quality;
		 agpu_buffer* upload_buffer;
		 agpu_buffer* readback_buffer;
		)
]

