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
		(layers 'ulong')
		(buffer_count 'ulong')
		(sample_buffers 'long')
		(samples 'long')
		(flags 'long')
		(old_swap_chain 'void*')
		(presentation_mode 'long')
		(fallback_presentation_mode 'long')
		(x 'long')
		(y 'long')
	)
]

