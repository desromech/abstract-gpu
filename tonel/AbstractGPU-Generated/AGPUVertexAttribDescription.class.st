Class {
	#name : #AGPUVertexAttribDescription,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-Generated'
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
		 agpu_uint rows;
		 agpu_size offset;
		 agpu_uint divisor;
		)
]

