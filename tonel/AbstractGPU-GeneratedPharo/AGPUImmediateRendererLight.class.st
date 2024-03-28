Class {
	#name : #AGPUImmediateRendererLight,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalUnion,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUImmediateRendererLight class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_immediate_renderer_light_classic classic;
		 agpu_immediate_renderer_light_pbr pbr;
	)
]

