Class {
	#name : #AGPUInheritanceInfo,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'definition' }
AGPUInheritanceInfo class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
	^ #(
		 agpu_int flat;
		 agpu_renderpass* renderpass;
		)
]

