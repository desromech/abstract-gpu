Class {
	#name : #AGPUImmediateRendererMaterialClassic,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUImmediateRendererMaterialClassic class >> fields [
	"
	self defineFields
	"
    ^ #(
		(emission 'AGPUVector4f')
		(ambient 'AGPUVector4f')
		(diffuse 'AGPUVector4f')
		(specular 'AGPUVector4f')
		(shininess 'float')
		(alpha_cutoff 'float')
	)
]

