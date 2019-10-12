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
		 agpu_size buffer_row_length;
		 agpu_size buffer_image_height;
		 agpu_subresource_range texture_subresource_range;
		 agpu_region3d texture_region;
	)
]

