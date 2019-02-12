Class {
	#name : #AGPUSample,
	#superclass : #AGPUMorph,
	#instVars : [
		'commandQueue',
		'commandAllocator',
		'commandList',
		'mainRenderPass'
	],
	#category : #'AbstractGPU-Samples'
}

{ #category : #initialization }
AGPUSample >> createRenderPassWithColor: aColor [
	| colorAttachment depthStencilAttachment renderpassDescription |
	colorAttachment := nil.
	depthStencilAttachment := nil.
	
	[
		colorAttachment := AGPURenderpassColorAttachmentDescription externalNew.
		colorAttachment
			format: AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM;
			begin_action: AGPU_ATTACHMENT_CLEAR;
			end_action: AGPU_ATTACHMENT_KEEP;
			clear_value: aColor asAGPUColor4f.
		
		depthStencilAttachment := AGPURenderpassDepthStencilDescription externalNew.
		depthStencilAttachment
			format: AGPU_TEXTURE_FORMAT_D24_UNORM_S8_UINT;
			begin_action: AGPU_ATTACHMENT_CLEAR;
			end_action: AGPU_ATTACHMENT_KEEP;
			clear_value: (AGPUDepthStencilValue new depth: 1.0; stencil: 0; yourself).
		
		renderpassDescription := AGPURenderpassDescription new.
		renderpassDescription
			color_attachment_count: 1;
			color_attachments: colorAttachment;
			depth_stencil_attachment: depthStencilAttachment.
		^ device createRenderPass: renderpassDescription.
	] ensure: [
		colorAttachment ifNotNil: [ colorAttachment free ].
		depthStencilAttachment ifNotNil: [ depthStencilAttachment free ].
	]
]

{ #category : #initialization }
AGPUSample >> initializeForNewSession [
	super initializeForNewSession.
	
	mainRenderPass := self createRenderPassWithColor: Color blue.
	commandQueue := device getDefaultCommandQueue.
	commandAllocator := device createCommandAllocator: AGPU_COMMAND_LIST_TYPE_DIRECT queue: commandQueue.
	commandList := device createCommandList: AGPU_COMMAND_LIST_TYPE_DIRECT allocator: commandAllocator initial_pipeline_state: nil.
	commandList close.
	
]

{ #category : #initialization }
AGPUSample >> render [
	commandAllocator reset.
	commandList reset: commandAllocator initial_pipeline_state: nil.
	commandList beginRenderPass: mainRenderPass framebuffer: mainFramebuffer bundle_content: false asBit.
	
	commandList
		setViewport: 0 y: 0 w: screenWidth h: screenHeight;
		setScissor: 0 y: 0 w: screenWidth h: screenHeight;
		endRenderPass;
		close.
		
	commandQueue
		addCommandList: commandList;
		finishExecution.
]
