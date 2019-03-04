Class {
	#name : #AGPUMatrix3x3f,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'definition' }
AGPUMatrix3x3f class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
	^ #(
		 agpu_vector3f c1;
		 agpu_vector3f c2;
		 agpu_vector3f c3;
		)
]

