Class {
	#name : #AGPUBufferImageCopyRegion,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUBufferImageCopyRegion class >> fields [
	"
	self defineFields
	"
    ^ #(
		(buffer_offset 'ulong')
		(buffer_row_length 'ulong')
		(buffer_image_height 'ulong')
		(texture_subresource_range 'AGPUSubresourceRange')
		(texture_region 'AGPURegion3d')
	)
]

