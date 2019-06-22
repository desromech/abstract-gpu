Class {
	#name : #AGPUMatrix4x4f,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUMatrix4x4f class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_vector4f c1;
		 agpu_vector4f c2;
		 agpu_vector4f c3;
		 agpu_vector4f c4;
	)
]

