Class {
	#name : #AGPUTextureViewDescription,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUTextureViewDescription class >> fields [
	"
	self defineFields
	"
    ^ #(
		(type 'long')
		(format 'long')
		(sample_count 'ulong')
		(usage_mode 'long')
		(components 'AGPUComponentsSwizzle')
		(subresource_range 'AGPUTextureSubresourceRange')
	)
]

