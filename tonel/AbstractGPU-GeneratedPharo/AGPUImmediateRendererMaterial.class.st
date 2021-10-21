Class {
	#name : #AGPUImmediateRendererMaterial,
	#superclass : #FFIExternalUnion,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUImmediateRendererMaterial class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_immediate_renderer_material_classic classic;
		 agpu_immediate_renderer_material_metallic_roughness metallic_roughness;
		 agpu_immediate_renderer_material_flat_color flat_color;
	)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterial >> classic [
	"This method was automatically generated"
	^ AGPUImmediateRendererMaterialClassic fromHandle: (handle referenceStructAt: 1 length: AGPUImmediateRendererMaterialClassic byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterial >> classic: anObject [
	"This method was automatically generated"
	handle structAt: 1 put: anObject getHandle length: 72.
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterial >> flat_color [
	"This method was automatically generated"
	^ AGPUImmediateRendererMaterialFlatColor fromHandle: (handle referenceStructAt: 1 length: AGPUImmediateRendererMaterialFlatColor byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterial >> flat_color: anObject [
	"This method was automatically generated"
	handle structAt: 1 put: anObject getHandle length: 20.
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterial >> metallic_roughness [
	"This method was automatically generated"
	^ AGPUImmediateRendererMaterialMetallicRoughness fromHandle: (handle referenceStructAt: 1 length: AGPUImmediateRendererMaterialMetallicRoughness byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterial >> metallic_roughness: anObject [
	"This method was automatically generated"
	handle structAt: 1 put: anObject getHandle length: 48.
]
