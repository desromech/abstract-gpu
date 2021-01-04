Class {
	#name : #AGPUImmediateRendererLightPbr,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
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

