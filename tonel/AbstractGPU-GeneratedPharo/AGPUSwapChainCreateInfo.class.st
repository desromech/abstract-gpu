Class {
	#name : #AGPUSwapChainCreateInfo,
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#superclass : #FFIExternalStructure,
	#category : 'AbstractGPU-GeneratedPharo'
}

{ #category : #'definition' }
AGPUSwapChainCreateInfo class >> fieldsDesc [
	"
	self rebuildFieldAccessors
	"
    ^ #(
		 agpu_pointer display;
		 agpu_pointer window;
		 agpu_cstring window_system_name;
		 agpu_pointer surface;
		 agpu_texture_format colorbuffer_format;
		 agpu_texture_format depth_stencil_format;
		 agpu_uint width;
		 agpu_uint height;
		 agpu_uint buffer_count;
		 agpu_bool sample_buffers;
		 agpu_int samples;
		 agpu_swap_chain_flags flags;
		 agpu_swap_chain_presentation_mode presentation_mode;
		 agpu_swap_chain_presentation_mode fallback_presentation_mode;
		 agpu_int x;
		 agpu_int y;
		 agpu_swap_chain* old_swap_chain;
	)
]

