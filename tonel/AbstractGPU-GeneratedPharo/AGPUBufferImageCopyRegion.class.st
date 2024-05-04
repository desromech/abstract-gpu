Class {
	#name : #AGPUBufferImageCopyRegion,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUBufferImageCopyRegion class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_size buffer_offset;
		 agpu_size buffer_pitch;
		 agpu_size buffer_slice_pitch;
		 agpu_texture_usage_mode_mask texture_usage_mode;
		 agpu_texture_subresource_level texture_subresource_level;
		 agpu_region3d texture_region;
	)
]

