Class {
	#name : #AGPUSamplerDescription,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_ADDRESS_U',
		'OFFSET_ADDRESS_V',
		'OFFSET_ADDRESS_W',
		'OFFSET_BORDER_COLOR',
		'OFFSET_COMPARISON_ENABLED',
		'OFFSET_COMPARISON_FUNCTION',
		'OFFSET_FILTER',
		'OFFSET_MAXANISOTROPY',
		'OFFSET_MAX_LOD',
		'OFFSET_MIN_LOD',
		'OFFSET_MIP_LOD_BIAS'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUSamplerDescription class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_filter filter;
		 agpu_texture_address_mode address_u;
		 agpu_texture_address_mode address_v;
		 agpu_texture_address_mode address_w;
		 agpu_float mip_lod_bias;
		 agpu_float maxanisotropy;
		 agpu_bool comparison_enabled;
		 agpu_compare_function comparison_function;
		 agpu_color4f border_color;
		 agpu_float min_lod;
		 agpu_float max_lod;
	)
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> address_u [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_ADDRESS_U
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> address_u: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_ADDRESS_U put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> address_v [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_ADDRESS_V
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> address_v: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_ADDRESS_V put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> address_w [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_ADDRESS_W
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> address_w: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_ADDRESS_W put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> border_color [
	"This method was automatically generated"
	^ AGPUColor4f fromHandle: (handle referenceStructAt: OFFSET_BORDER_COLOR length: AGPUColor4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> border_color: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_BORDER_COLOR put: anObject getHandle length: AGPUColor4f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> comparison_enabled [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_COMPARISON_ENABLED
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> comparison_enabled: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_COMPARISON_ENABLED put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> comparison_function [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_COMPARISON_FUNCTION
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> comparison_function: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_COMPARISON_FUNCTION put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> filter [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_FILTER
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> filter: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_FILTER put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> max_lod [
	"This method was automatically generated"
	^handle floatAt: OFFSET_MAX_LOD
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> max_lod: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_MAX_LOD put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> maxanisotropy [
	"This method was automatically generated"
	^handle floatAt: OFFSET_MAXANISOTROPY
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> maxanisotropy: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_MAXANISOTROPY put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> min_lod [
	"This method was automatically generated"
	^handle floatAt: OFFSET_MIN_LOD
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> min_lod: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_MIN_LOD put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> mip_lod_bias [
	"This method was automatically generated"
	^handle floatAt: OFFSET_MIP_LOD_BIAS
]

{ #category : #'accessing - structure variables' }
AGPUSamplerDescription >> mip_lod_bias: anObject [
	"This method was automatically generated"
	handle floatAt: OFFSET_MIP_LOD_BIAS put: anObject
]
