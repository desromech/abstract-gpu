Class {
	#name : #AGPUSwapChainCreateInfo,
	#superclass : #FFIExternalStructure,
	#classVars : [
		'OFFSET_BUFFER_COUNT',
		'OFFSET_COLORBUFFER_FORMAT',
		'OFFSET_DEPTH_STENCIL_FORMAT',
		'OFFSET_DISPLAY',
		'OFFSET_FALLBACK_PRESENTATION_MODE',
		'OFFSET_FLAGS',
		'OFFSET_HEIGHT',
		'OFFSET_LAYERS',
		'OFFSET_OLD_SWAP_CHAIN',
		'OFFSET_PRESENTATION_MODE',
		'OFFSET_SAMPLES',
		'OFFSET_SAMPLE_BUFFERS',
		'OFFSET_SURFACE',
		'OFFSET_WIDTH',
		'OFFSET_WINDOW',
		'OFFSET_WINDOW_SYSTEM_NAME',
		'OFFSET_X',
		'OFFSET_Y'
	],
	#pools : [
		'AGPUConstants',
		'AGPUTypes'
	],
	#category : #'AbstractGPU-GeneratedPharo'
}

{ #category : #definition }
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
		 agpu_uint layers;
		 agpu_uint buffer_count;
		 agpu_bool sample_buffers;
		 agpu_int samples;
		 agpu_swap_chain_flags flags;
		 agpu_swap_chain* old_swap_chain;
		 agpu_swap_chain_presentation_mode presentation_mode;
		 agpu_swap_chain_presentation_mode fallback_presentation_mode;
		 agpu_int x;
		 agpu_int y;
	)
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> buffer_count [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_BUFFER_COUNT
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> buffer_count: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_BUFFER_COUNT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> colorbuffer_format [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_COLORBUFFER_FORMAT
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> colorbuffer_format: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_COLORBUFFER_FORMAT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> depth_stencil_format [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_DEPTH_STENCIL_FORMAT
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> depth_stencil_format: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_DEPTH_STENCIL_FORMAT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> display [
	"This method was automatically generated"
	^ExternalData fromHandle: (handle pointerAt: OFFSET_DISPLAY) type: ExternalType void asPointerType
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> display: anObject [
	"This method was automatically generated"
	handle pointerAt: OFFSET_DISPLAY put: anObject getHandle.
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> fallback_presentation_mode [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_FALLBACK_PRESENTATION_MODE
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> fallback_presentation_mode: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_FALLBACK_PRESENTATION_MODE put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> flags [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_FLAGS
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> flags: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_FLAGS put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> height [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_HEIGHT
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> height: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_HEIGHT put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> layers [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_LAYERS
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> layers: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_LAYERS put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> old_swap_chain [
	"This method was automatically generated"
	^ExternalData fromHandle: (handle pointerAt: OFFSET_OLD_SWAP_CHAIN) type: ExternalType void asPointerType
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> old_swap_chain: anObject [
	"This method was automatically generated"
	handle pointerAt: OFFSET_OLD_SWAP_CHAIN put: anObject getHandle.
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> presentation_mode [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_PRESENTATION_MODE
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> presentation_mode: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_PRESENTATION_MODE put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> sample_buffers [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_SAMPLE_BUFFERS
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> sample_buffers: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_SAMPLE_BUFFERS put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> samples [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_SAMPLES
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> samples: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_SAMPLES put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> surface [
	"This method was automatically generated"
	^ExternalData fromHandle: (handle pointerAt: OFFSET_SURFACE) type: ExternalType void asPointerType
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> surface: anObject [
	"This method was automatically generated"
	handle pointerAt: OFFSET_SURFACE put: anObject getHandle.
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> width [
	"This method was automatically generated"
	^handle unsignedLongAt: OFFSET_WIDTH
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> width: anObject [
	"This method was automatically generated"
	handle unsignedLongAt: OFFSET_WIDTH put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> window [
	"This method was automatically generated"
	^ExternalData fromHandle: (handle pointerAt: OFFSET_WINDOW) type: ExternalType void asPointerType
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> window: anObject [
	"This method was automatically generated"
	handle pointerAt: OFFSET_WINDOW put: anObject getHandle.
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> window_system_name [
	"This method was automatically generated"
	^ExternalData fromHandle: (handle pointerAt: OFFSET_WINDOW_SYSTEM_NAME) type: ExternalType char asPointerType
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> window_system_name: anObject [
	"This method was automatically generated"
	handle pointerAt: OFFSET_WINDOW_SYSTEM_NAME put: anObject getHandle.
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> x [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_X
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> x: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_X put: anObject
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> y [
	"This method was automatically generated"
	^handle signedLongAt: OFFSET_Y
]

{ #category : #'accessing - structure variables' }
AGPUSwapChainCreateInfo >> y: anObject [
	"This method was automatically generated"
	handle signedLongAt: OFFSET_Y put: anObject
]
