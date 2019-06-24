Class {
	#name : #AGPURenderpassDescription,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #AGPUExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPURenderpassDescription class >> fields [
	"
	self defineFields
	"
    ^ #(
		(color_attachment_count 'ulong')
		(color_attachments 'AGPURenderpassColorAttachmentDescription*')
		(depth_stencil_attachment 'AGPURenderpassDepthStencilDescription*')
	)
]

