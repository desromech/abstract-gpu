Class {
	#name : #AGPUVrControllerEvent,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'definition' }
AGPUVrControllerEvent class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
	^ #(
		 agpu_uint button;
		)
]

