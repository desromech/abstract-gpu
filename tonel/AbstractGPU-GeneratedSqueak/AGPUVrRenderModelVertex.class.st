Class {
	#name : #AGPUVrRenderModelVertex,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUVrRenderModelVertex class >> fields [
	"
	self defineFields
	"
    ^ #(
		(position 'AGPUVector3f')
		(normal 'AGPUVector3f')
		(texcoord 'AGPUVector2f')
	)
]

