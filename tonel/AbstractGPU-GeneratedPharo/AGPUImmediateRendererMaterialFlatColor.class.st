Class {
	#name : #AGPUImmediateRendererMaterialFlatColor,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_ALPHA_CUTOFF',
		'OFFSET_COLOR'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUImmediateRendererMaterialFlatColor class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_vector4f color;
		 agpu_float alpha_cutoff;
	)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialFlatColor >> alpha_cutoff [
	"This method was automatically generated"
	^handle floatAt: OFFSET_ALPHA_CUTOFF
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialFlatColor >> alpha_cutoff: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_ALPHA_CUTOFF put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialFlatColor >> color [
	"This method was automatically generated"
	^ AGPUVector4f fromHandle: (handle referenceStructAt: OFFSET_COLOR length: AGPUVector4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererMaterialFlatColor >> color: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_COLOR put: anObject getHandle length: AGPUVector4f byteSize
]
