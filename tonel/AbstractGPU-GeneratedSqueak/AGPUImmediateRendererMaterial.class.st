Class {
	#name : #AGPUImmediateRendererMaterial,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUImmediateRendererMaterial class >> fields [
	"
	self defineFields
	"
    ^ #(
		(emission 'AGPUVector4f')
		(ambient 'AGPUVector4f')
		(diffuse 'AGPUVector4f')
		(specular 'AGPUVector4f')
		(shininess 'float')
	)
]

