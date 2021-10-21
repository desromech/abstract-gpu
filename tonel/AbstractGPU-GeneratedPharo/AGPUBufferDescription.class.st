Class {
	#name : #AGPUBufferDescription,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_HEAP_TYPE',
		'OFFSET_MAIN_USAGE_MODE',
		'OFFSET_MAPPING_FLAGS',
		'OFFSET_SIZE',
		'OFFSET_STRIDE',
		'OFFSET_USAGE_MODES'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUBufferDescription class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint size;
		 agpu_memory_heap_type heap_type;
		 agpu_buffer_usage_mask usage_modes;
		 agpu_buffer_usage_mask main_usage_mode;
		 agpu_bitfield mapping_flags;
		 agpu_uint stride;
	)
]

{ #category : #'accessing - structure variables' }
AGPUBufferDescription >> heap_type [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_HEAP_TYPE
]

{ #category : #'accessing - structure variables' }
AGPUBufferDescription >> heap_type: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_HEAP_TYPE put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUBufferDescription >> main_usage_mode [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_MAIN_USAGE_MODE
]

{ #category : #'accessing - structure variables' }
AGPUBufferDescription >> main_usage_mode: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_MAIN_USAGE_MODE put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUBufferDescription >> mapping_flags [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_MAPPING_FLAGS
]

{ #category : #'accessing - structure variables' }
AGPUBufferDescription >> mapping_flags: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_MAPPING_FLAGS put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUBufferDescription >> size [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_SIZE
]

{ #category : #'accessing - structure variables' }
AGPUBufferDescription >> size: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_SIZE put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUBufferDescription >> stride [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_STRIDE
]

{ #category : #'accessing - structure variables' }
AGPUBufferDescription >> stride: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_STRIDE put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUBufferDescription >> usage_modes [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_USAGE_MODES
]

{ #category : #'accessing - structure variables' }
AGPUBufferDescription >> usage_modes: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_USAGE_MODES put: anObject
]
