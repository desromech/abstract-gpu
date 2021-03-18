Class {
	#name : #AGPUImmediateRendererMaterialFlatColor,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUImmediateRendererMaterialFlatColor class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_vector4f color;
	)
]

