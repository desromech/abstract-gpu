Class {
	#name : #AGPURenderpassColorAttachmentDescription,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #AGPUExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPURenderpassColorAttachmentDescription class >> fields [
	"
	self defineFields
	"
    ^ #(
		(format 'long')
		(sample_count 'ulong')
		(begin_action 'long')
		(end_action 'long')
		(clear_value 'AGPUColor4f')
	)
]

