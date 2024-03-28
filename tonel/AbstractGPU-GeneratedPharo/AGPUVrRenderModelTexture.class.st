Class {
	#name : #AGPUVrRenderModelTexture,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUVrRenderModelTexture class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_ushort width;
		 agpu_ushort height;
		 agpu_uint pitch;
		 agpu_byte* data;
	)
]

