Class {
	#name : #AGPUDeviceOpenInfo,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUDeviceOpenInfo class >> fields [
	"
	self defineFields
	"
    ^ #(
		(display 'void*')
		(window_system_name 'byte*')
		(debug_layer 'long')
		(application_name 'byte*')
		(application_version 'ulong')
		(engine_name 'byte*')
		(engine_version 'ulong')
		(gpu_index 'long')
		(open_flags 'long')
	)
]

