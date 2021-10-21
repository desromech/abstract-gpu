Class {
	#name : #AGPUImmediateRendererLightPbr,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_AMBIENT',
		'OFFSET_AMBIENT_RESERVED',
		'OFFSET_INTENSITY',
		'OFFSET_INTENSITY_RESERVED',
		'OFFSET_POSITION',
		'OFFSET_RADIUS',
		'OFFSET_RESERVED_ATTENUATION',
		'OFFSET_RESERVED_SPECULAR',
		'OFFSET_SPOT_CUTOFF',
		'OFFSET_SPOT_DIRECTION',
		'OFFSET_SPOT_EXPONENT',
		'OFFSET_SPOT_INNER_CUTOFF'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUImmediateRendererLightPbr class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_vector3f ambient;
		 agpu_float ambient_reserved;
		 agpu_vector3f intensity;
		 agpu_float intensity_reserved;
		 agpu_vector4f reserved_specular;
		 agpu_vector4f position;
		 agpu_vector3f spot_direction;
		 agpu_float spot_exponent;
		 agpu_float spot_cutoff;
		 agpu_float spot_inner_cutoff;
		 agpu_float radius;
		 agpu_float reserved_attenuation;
	)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> ambient [
	"This method was automatically generated"
	^ AGPUVector3f fromHandle: (handle referenceStructAt: OFFSET_AMBIENT length: AGPUVector3f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> ambient: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_AMBIENT put: anObject getHandle length: AGPUVector3f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> ambient_reserved [
	"This method was automatically generated"
	^handle floatAt: OFFSET_AMBIENT_RESERVED
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> ambient_reserved: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_AMBIENT_RESERVED put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> intensity [
	"This method was automatically generated"
	^ AGPUVector3f fromHandle: (handle referenceStructAt: OFFSET_INTENSITY length: AGPUVector3f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> intensity: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_INTENSITY put: anObject getHandle length: AGPUVector3f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> intensity_reserved [
	"This method was automatically generated"
	^handle floatAt: OFFSET_INTENSITY_RESERVED
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> intensity_reserved: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_INTENSITY_RESERVED put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> position [
	"This method was automatically generated"
	^ AGPUVector4f fromHandle: (handle referenceStructAt: OFFSET_POSITION length: AGPUVector4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> position: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_POSITION put: anObject getHandle length: AGPUVector4f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> radius [
	"This method was automatically generated"
	^handle floatAt: OFFSET_RADIUS
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> radius: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_RADIUS put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> reserved_attenuation [
	"This method was automatically generated"
	^handle floatAt: OFFSET_RESERVED_ATTENUATION
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> reserved_attenuation: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_RESERVED_ATTENUATION put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> reserved_specular [
	"This method was automatically generated"
	^ AGPUVector4f fromHandle: (handle referenceStructAt: OFFSET_RESERVED_SPECULAR length: AGPUVector4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> reserved_specular: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_RESERVED_SPECULAR put: anObject getHandle length: AGPUVector4f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> spot_cutoff [
	"This method was automatically generated"
	^handle floatAt: OFFSET_SPOT_CUTOFF
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> spot_cutoff: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_SPOT_CUTOFF put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> spot_direction [
	"This method was automatically generated"
	^ AGPUVector3f fromHandle: (handle referenceStructAt: OFFSET_SPOT_DIRECTION length: AGPUVector3f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> spot_direction: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_SPOT_DIRECTION put: anObject getHandle length: AGPUVector3f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> spot_exponent [
	"This method was automatically generated"
	^handle floatAt: OFFSET_SPOT_EXPONENT
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> spot_exponent: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_SPOT_EXPONENT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> spot_inner_cutoff [
	"This method was automatically generated"
	^handle floatAt: OFFSET_SPOT_INNER_CUTOFF
]

{ #category : #'accessing - structure variables' }
AGPUImmediateRendererLightPbr >> spot_inner_cutoff: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_SPOT_INNER_CUTOFF put: anObject
]
