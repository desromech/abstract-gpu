Class {
	#name : #AGPUSample3,
	#superclass : #AGPUMorph,
	#instVars : [
		'mainRenderPass',
		'texture'
	],
	#category : #'AbstractGPU-Samples'
}

{ #category : #initialization }
AGPUSample3 >> initializeForNewSession [
	| textureDescription form |
	super initializeForNewSession.

	form := self textureForm.
	self assert: form depth = 32.
	
	mainRenderPass := self createRenderPassWithColor: Color black.
	textureDescription := AGPUTextureDescription new
		type: AGPU_TEXTURE_2D;
		width: form width;
		height: form height;
		depth: 1;
		layers: 1;
		miplevels: 1;
		sample_count: 1;
		format: AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM;
		heap_type: AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
		usage_modes: (AGPU_TEXTURE_USAGE_UPLOADED bitOr: AGPU_TEXTURE_USAGE_SAMPLED);
		yourself.
	texture := device createTexture: textureDescription.

	form unhibernate.
	texture uploadTextureData: 0 arrayIndex: 0 pitch: form width*4 slicePitch: form width* form height*4 data: form bits
]

{ #category : #initialization }
AGPUSample3 >> render [
	| aspectRatio w h |
	aspectRatio := screenWidth / screenHeight.
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

{ #category : #initialization }
AGPUSample3 >> textureForm [
	| form |
	form := Form extent: 32@32 depth: 32.
	form getCanvas
		fillRectangle: (0@0 extent: 32@32) color: Color white;
		fillRectangle: (0@0 extent: 16@16) color: Color black;
		fillRectangle: (16@16 extent: 16@16) color: Color black.
	^ form
]
