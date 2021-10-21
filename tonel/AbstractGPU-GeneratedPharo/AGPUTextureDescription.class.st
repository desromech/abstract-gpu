Class {
	#name : #AGPUTextureDescription,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_CLEAR_VALUE',
		'OFFSET_DEPTH',
		'OFFSET_FORMAT',
		'OFFSET_HEAP_TYPE',
		'OFFSET_HEIGHT',
		'OFFSET_LAYERS',
		'OFFSET_MAIN_USAGE_MODE',
		'OFFSET_MIPLEVELS',
		'OFFSET_SAMPLE_COUNT',
		'OFFSET_SAMPLE_QUALITY',
		'OFFSET_TYPE',
		'OFFSET_USAGE_MODES',
		'OFFSET_WIDTH'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUTextureDescription class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_texture_type type;
		 agpu_uint width;
		 agpu_uint height;
		 agpu_uint depth;
		 agpu_uint layers;
		 agpu_ushort miplevels;
		 agpu_texture_format format;
		 agpu_texture_usage_mode_mask usage_modes;
		 agpu_texture_usage_mode_mask main_usage_mode;
		 agpu_memory_heap_type heap_type;
		 agpu_uint sample_count;
		 agpu_uint sample_quality;
		 agpu_texture_clear_value clear_value;
	)
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> clear_value [
	"This method was automatically generated"
	^ AGPUTextureClearValue fromHandle: (handle referenceStructAt: OFFSET_CLEAR_VALUE length: AGPUTextureClearValue byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> clear_value: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_CLEAR_VALUE put: anObject getHandle length: AGPUTextureClearValue byteSize
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> depth [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_DEPTH
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> depth: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_DEPTH put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> format [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_FORMAT
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> format: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_FORMAT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> heap_type [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_HEAP_TYPE
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> heap_type: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_HEAP_TYPE put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> height [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_HEIGHT
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> height: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_HEIGHT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> layers [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_LAYERS
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> layers: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_LAYERS put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> main_usage_mode [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_MAIN_USAGE_MODE
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> main_usage_mode: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_MAIN_USAGE_MODE put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> miplevels [
	"This method was automatically generated"
	^handle unsignedShortAt: OFFSET_MIPLEVELS
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> miplevels: anObject [
	"This method was automatically generated"
	handle unsignedShortAt: OFFSET_MIPLEVELS put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> sample_count [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_SAMPLE_COUNT
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> sample_count: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_SAMPLE_COUNT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> sample_quality [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_SAMPLE_QUALITY
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> sample_quality: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_SAMPLE_QUALITY put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> type [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_TYPE
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> type: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_TYPE put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> usage_modes [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_USAGE_MODES
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> usage_modes: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_USAGE_MODES put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> width [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_WIDTH
]

{ #category : #'accessing - structure variables' }
AGPUTextureDescription >> width: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_WIDTH put: anObject
]
