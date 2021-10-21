Class {
	#name : #AGPURenderpassColorAttachmentDescription,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_BEGIN_ACTION',
		'OFFSET_CLEAR_VALUE',
		'OFFSET_END_ACTION',
		'OFFSET_FORMAT',
		'OFFSET_SAMPLE_COUNT',
		'OFFSET_SAMPLE_QUALITY'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPURenderpassColorAttachmentDescription class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_texture_format format;
		 agpu_uint sample_count;
		 agpu_uint sample_quality;
		 agpu_renderpass_attachment_action begin_action;
		 agpu_renderpass_attachment_action end_action;
		 agpu_color4f clear_value;
	)
]

{ #category : #'accessing - structure variables' }
AGPURenderpassColorAttachmentDescription >> begin_action [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_BEGIN_ACTION
]

{ #category : #'accessing - structure variables' }
AGPURenderpassColorAttachmentDescription >> begin_action: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_BEGIN_ACTION put: anObject
]

{ #category : #'accessing - structure variables' }
AGPURenderpassColorAttachmentDescription >> clear_value [
	"This method was automatically generated"
	^ AGPUColor4f fromHandle: (handle referenceStructAt: OFFSET_CLEAR_VALUE length: AGPUColor4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPURenderpassColorAttachmentDescription >> clear_value: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_CLEAR_VALUE put: anObject getHandle length: AGPUColor4f byteSize
]

{ #category : #'accessing - structure variables' }
AGPURenderpassColorAttachmentDescription >> end_action [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_END_ACTION
]

{ #category : #'accessing - structure variables' }
AGPURenderpassColorAttachmentDescription >> end_action: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_END_ACTION put: anObject
]

{ #category : #'accessing - structure variables' }
AGPURenderpassColorAttachmentDescription >> format [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_FORMAT
]

{ #category : #'accessing - structure variables' }
AGPURenderpassColorAttachmentDescription >> format: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_FORMAT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPURenderpassColorAttachmentDescription >> sample_count [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_SAMPLE_COUNT
]

{ #category : #'accessing - structure variables' }
AGPURenderpassColorAttachmentDescription >> sample_count: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_SAMPLE_COUNT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPURenderpassColorAttachmentDescription >> sample_quality [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_SAMPLE_QUALITY
]

{ #category : #'accessing - structure variables' }
AGPURenderpassColorAttachmentDescription >> sample_quality: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_SAMPLE_QUALITY put: anObject
]
