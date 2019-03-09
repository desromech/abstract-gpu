Class {
	#name : #AGPUSize2d,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'definition' }
AGPUSize2d class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
	^ #(
		 agpu_uint width;
		 agpu_uint height;
		)
]

