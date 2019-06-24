Class {
	#name : #AGPURegion3d,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPURegion3d class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint x;
		 agpu_uint y;
		 agpu_uint z;
		 agpu_uint width;
		 agpu_uint height;
		 agpu_uint depth;
	)
]

