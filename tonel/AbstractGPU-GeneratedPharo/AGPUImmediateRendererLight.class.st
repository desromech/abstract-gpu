Class {
	#name : #AGPUImmediateRendererLight,
	#superclass : #FFIExternalUnion,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUImmediateRendererLight class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_immediate_renderer_light_classic classic;
		 agpu_immediate_renderer_light_pbr pbr;
	)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLight >> classic [
	"This method was automatically generated"
	^ AGPUImmediateRendererLightClassic fromHandle: (handle referenceStructAt: 1 length: AGPUImmediateRendererLightClassic byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLight >> classic: anObject [
	"This method was automatically generated"
	handle structAt: 1 put: anObject getHandle length: 96.
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLight >> pbr [
	"This method was automatically generated"
	^ AGPUImmediateRendererLightPbr fromHandle: (handle referenceStructAt: 1 length: AGPUImmediateRendererLightPbr byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLight >> pbr: anObject [
	"This method was automatically generated"
	handle structAt: 1 put: anObject getHandle length: 96.
]
