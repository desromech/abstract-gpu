Class {
	#name : #AGPUSample1,
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
AGPUSample1 >> initializeForNewSession [
	super initializeForNewSession.
	
	mainRenderPass := self createRenderPassWithColor: Color blue.
	commandQueue := device getDefaultCommandQueue.
	commandAllocator := device createCommandAllocator: AGPU_COMMAND_LIST_TYPE_DIRECT queue: commandQueue.
	commandList := device createCommandList: AGPU_COMMAND_LIST_TYPE_DIRECT allocator: commandAllocator initial_pipeline_state: nil.
	commandList close.
	
]

{ #category : #initialization }
AGPUSample1 >> render [
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
