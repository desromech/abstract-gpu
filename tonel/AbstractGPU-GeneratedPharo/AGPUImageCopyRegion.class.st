Class {
	#name : #AGPUImageCopyRegion,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUImageCopyRegion class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_texture_usage_mode_mask source_usage_mode;
		 agpu_texture_subresource_level source_subresource_level;
		 agpu_offset3d source_offset;
		 agpu_texture_usage_mode_mask destination_usage_mode;
		 agpu_texture_subresource_level destination_subresource_level;
		 agpu_offset3d destination_offset;
		 agpu_size3d extent;
	)
]

