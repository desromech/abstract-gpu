Class {
	#name : #AGPUQuaternionf,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUQuaternionf class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_float w;
		 agpu_float x;
		 agpu_float y;
		 agpu_float z;
	)
]

