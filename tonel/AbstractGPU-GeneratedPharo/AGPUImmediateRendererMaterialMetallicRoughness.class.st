Class {
	#name : #AGPUImmediateRendererMaterialMetallicRoughness,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_ALPHA_CUTOFF',
		'OFFSET_BASE_COLOR',
		'OFFSET_EMISSION',
		'OFFSET_METALLIC_FACTOR',
		'OFFSET_OCCLUSION_FACTOR',
		'OFFSET_ROUGHNESS_FACTOR'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUImmediateRendererMaterialMetallicRoughness class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_vector4f emission;
		 agpu_vector4f base_color;
		 agpu_float metallic_factor;
		 agpu_float roughness_factor;
		 agpu_float occlusion_factor;
		 agpu_float alpha_cutoff;
	)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialMetallicRoughness >> alpha_cutoff [
	"This method was automatically generated"
	^handle floatAt: OFFSET_ALPHA_CUTOFF
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialMetallicRoughness >> alpha_cutoff: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_ALPHA_CUTOFF put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialMetallicRoughness >> base_color [
	"This method was automatically generated"
	^ AGPUVector4f fromHandle: (handle referenceStructAt: OFFSET_BASE_COLOR length: AGPUVector4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialMetallicRoughness >> base_color: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_BASE_COLOR put: anObject getHandle length: AGPUVector4f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialMetallicRoughness >> emission [
	"This method was automatically generated"
	^ AGPUVector4f fromHandle: (handle referenceStructAt: OFFSET_EMISSION length: AGPUVector4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialMetallicRoughness >> emission: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_EMISSION put: anObject getHandle length: AGPUVector4f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialMetallicRoughness >> metallic_factor [
	"This method was automatically generated"
	^handle floatAt: OFFSET_METALLIC_FACTOR
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialMetallicRoughness >> metallic_factor: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_METALLIC_FACTOR put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialMetallicRoughness >> occlusion_factor [
	"This method was automatically generated"
	^handle floatAt: OFFSET_OCCLUSION_FACTOR
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialMetallicRoughness >> occlusion_factor: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_OCCLUSION_FACTOR put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialMetallicRoughness >> roughness_factor [
	"This method was automatically generated"
	^handle floatAt: OFFSET_ROUGHNESS_FACTOR
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialMetallicRoughness >> roughness_factor: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_ROUGHNESS_FACTOR put: anObject
]
