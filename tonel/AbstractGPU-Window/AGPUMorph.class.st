Class {
	#name : #AGPUMorph,
	#superclass : #BorderedMorph,
	#instVars : [
		'session',
		'platform',
		'device',
		'mainColorbuffer',
		'mainFramebuffer',
		'mainDepthbuffer',
		'mainFramebufferForm',
		'screenWidth',
		'screenHeight'
	],
	#pools : [
		'AGPUConstants'
	],
	#category : #'AbstractGPU-Window'
}

{ #category : #configurations }
AGPUMorph class >> debugLayerEnabled [
	^ true
]

{ #category : #configurations }
AGPUMorph class >> usesDepth [
	^ true
]

{ #category : #configurations }
AGPUMorph class >> usesStencil [
	^ true
]

{ #category : #initialization }
AGPUMorph >> blitTexture: texture toScreen: aCanvas [
	| pitch slicePitch |
	"texture readTextureLevel: 0 arrayIndex: 0 pitch: mainFramebufferForm width * 4 slicePitch: 0 data: mainFramebufferForm bits."
	pitch := mainFramebufferForm width * 4.
	slicePitch := pitch * mainFramebufferForm height.
	mainFramebufferForm unhibernate.
	
	texture readTextureData: 0 arrayIndex: 0 pitch: pitch slicePitch: slicePitch buffer: mainFramebufferForm bits.
	aCanvas translucentImage: mainFramebufferForm at: self bounds origin.
]

{ #category : #initialization }
AGPUMorph >> checkFramebuffer [
	| innerBounds currentWidth currentHeight |
	innerBounds := self innerBounds.
	currentWidth := innerBounds width asInteger.
	currentHeight := innerBounds height asInteger.
	(currentWidth ~= screenWidth or: [ currentHeight ~= screenHeight ]) ifTrue: [ 
		mainFramebuffer release.
		mainColorbuffer release.
		mainDepthbuffer ifNotNil: [
			mainDepthbuffer release.
		].
		mainFramebufferForm destroySurface.
	
		self createFramebuffer
	]
]

{ #category : #'session management' }
AGPUMorph >> checkSession [
	session ~~ Smalltalk session ifTrue: [
		self initializeForNewSession.
	].
]

{ #category : #initialization }
AGPUMorph >> createFramebuffer [
	| innerBounds colorBufferDesc hasDepth hasStencil depthStencilFormat depthStencilDesc mainColorbufferView mainDepthbufferView |
	innerBounds := self innerBounds.
	screenWidth := innerBounds width asInteger.
	screenHeight := innerBounds height asInteger.
	hasDepth := self class usesDepth.
	hasStencil := self class usesStencil.
	colorBufferDesc := AGPUTextureDescription new
		type: AGPU_TEXTURE_2D;
		width: screenWidth;
		height: screenHeight;
		depthOrArraySize: 1;
		miplevels: 1;
		format: AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM;
		flags: AGPU_TEXTURE_FLAG_RENDER_TARGET | AGPU_TEXTURE_FLAG_READED_BACK;
		sample_count: 1.
	mainColorbuffer := device createTexture: colorBufferDesc.
	
	mainColorbufferView := AGPUTextureViewDescription new.
	mainColorbuffer getFullViewDescription: mainColorbufferView.

	mainDepthbufferView := nil.
	(hasDepth or: [ hasStencil ]) ifTrue: [ 
		depthStencilFormat := hasStencil ifTrue: [ AGPU_TEXTURE_FORMAT_D24_UNORM_S8_UINT ] ifFalse: [ AGPU_TEXTURE_FORMAT_D32_FLOAT ].
		depthStencilDesc := AGPUTextureDescription new
			type: AGPU_TEXTURE_2D;
			width: screenWidth;
			height: screenHeight;
			depthOrArraySize: 1;
			miplevels: 1;
			format: depthStencilFormat;
			flags: AGPU_TEXTURE_FLAG_DEPTH | AGPU_TEXTURE_FLAG_RENDERBUFFER_ONLY;
			sample_count: 1.
		hasStencil ifTrue: [
			depthStencilDesc flags: depthStencilDesc flags | AGPU_TEXTURE_FLAG_STENCIL
		].

		mainDepthbuffer := device createTexture: depthStencilDesc.
		mainDepthbufferView := AGPUTextureViewDescription new.
		mainDepthbuffer getFullViewDescription: mainDepthbufferView.
	].

	mainFramebuffer := device createFrameBuffer: screenWidth height: screenHeight colorCount: 1 colorViews: mainColorbufferView depthStencilView: mainDepthbufferView.
	mainFramebufferForm := Form extent: screenWidth@screenHeight depth: 32.
		
]

{ #category : #initialization }
AGPUMorph >> drawOn: aCanvas [
	self checkSession.
	self checkFramebuffer.
	
	self render.
	
	self blitTexture: mainColorbuffer toScreen: aCanvas
]

{ #category : #initialization }
AGPUMorph >> initialize [
	super initialize.
	
	self extent: 640@480.
	self clipSubmorphs: true.
	self initializeForNewSession
	

]

{ #category : #initialization }
AGPUMorph >> initializeForNewSession [
	device := AGPU mainDevice.
	
	self createFramebuffer.

	session := Smalltalk session.
	
]

{ #category : #initialization }
AGPUMorph >> render [
	"TODO: Perform rendering in the main framebuffer"
]
