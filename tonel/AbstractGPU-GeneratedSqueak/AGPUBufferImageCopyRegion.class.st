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
		(buffer_pitch 'ulong')
		(buffer_slice_pitch 'ulong')
		(texture_usage_mode 'long')
		(texture_subresource_level 'AGPUTextureSubresourceLevel')
		(texture_region 'AGPURegion3d')
	)
]

