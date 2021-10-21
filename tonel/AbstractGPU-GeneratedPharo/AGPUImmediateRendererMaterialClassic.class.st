Class {
	#name : #AGPUImmediateRendererMaterialClassic,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_ALPHA_CUTOFF',
		'OFFSET_AMBIENT',
		'OFFSET_DIFFUSE',
		'OFFSET_EMISSION',
		'OFFSET_SHININESS',
		'OFFSET_SPECULAR'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUImmediateRendererMaterialClassic class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_vector4f emission;
		 agpu_vector4f ambient;
		 agpu_vector4f diffuse;
		 agpu_vector4f specular;
		 agpu_float shininess;
		 agpu_float alpha_cutoff;
	)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialClassic >> alpha_cutoff [
	"This method was automatically generated"
	^handle floatAt: OFFSET_ALPHA_CUTOFF
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialClassic >> alpha_cutoff: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_ALPHA_CUTOFF put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialClassic >> ambient [
	"This method was automatically generated"
	^ AGPUVector4f fromHandle: (handle referenceStructAt: OFFSET_AMBIENT length: AGPUVector4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialClassic >> ambient: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_AMBIENT put: anObject getHandle length: AGPUVector4f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialClassic >> diffuse [
	"This method was automatically generated"
	^ AGPUVector4f fromHandle: (handle referenceStructAt: OFFSET_DIFFUSE length: AGPUVector4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialClassic >> diffuse: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_DIFFUSE put: anObject getHandle length: AGPUVector4f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialClassic >> emission [
	"This method was automatically generated"
	^ AGPUVector4f fromHandle: (handle referenceStructAt: OFFSET_EMISSION length: AGPUVector4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialClassic >> emission: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_EMISSION put: anObject getHandle length: AGPUVector4f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialClassic >> shininess [
	"This method was automatically generated"
	^handle floatAt: OFFSET_SHININESS
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialClassic >> shininess: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_SHININESS put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialClassic >> specular [
	"This method was automatically generated"
	^ AGPUVector4f fromHandle: (handle referenceStructAt: OFFSET_SPECULAR length: AGPUVector4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialClassic >> specular: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_SPECULAR put: anObject getHandle length: AGPUVector4f byteSize
]
