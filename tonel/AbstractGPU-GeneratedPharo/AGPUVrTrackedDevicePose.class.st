Class {
	#name : #AGPUVrTrackedDevicePose,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUVrTrackedDevicePose class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint device_id;
		 agpu_vr_tracked_device_class device_class;
		 agpu_vr_tracked_device_role device_role;
		 agpu_matrix4x4f device_to_absolute_tracking;
		 agpu_vector3f velocity;
		 agpu_vector3f angular_velocity;
		 agpu_bool is_valid;
	)
]

