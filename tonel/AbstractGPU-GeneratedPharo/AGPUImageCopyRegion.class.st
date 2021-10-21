Class {
	#name : #AGPUImageCopyRegion,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_DESTINATION_OFFSET',
		'OFFSET_DESTINATION_SUBRESOURCE_LEVEL',
		'OFFSET_DESTINATION_USAGE_MODE',
		'OFFSET_EXTENT',
		'OFFSET_SOURCE_OFFSET',
		'OFFSET_SOURCE_SUBRESOURCE_LEVEL',
		'OFFSET_SOURCE_USAGE_MODE'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
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

{ #category : #'accessing - structure variables' }
AGPUImageCopyRegion >> destination_offset [
	"This method was automatically generated"
	^ AGPUOffset3d fromHandle: (handle referenceStructAt: OFFSET_DESTINATION_OFFSET length: AGPUOffset3d byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImageCopyRegion >> destination_offset: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_DESTINATION_OFFSET put: anObject getHandle length: AGPUOffset3d byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImageCopyRegion >> destination_subresource_level [
	"This method was automatically generated"
	^ AGPUTextureSubresourceLevel fromHandle: (handle referenceStructAt: OFFSET_DESTINATION_SUBRESOURCE_LEVEL length: AGPUTextureSubresourceLevel byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImageCopyRegion >> destination_subresource_level: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_DESTINATION_SUBRESOURCE_LEVEL put: anObject getHandle length: AGPUTextureSubresourceLevel byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImageCopyRegion >> destination_usage_mode [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_DESTINATION_USAGE_MODE
]

{ #category : #'accessing - structure variables' }
AGPUImageCopyRegion >> destination_usage_mode: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_DESTINATION_USAGE_MODE put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUImageCopyRegion >> extent [
	"This method was automatically generated"
	^ AGPUSize3d fromHandle: (handle referenceStructAt: OFFSET_EXTENT length: AGPUSize3d byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImageCopyRegion >> extent: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_EXTENT put: anObject getHandle length: AGPUSize3d byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImageCopyRegion >> source_offset [
	"This method was automatically generated"
	^ AGPUOffset3d fromHandle: (handle referenceStructAt: OFFSET_SOURCE_OFFSET length: AGPUOffset3d byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImageCopyRegion >> source_offset: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_SOURCE_OFFSET put: anObject getHandle length: AGPUOffset3d byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImageCopyRegion >> source_subresource_level [
	"This method was automatically generated"
	^ AGPUTextureSubresourceLevel fromHandle: (handle referenceStructAt: OFFSET_SOURCE_SUBRESOURCE_LEVEL length: AGPUTextureSubresourceLevel byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImageCopyRegion >> source_subresource_level: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_SOURCE_SUBRESOURCE_LEVEL put: anObject getHandle length: AGPUTextureSubresourceLevel byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImageCopyRegion >> source_usage_mode [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_SOURCE_USAGE_MODE
]

{ #category : #'accessing - structure variables' }
AGPUImageCopyRegion >> source_usage_mode: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_SOURCE_USAGE_MODE put: anObject
]
