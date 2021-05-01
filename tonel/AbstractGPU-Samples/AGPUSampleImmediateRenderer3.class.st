Class {
	#name : #AGPUSampleImmediateRenderer3,
	#superclass : #AGPUSampleImmediateRendererCommon,
	#instVars : [
		'texture',
		'textureUploader',
		'cubeMesh',
		'cameraPositionX',
		'cameraPositionY',
		'cameraPositionZ',
		'dragStartPosition',
		'cameraRotationX',
		'cameraRotationY'
	],
	#category : #'AbstractGPU-Samples'
}

{ #category : #'event handling' }
AGPUSampleImmediateRenderer3 >> handlesMouseDown: evt [
	^ true
]

{ #category : #'event handling' }
AGPUSampleImmediateRenderer3 >> handlesMouseWheel: evt [
	^ true
]

{ #category : #initialization }
AGPUSampleImmediateRenderer3 >> initialize [
	super initialize.
	cameraRotationX := 0.0.
	cameraRotationY := 0.0.
	
	cameraPositionX := 0.0.
	cameraPositionY := 0.0.
	cameraPositionZ := 5.0.
]

{ #category : #initialization }
AGPUSampleImmediateRenderer3 >> initializeForNewSession [
	super initializeForNewSession.

	textureUploader := AGPUTextureUploader for: device.
	texture := textureUploader createAndUploadTextureForForm: self checkboardTextureForm.
	cubeMesh := (AGPUSampleMesh for: device) addCubeMinX: -1.0 y: -1.0 z: -1.0 maxX: 1.0 y: 1.0 z: 1.0.
]

{ #category : #'event handling' }
AGPUSampleImmediateRenderer3 >> mouseDown: aMorphicEvent [
	dragStartPosition := aMorphicEvent position
]

{ #category : #'event handling' }
AGPUSampleImmediateRenderer3 >> mouseMove: event [
	| delta |
	dragStartPosition ifNil: [ ^ self ].
	event isDraggingEvent ifFalse: [ ^ self ].
	
	delta := (event position - dragStartPosition) * 0.01.
	dragStartPosition := event position.
	event redButtonPressed ifTrue: [
		cameraRotationY := cameraRotationY + (delta x * 10.0).
		cameraRotationX := cameraRotationX + (delta y * 10.0).
	].
	event yellowButtonPressed ifTrue: [
		cameraPositionX := cameraPositionX + delta x.
		cameraPositionY := cameraPositionY - delta y.
	].

	self changed.
]

{ #category : #'event handling' }
AGPUSampleImmediateRenderer3 >> mouseUp: aMorphicEvent [
	dragStartPosition := nil
]

{ #category : #'event handling' }
AGPUSampleImmediateRenderer3 >> mouseWheel: event [
	event isUp ifTrue: [ 
		cameraPositionZ := cameraPositionZ - 0.2
	].
	event isDown ifTrue: [ 
		cameraPositionZ := cameraPositionZ + 0.2
	].
	self changed.
]

{ #category : #initialization }
AGPUSampleImmediateRenderer3 >> render [
	| aspectRatio immediateRenderer |
	aspectRatio := screenWidth / screenHeight.
	stateTracker
		beginRenderPass: mainRenderPass framebuffer: mainFramebuffer bundle_content: 0;
		setViewport: 0 y: 0 w: screenWidth h: screenHeight;
		setScissor: 0 y: 0 w: screenWidth h: screenHeight.
			
	immediateRenderer := self mainImmediateRenderer.
	immediateRenderer
		beginRendering: stateTracker;
		
		setCullMode: AGPU_CULL_MODE_BACK;
		setDepthState: 1 writeMask: 1 function: AGPU_LESS_EQUAL;

		projectionMatrixMode;
		loadIdentity;
		perspective: 60.0 aspect: aspectRatio near: 0.01 far: 100.0;

		modelViewMatrixMode;
		loadIdentity;
		translate: cameraPositionX negated y: cameraPositionY negated z: cameraPositionZ negated;
		rotate: cameraRotationX x: 1.0 y: 0.0 z: 0.0;
		rotate: cameraRotationY x: 0.0 y: 1.0 z: 0.0;
		
		setTexturingEnabled: true;
		setSamplingMode: AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST maxAnisotropy: 0 addressU: AGPU_TEXTURE_ADDRESS_MODE_WRAP addressV: AGPU_TEXTURE_ADDRESS_MODE_WRAP addressW: AGPU_TEXTURE_ADDRESS_MODE_WRAP;
		bindTexture: texture.
	
	cubeMesh
		beginDrawingUsingExplicitBuffers: false on: immediateRenderer;
		drawUsingExplicitBuffers: false withImmediateRenderer: immediateRenderer;
		endDrawingUsingExplicitBuffers: false on: immediateRenderer.

	immediateRenderer
		endRendering.
		
	stateTracker
		endRenderPass

]
