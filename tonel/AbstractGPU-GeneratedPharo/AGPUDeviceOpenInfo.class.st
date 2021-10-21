Class {
	#name : #AGPUDeviceOpenInfo,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_APPLICATION_NAME',
		'OFFSET_APPLICATION_VERSION',
		'OFFSET_DEBUG_LAYER',
		'OFFSET_DISPLAY',
		'OFFSET_ENGINE_NAME',
		'OFFSET_ENGINE_VERSION',
		'OFFSET_GPU_INDEX',
		'OFFSET_OPEN_FLAGS',
		'OFFSET_WINDOW_SYSTEM_NAME'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
AGPUDeviceOpenInfo class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_pointer display;
		 agpu_cstring window_system_name;
		 agpu_bool debug_layer;
		 agpu_cstring application_name;
		 agpu_uint application_version;
		 agpu_cstring engine_name;
		 agpu_uint engine_version;
		 agpu_int gpu_index;
		 agpu_device_open_flags open_flags;
	)
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> application_name [
	"This method was automatically generated"
	^ExternalData fromHandle: (handle pointerAt: OFFSET_APPLICATION_NAME) type: ExternalType char asPointerType
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> application_name: anObject [
	"This method was automatically generated"
	handle pointerAt: OFFSET_APPLICATION_NAME put: anObject getHandle.
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> application_version [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_APPLICATION_VERSION
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> application_version: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_APPLICATION_VERSION put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> debug_layer [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_DEBUG_LAYER
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> debug_layer: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_DEBUG_LAYER put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> display [
	"This method was automatically generated"
	^ExternalData fromHandle: (handle pointerAt: OFFSET_DISPLAY) type: ExternalType void asPointerType
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> display: anObject [
	"This method was automatically generated"
	handle pointerAt: OFFSET_DISPLAY put: anObject getHandle.
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> engine_name [
	"This method was automatically generated"
	^ExternalData fromHandle: (handle pointerAt: OFFSET_ENGINE_NAME) type: ExternalType char asPointerType
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> engine_name: anObject [
	"This method was automatically generated"
	handle pointerAt: OFFSET_ENGINE_NAME put: anObject getHandle.
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> engine_version [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_ENGINE_VERSION
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> engine_version: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_ENGINE_VERSION put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> gpu_index [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_GPU_INDEX
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> gpu_index: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_GPU_INDEX put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> open_flags [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_OPEN_FLAGS
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> open_flags: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_OPEN_FLAGS put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> window_system_name [
	"This method was automatically generated"
	^ExternalData fromHandle: (handle pointerAt: OFFSET_WINDOW_SYSTEM_NAME) type: ExternalType char asPointerType
]

{ #category : #'accessing - structure variables' }
AGPUDeviceOpenInfo >> window_system_name: anObject [
	"This method was automatically generated"
	handle pointerAt: OFFSET_WINDOW_SYSTEM_NAME put: anObject getHandle.
]
