Class {
	#name : #AGPURenderpassDepthStencilDescription,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_BEGIN_ACTION',
		'OFFSET_CLEAR_VALUE',
		'OFFSET_END_ACTION',
		'OFFSET_FORMAT',
		'OFFSET_SAMPLE_COUNT',
		'OFFSET_SAMPLE_QUALITY',
		'OFFSET_STENCIL_BEGIN_ACTION',
		'OFFSET_STENCIL_END_ACTION'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPURenderpassDepthStencilDescription class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_texture_format format;
		 agpu_uint sample_count;
		 agpu_uint sample_quality;
		 agpu_renderpass_attachment_action begin_action;
		 agpu_renderpass_attachment_action end_action;
		 agpu_renderpass_attachment_action stencil_begin_action;
		 agpu_renderpass_attachment_action stencil_end_action;
		 agpu_depth_stencil_value clear_value;
	)
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDepthStencilDescription >> begin_action [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_BEGIN_ACTION
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDepthStencilDescription >> begin_action: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_BEGIN_ACTION put: anObject
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDepthStencilDescription >> clear_value [
	"This method was automatically generated"
	^ AGPUDepthStencilValue fromHandle: (handle referenceStructAt: OFFSET_CLEAR_VALUE length: AGPUDepthStencilValue byteSize)
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDepthStencilDescription >> clear_value: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_CLEAR_VALUE put: anObject getHandle length: AGPUDepthStencilValue byteSize
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDepthStencilDescription >> end_action [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_END_ACTION
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDepthStencilDescription >> end_action: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_END_ACTION put: anObject
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDepthStencilDescription >> format [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_FORMAT
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDepthStencilDescription >> format: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_FORMAT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDepthStencilDescription >> sample_count [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_SAMPLE_COUNT
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDepthStencilDescription >> sample_count: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_SAMPLE_COUNT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDepthStencilDescription >> sample_quality [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_SAMPLE_QUALITY
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDepthStencilDescription >> sample_quality: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_SAMPLE_QUALITY put: anObject
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDepthStencilDescription >> stencil_begin_action [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_STENCIL_BEGIN_ACTION
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDepthStencilDescription >> stencil_begin_action: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_STENCIL_BEGIN_ACTION put: anObject
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDepthStencilDescription >> stencil_end_action [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_STENCIL_END_ACTION
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDepthStencilDescription >> stencil_end_action: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_STENCIL_END_ACTION put: anObject
]
