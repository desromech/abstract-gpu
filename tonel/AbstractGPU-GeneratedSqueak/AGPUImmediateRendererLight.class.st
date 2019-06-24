Class {
	#name : #AGPUImmediateRendererLight,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #AGPUExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUImmediateRendererLight class >> fields [
	"
	self defineFields
	"
    ^ #(
		(ambient 'AGPUVector4f')
		(diffuse 'AGPUVector4f')
		(specular 'AGPUVector4f')
		(position 'AGPUVector4f')
		(spot_direction 'AGPUVector3f')
		(spot_exponent 'float')
		(spot_cutoff 'float')
		(constant_attenuation 'float')
		(linear_attenuation 'float')
		(quadratic_attenuation 'float')
	)
]

