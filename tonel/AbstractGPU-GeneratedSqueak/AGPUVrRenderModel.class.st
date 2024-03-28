Class {
	#name : #AGPUVrRenderModel,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUVrRenderModel class >> fields [
	"
	self defineFields
	"
    ^ #(
		(vertex_count 'ulong')
		(vertices 'AGPUVrRenderModelVertex*')
		(triangle_count 'ulong')
		(indices 'ushort*')
		(texture 'AGPUVrRenderModelTexture*')
	)
]

