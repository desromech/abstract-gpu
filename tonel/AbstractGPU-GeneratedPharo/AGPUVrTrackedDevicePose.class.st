Class {
	#name : #AGPUVrTrackedDevicePose,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_ANGULAR_VELOCITY',
		'OFFSET_DEVICE_CLASS',
		'OFFSET_DEVICE_ID',
		'OFFSET_DEVICE_ROLE',
		'OFFSET_DEVICE_TO_ABSOLUTE_TRACKING',
		'OFFSET_IS_VALID',
		'OFFSET_VELOCITY'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
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

{ #category : #'accessing - structure variables' }
AGPUVrTrackedDevicePose >> angular_velocity [
	"This method was automatically generated"
	^ AGPUVector3f fromHandle: (handle referenceStructAt: OFFSET_ANGULAR_VELOCITY length: AGPUVector3f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUVrTrackedDevicePose >> angular_velocity: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_ANGULAR_VELOCITY put: anObject getHandle length: AGPUVector3f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUVrTrackedDevicePose >> device_class [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_DEVICE_CLASS
]

{ #category : #'accessing - structure variables' }
AGPUVrTrackedDevicePose >> device_class: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_DEVICE_CLASS put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVrTrackedDevicePose >> device_id [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_DEVICE_ID
]

{ #category : #'accessing - structure variables' }
AGPUVrTrackedDevicePose >> device_id: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_DEVICE_ID put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVrTrackedDevicePose >> device_role [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_DEVICE_ROLE
]

{ #category : #'accessing - structure variables' }
AGPUVrTrackedDevicePose >> device_role: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_DEVICE_ROLE put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVrTrackedDevicePose >> device_to_absolute_tracking [
	"This method was automatically generated"
	^ AGPUMatrix4x4f fromHandle: (handle referenceStructAt: OFFSET_DEVICE_TO_ABSOLUTE_TRACKING length: AGPUMatrix4x4f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUVrTrackedDevicePose >> device_to_absolute_tracking: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_DEVICE_TO_ABSOLUTE_TRACKING put: anObject getHandle length: AGPUMatrix4x4f byteSize
]

{ #category : #'accessing - structure variables' }
AGPUVrTrackedDevicePose >> is_valid [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_IS_VALID
]

{ #category : #'accessing - structure variables' }
AGPUVrTrackedDevicePose >> is_valid: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_IS_VALID put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUVrTrackedDevicePose >> velocity [
	"This method was automatically generated"
	^ AGPUVector3f fromHandle: (handle referenceStructAt: OFFSET_VELOCITY length: AGPUVector3f byteSize)
]

{ #category : #'accessing - structure variables' }
AGPUVrTrackedDevicePose >> velocity: anObject [
	"This method was automatically generated"
	handle structAt: OFFSET_VELOCITY put: anObject getHandle length: AGPUVector3f byteSize
]
