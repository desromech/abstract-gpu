Class {
	#name : #AGPUVrControllerState,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUVrControllerState class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_ulong buttons_pressed;
		 agpu_ulong buttons_touched;
		 agpu_vr_controller_axis_state axis0;
		 agpu_vr_controller_axis_state axis1;
		 agpu_vr_controller_axis_state axis2;
		 agpu_vr_controller_axis_state axis3;
		 agpu_vr_controller_axis_state axis4;
	)
]

