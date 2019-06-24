Class {
	#name : #AGPUSwapChainCreateInfo,
	#pools : [
		'AGPUConstants'
	],
	#superclass : #ExternalStructure,
	#category : 'AbstractGPU-GeneratedSqueak'
}

{ #category : #'definition' }
AGPUSwapChainCreateInfo class >> fields [
	"
	self defineFields
	"
    ^ #(
		(display 'void*')
		(window 'void*')
		(window_system_name 'byte*')
		(surface 'void*')
		(colorbuffer_format 'long')
		(depth_stencil_format 'long')
		(width 'ulong')
		(height 'ulong')
		(buffer_count 'ulong')
		(sample_buffers 'long')
		(samples 'long')
	)
]

