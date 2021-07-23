Class {
	#name : #AGPUSampleImmediateRenderer2,
	#superclass : #AGPUSampleImmediateRendererCommon,
	#instVars : [
		'texture',
		'textureUploader'
	],
	#category : #'AbstractGPU-Samples'
}

{ #category : #initialization }
AGPUSampleImmediateRenderer2 >> initializeForNewSession [
	super initializeForNewSession.

	textureUploader := AGPUTextureUploader for: device.
	texture := textureUploader createAndUploadTextureForForm: self checkboardTextureForm repeated: true quality: 1.

]

{ #category : #initialization }
AGPUSampleImmediateRenderer2 >> render [
	| aspectRatio w h |
	aspectRatio := (screenWidth / screenHeight) asFloat.
	stateTracker
		beginRenderPass: mainRenderPass framebuffer: mainFramebuffer bundle_content: 0;
		setViewport: 0 y: 0 w: screenWidth h: screenHeight;
		setScissor: 0 y: 0 w: screenWidth h: screenHeight.
		
	h := 1.0.
	w := h * aspectRatio.
	
	self mainImmediateRenderer
		beginRendering: stateTracker;
		projectionMatrixMode;
		loadIdentity;
		ortho: w negated right: w
			bottom: h negated top: h
			near: -1.0 far: 1.0;

		modelViewMatrixMode;
		loadIdentity;
		
		setTexturingEnabled: true;
		bindTexture: texture;

		beginPrimitives: AGPU_TRIANGLES;
		color: 1.0 g: 1.0 b: 1.0 a: 1.0; 

		texcoord: 0.0 y: 0.0; vertex: -0.5 y: -0.5 z: 0.0;
		texcoord: 1.0 y: 0.0; vertex: 0.5 y: -0.5 z: 0.0;
		texcoord: 1.0 y: 1.0; vertex: 0.5 y: 0.5 z: 0.0;

		texcoord: 1.0 y: 1.0; vertex: 0.5 y: 0.5 z: 0.0;
		texcoord: 0.0 y: 1.0; vertex: -0.5 y: 0.5 z: 0.0;
		texcoord: 0.0 y: 0.0; vertex: -0.5 y: -0.5 z: 0.0;

		endPrimitives;
		endRendering.
		
	stateTracker
		endRenderPass

]
