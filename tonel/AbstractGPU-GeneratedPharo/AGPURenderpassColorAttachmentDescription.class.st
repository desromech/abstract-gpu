Class {
	#name : #AGPURenderpassColorAttachmentDescription,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
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

