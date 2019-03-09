Class {
	#name : #AGPUVrDualAnalogEvent,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'definition' }
AGPUVrDualAnalogEvent class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
	^ #(
		 agpu_float x;
		 agpu_float y;
		 agpu_float transformed_x;
		 agpu_float transformed_y;
		 agpu_uint which;
		)
]

