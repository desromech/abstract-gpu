Class {
	#name : #AGPUTextureDescription,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUTextureDescription class >> fields [
	"
	self defineFields
	"
    ^ #(
		(type 'long')
		(width 'ulong')
		(height 'ulong')
		(depth 'ulong')
		(layers 'ulong')
		(miplevels 'ushort')
		(format 'long')
		(usage_modes 'long')
		(main_usage_mode 'long')
		(heap_type 'long')
		(sample_count 'ulong')
		(sample_quality 'ulong')
	)
]

