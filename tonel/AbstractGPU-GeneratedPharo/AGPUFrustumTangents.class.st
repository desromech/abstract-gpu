Class {
	#name : #AGPUFrustumTangents,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUFrustumTangents class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_float left;
		 agpu_float right;
		 agpu_float top;
		 agpu_float bottom;
	)
]

