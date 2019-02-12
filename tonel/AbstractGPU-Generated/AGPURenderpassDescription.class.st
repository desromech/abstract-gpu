Class {
	#name : #AGPURenderpassDescription,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'definition' }
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

