Class {
	#name : #AGPUVertexAttribDescription,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUVertexAttribDescription class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint buffer;
		 agpu_uint binding;
		 agpu_texture_format format;
		 agpu_size offset;
		 agpu_uint divisor;
	)
]

