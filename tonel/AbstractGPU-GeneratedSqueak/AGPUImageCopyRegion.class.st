Class {
	#name : #AGPUImageCopyRegion,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUImageCopyRegion class >> fields [
	"
	self defineFields
	"
    ^ #(
		(source_usage_mode 'long')
		(source_subresource_level 'AGPUTextureSubresourceLevel')
		(source_offset 'AGPUOffset3d')
		(destination_usage_mode 'long')
		(destination_subresource_level 'AGPUTextureSubresourceLevel')
		(destination_offset 'AGPUOffset3d')
		(extent 'AGPUSize3d')
	)
]

