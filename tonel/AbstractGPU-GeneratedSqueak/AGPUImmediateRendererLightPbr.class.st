Class {
	#name : #AGPUImmediateRendererLightPbr,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUImmediateRendererLightPbr class >> fields [
	"
	self defineFields
	"
    ^ #(
		(reserved_ambient 'AGPUVector4f')
		(intensity 'AGPUVector3f')
		(reserved 'float')
		(reserved_specular 'AGPUVector4f')
		(position 'AGPUVector4f')
		(spot_direction 'AGPUVector3f')
		(spot_exponent 'float')
		(spot_cutoff 'float')
		(spot_inner_cutoff 'float')
		(radius 'float')
		(reserved_attenuation 'float')
	)
]

