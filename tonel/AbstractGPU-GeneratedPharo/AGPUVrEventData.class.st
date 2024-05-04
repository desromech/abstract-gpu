Class {
	#name : #AGPUVrEventData,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalUnion,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUVrEventData class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint type;
		 agpu_vr_generic_event generic;
		 agpu_vr_controller_event controller;
		 agpu_vr_dual_analog_event dual_analog;
	)
]

