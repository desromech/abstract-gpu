Class {
	#name : #AGPUImmediateRendererLightClassic,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_AMBIENT',
		'OFFSET_CONSTANT_ATTENUATION',
		'OFFSET_DIFFUSE',
		'OFFSET_LINEAR_ATTENUATION',
		'OFFSET_POSITION',
		'OFFSET_QUADRATIC_ATTENUATION',
		'OFFSET_SPECULAR',
		'OFFSET_SPOT_CUTOFF',
		'OFFSET_SPOT_DIRECTION',
		'OFFSET_SPOT_EXPONENT'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUImmediateRendererLightClassic class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_vector4f ambient;
		 agpu_vector4f diffuse;
		 agpu_vector4f specular;
		 agpu_vector4f position;
		 agpu_vector3f spot_direction;
		 agpu_float spot_exponent;
		 agpu_float spot_cutoff;
		 agpu_float constant_attenuation;
		 agpu_float linear_attenuation;
		 agpu_float quadratic_attenuation;
	)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> ambient [
	"This method was automatically generated"
	^ AGPUVector4f fromHandle: (handle referenceStructAt: OFFSET_AMBIENT length: AGPUVector4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> ambient: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_AMBIENT put: anObject getHandle length: AGPUVector4f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> constant_attenuation [
	"This method was automatically generated"
	^handle floatAt: OFFSET_CONSTANT_ATTENUATION
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> constant_attenuation: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_CONSTANT_ATTENUATION put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> diffuse [
	"This method was automatically generated"
	^ AGPUVector4f fromHandle: (handle referenceStructAt: OFFSET_DIFFUSE length: AGPUVector4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> diffuse: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_DIFFUSE put: anObject getHandle length: AGPUVector4f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> linear_attenuation [
	"This method was automatically generated"
	^handle floatAt: OFFSET_LINEAR_ATTENUATION
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> linear_attenuation: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_LINEAR_ATTENUATION put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> position [
	"This method was automatically generated"
	^ AGPUVector4f fromHandle: (handle referenceStructAt: OFFSET_POSITION length: AGPUVector4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> position: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_POSITION put: anObject getHandle length: AGPUVector4f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> quadratic_attenuation [
	"This method was automatically generated"
	^handle floatAt: OFFSET_QUADRATIC_ATTENUATION
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> quadratic_attenuation: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_QUADRATIC_ATTENUATION put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> specular [
	"This method was automatically generated"
	^ AGPUVector4f fromHandle: (handle referenceStructAt: OFFSET_SPECULAR length: AGPUVector4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> specular: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_SPECULAR put: anObject getHandle length: AGPUVector4f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> spot_cutoff [
	"This method was automatically generated"
	^handle floatAt: OFFSET_SPOT_CUTOFF
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> spot_cutoff: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_SPOT_CUTOFF put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> spot_direction [
	"This method was automatically generated"
	^ AGPUVector3f fromHandle: (handle referenceStructAt: OFFSET_SPOT_DIRECTION length: AGPUVector3f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> spot_direction: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_SPOT_DIRECTION put: anObject getHandle length: AGPUVector3f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> spot_exponent [
	"This method was automatically generated"
	^handle floatAt: OFFSET_SPOT_EXPONENT
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightClassic >> spot_exponent: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_SPOT_EXPONENT put: anObject
]
