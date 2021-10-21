Class {
	#name : #AGPUBufferImageCopyRegion,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_BUFFER_OFFSET',
		'OFFSET_BUFFER_PITCH',
		'OFFSET_BUFFER_SLICE_PITCH',
		'OFFSET_TEXTURE_REGION',
		'OFFSET_TEXTURE_SUBRESOURCE_LEVEL',
		'OFFSET_TEXTURE_USAGE_MODE'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
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

{ #category : #'accessing - structure variables' }
AGPUBufferImageCopyRegion >> buffer_offset [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_BUFFER_OFFSET
]

{ #category : #'accessing - structure variables' }
AGPUBufferImageCopyRegion >> buffer_offset: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_BUFFER_OFFSET put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUBufferImageCopyRegion >> buffer_pitch [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_BUFFER_PITCH
]

{ #category : #'accessing - structure variables' }
AGPUBufferImageCopyRegion >> buffer_pitch: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_BUFFER_PITCH put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUBufferImageCopyRegion >> buffer_slice_pitch [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_BUFFER_SLICE_PITCH
]

{ #category : #'accessing - structure variables' }
AGPUBufferImageCopyRegion >> buffer_slice_pitch: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_BUFFER_SLICE_PITCH put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUBufferImageCopyRegion >> texture_region [
	"This method was automatically generated"
	^ AGPURegion3d fromHandle: (handle referenceStructAt: OFFSET_TEXTURE_REGION length: AGPURegion3d byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUBufferImageCopyRegion >> texture_region: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_TEXTURE_REGION put: anObject getHandle length: AGPURegion3d byteSize
]

{ #category : #'accessing - structure variables' }
AGPUBufferImageCopyRegion >> texture_subresource_level [
	"This method was automatically generated"
	^ AGPUTextureSubresourceLevel fromHandle: (handle referenceStructAt: OFFSET_TEXTURE_SUBRESOURCE_LEVEL length: AGPUTextureSubresourceLevel byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUBufferImageCopyRegion >> texture_subresource_level: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_TEXTURE_SUBRESOURCE_LEVEL put: anObject getHandle length: AGPUTextureSubresourceLevel byteSize
]

{ #category : #'accessing - structure variables' }
AGPUBufferImageCopyRegion >> texture_usage_mode [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_TEXTURE_USAGE_MODE
]

{ #category : #'accessing - structure variables' }
AGPUBufferImageCopyRegion >> texture_usage_mode: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_TEXTURE_USAGE_MODE put: anObject
]
