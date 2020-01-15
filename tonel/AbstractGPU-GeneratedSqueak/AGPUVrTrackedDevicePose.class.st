Class {
	#name : #AGPUVrTrackedDevicePose,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUVrTrackedDevicePose class >> fields [
	"
	self defineFields
	"
    ^ #(
		(device_id 'ulong')
		(device_class 'long')
		(device_role 'long')
		(device_to_absolute_tracking 'AGPUMatrix4x4f')
		(velocity 'AGPUVector3f')
		(angular_velocity 'AGPUVector3f')
		(is_valid 'long')
	)
]

