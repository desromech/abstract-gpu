Class {
	#name : #AGPUVrGenericEvent,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-Generated'
}

{ #category : #'definition' }
AGPUVrGenericEvent class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
	^ #(
		 agpu_uint word1;
		 agpu_uint word2;
		 agpu_uint word3;
		 agpu_uint word4;
		 agpu_uint word5;
		 agpu_uint word6;
		 agpu_uint word7;
		 agpu_uint word8;
		)
]

