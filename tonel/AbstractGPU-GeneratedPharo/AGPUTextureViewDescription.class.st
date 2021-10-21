Class {
	#name : #AGPUTextureViewDescription,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_COMPONENTS',
		'OFFSET_FORMAT',
		'OFFSET_SAMPLE_COUNT',
		'OFFSET_SUBRESOURCE_RANGE',
		'OFFSET_TYPE',
		'OFFSET_USAGE_MODE'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUTextureViewDescription class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_texture_type type;
		 agpu_texture_format format;
		 agpu_uint sample_count;
		 agpu_texture_usage_mode_mask usage_mode;
		 agpu_components_swizzle components;
		 agpu_texture_subresource_range subresource_range;
	)
]

{ #category : #'accessing - structure variables' }
AGPUTextureViewDescription >> components [
	"This method was automatically generated"
	^ AGPUComponentsSwizzle fromHandle: (handle referenceStructAt: OFFSET_COMPONENTS length: AGPUComponentsSwizzle byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUTextureViewDescription >> components: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_COMPONENTS put: anObject getHandle length: AGPUComponentsSwizzle byteSize
]

{ #category : #'accessing - structure variables' }
AGPUTextureViewDescription >> format [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_FORMAT
]

{ #category : #'accessing - structure variables' }
AGPUTextureViewDescription >> format: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_FORMAT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureViewDescription >> sample_count [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_SAMPLE_COUNT
]

{ #category : #'accessing - structure variables' }
AGPUTextureViewDescription >> sample_count: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_SAMPLE_COUNT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureViewDescription >> subresource_range [
	"This method was automatically generated"
	^ AGPUTextureSubresourceRange fromHandle: (handle referenceStructAt: OFFSET_SUBRESOURCE_RANGE length: AGPUTextureSubresourceRange byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUTextureViewDescription >> subresource_range: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_SUBRESOURCE_RANGE put: anObject getHandle length: AGPUTextureSubresourceRange byteSize
]

{ #category : #'accessing - structure variables' }
AGPUTextureViewDescription >> type [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_TYPE
]

{ #category : #'accessing - structure variables' }
AGPUTextureViewDescription >> type: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_TYPE put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureViewDescription >> usage_mode [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_USAGE_MODE
]

{ #category : #'accessing - structure variables' }
AGPUTextureViewDescription >> usage_mode: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_USAGE_MODE put: anObject
]
