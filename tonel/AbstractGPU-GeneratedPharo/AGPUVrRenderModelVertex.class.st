Class {
	#name : #AGPUVrRenderModelVertex,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUVrRenderModelVertex class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_vector3f position;
		 agpu_vector3f normal;
		 agpu_vector2f texcoord;
	)
]

