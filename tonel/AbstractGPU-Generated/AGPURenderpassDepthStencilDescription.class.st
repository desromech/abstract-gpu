Class {
	#name : #AGPURenderpassDepthStencilDescription,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'definition' }
AGPURenderpassDepthStencilDescription class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
	^ #(
		 agpu_texture_format format;
		 agpu_uint sample_count;
		 agpu_renderpass_attachment_action begin_action;
		 agpu_renderpass_attachment_action end_action;
		 agpu_renderpass_attachment_action stencil_begin_action;
		 agpu_renderpass_attachment_action stencil_end_action;
		 agpu_depth_stencil_value clear_value;
		)
]

