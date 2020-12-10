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
		 agpu_vector4f reserved_ambient;
		 agpu_vector3f intensity;
		 agpu_float reserved;
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

