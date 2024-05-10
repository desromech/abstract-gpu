Class {
	#name : #AGPUVrControllerAxisState,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUVrControllerAxisState class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_vr_controller_axis_type type;
		 agpu_float x;
		 agpu_float y;
	)
]

