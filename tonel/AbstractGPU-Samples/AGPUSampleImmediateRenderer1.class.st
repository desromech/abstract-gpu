Class {
	#name : #AGPUSampleImmediateRenderer1,
	#superclass : #AGPUSampleImmediateRendererCommon,
	#instVars : [
		'rotation'
	],
	#category : #'AbstractGPU-Samples'
}

{ #category : #initialization }
AGPUSampleImmediateRenderer1 >> initialize [
	rotation := 0.0.
	super initialize.
	self startStepping
]

{ #category : #initialization }
AGPUSampleImmediateRenderer1 >> render [
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
		rotate: rotation x: 0.0 y: 1.0 z: 0.0;

		beginPrimitives: AGPU_TRIANGLES;
		color: 1.0 g: 0.0 b: 0.0 a: 1.0; 
		vertex: -0.5 y: -0.5 z: 0.0;
		color: 0.0 g: 1.0 b: 0.0 a: 1.0; 
		vertex: 0.5 y: -0.5 z: 0.0;
		color: 0.0 g: 0.0 b: 1.0 a: 1.0; 
		vertex: 0.0 y: 0.5 z: 0.0;
		endPrimitives;
		endRendering.
		
	stateTracker
		endRenderPass

]

{ #category : #initialization }
AGPUSampleImmediateRenderer1 >> step [
	| delta |
	delta := self stepTime * 0.001.
	rotation := rotation + (180.0 *delta).
	self changed.
]

{ #category : #initialization }
AGPUSampleImmediateRenderer1 >> stepTime [
	^ 1000 / 25
]
