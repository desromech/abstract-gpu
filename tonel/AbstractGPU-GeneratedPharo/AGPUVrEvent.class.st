Class {
	#name : #AGPUVrEvent,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUVrEvent class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint type;
		 agpu_uint tracked_device_index;
		 agpu_float event_age_seconds;
		 agpu_vr_event_data data;
	)
]

