Class {
	#name : #AGPUBufferDescription,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUBufferDescription class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_uint size;
		 agpu_memory_heap_type heap_type;
		 agpu_buffer_binding_type binding;
		 agpu_bitfield mapping_flags;
		 agpu_uint stride;
	)
]

