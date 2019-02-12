Class {
	#name : #AGPUDeviceOpenInfo,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'definition' }
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
		)
]

