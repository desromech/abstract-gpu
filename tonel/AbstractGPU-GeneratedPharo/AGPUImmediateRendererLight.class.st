Class {
	#name : #AGPUImmediateRendererLight,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUImmediateRendererLight class >> fieldsDesc [
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

