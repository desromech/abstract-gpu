Class {
	#name : #AGPUSamplerDescription,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUSamplerDescription class >> fields [
	"
	self defineFields
	"
    ^ #(
		(filter 'long')
		(address_u 'long')
		(address_v 'long')
		(address_w 'long')
		(mip_lod_bias 'float')
		(maxanisotropy 'float')
		(comparison_enabled 'long')
		(comparison_function 'long')
		(border_color 'AGPUColor4f')
		(min_lod 'float')
		(max_lod 'float')
	)
]

