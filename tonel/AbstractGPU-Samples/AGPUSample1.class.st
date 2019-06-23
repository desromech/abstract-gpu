Class {
	#name : #AGPUSample1,
	#superclass : #AGPUMorph,
	#instVars : [
		'mainRenderPass'
	],
	#category : #'AbstractGPU-Samples'
}

{ #category : #initialization }
AGPUSample1 >> initializeForNewSession [
	super initializeForNewSession.
	
	mainRenderPass := self createRenderPassWithColor: Color magenta.
]

{ #category : #initialization }
AGPUSample1 >> render [
	stateTracker
		beginRenderPass: mainRenderPass framebuffer: mainFramebuffer bundle_content: 0;
		setViewport: 0 y: 0 w: screenWidth h: screenHeight;
		setScissor: 0 y: 0 w: screenWidth h: screenHeight;
		endRenderPass

]
