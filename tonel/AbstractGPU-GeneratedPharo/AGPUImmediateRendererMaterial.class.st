Class {
	#name : #AGPUImmediateRendererMaterial,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUImmediateRendererMaterial class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_vector4f emission;
		 agpu_vector4f ambient;
		 agpu_vector4f diffuse;
		 agpu_vector4f specular;
		 agpu_float shininess;
	)
]

