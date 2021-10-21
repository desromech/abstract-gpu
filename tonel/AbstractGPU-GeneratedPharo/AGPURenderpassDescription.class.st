Class {
	#name : #AGPURenderpassDescription,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_COLOR_ATTACHMENTS',
		'OFFSET_COLOR_ATTACHMENT_COUNT',
		'OFFSET_DEPTH_STENCIL_ATTACHMENT'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPURenderpassDescription class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_size color_attachment_count;
		 agpu_renderpass_color_attachment_description* color_attachments;
		 agpu_renderpass_depth_stencil_description* depth_stencil_attachment;
	)
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDescription >> color_attachment_count [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_COLOR_ATTACHMENT_COUNT
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDescription >> color_attachment_count: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_COLOR_ATTACHMENT_COUNT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDescription >> color_attachments [
	"This method was automatically generated"
	^AGPURenderpassColorAttachmentDescription fromHandle: (handle pointerAt: OFFSET_COLOR_ATTACHMENTS)
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDescription >> color_attachments: anObject [
	"This method was automatically generated"
	handle pointerAt: OFFSET_COLOR_ATTACHMENTS put: anObject getHandle.
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDescription >> depth_stencil_attachment [
	"This method was automatically generated"
	^AGPURenderpassDepthStencilDescription fromHandle: (handle pointerAt: OFFSET_DEPTH_STENCIL_ATTACHMENT)
]

{ #category : #'accessing - structure variables' }
AGPURenderpassDescription >> depth_stencil_attachment: anObject [
	"This method was automatically generated"
	handle pointerAt: OFFSET_DEPTH_STENCIL_ATTACHMENT put: anObject getHandle.
]
