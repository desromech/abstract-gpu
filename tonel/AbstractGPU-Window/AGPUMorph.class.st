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
		'screenHeight',
		'perFrameSynchronization',
		'stateTracker'
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
	perFrameSynchronization synchronizeForFrameReadback: [ :readbackFrameIndex |
		pitch := mainFramebufferForm width * 4.
		slicePitch := pitch * mainFramebufferForm height.
		mainFramebufferForm unhibernate.

		texture readTextureData: 0 arrayIndex: 0 pitch: pitch slicePitch: slicePitch buffer: mainFramebufferForm bits.
		aCanvas translucentImage: mainFramebufferForm at: self bounds origin.
	].

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

		self createFramebuffer
	]
]

{ #category : #'session management' }
AGPUMorph >> checkSession [
	session ~~ AGPU session ifTrue: [
		self initializeForNewSession.
	].
]

{ #category : #initialization }
AGPUMorph >> colorAttachmentFormat [
	^ AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM_SRGB
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
		depth: 1;
		layers: 1;
		miplevels: 1;
		format: self colorAttachmentFormat;
		usage_modes: (AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT bitOr: AGPU_TEXTURE_USAGE_READED_BACK);
		main_usage_mode: AGPU_TEXTURE_USAGE_READED_BACK;
		sample_count: 1.
	mainColorbuffer := device createTexture: colorBufferDesc.

	mainColorbufferView := mainColorbuffer getOrCreateFullView.

	mainDepthbufferView := nil.
	(hasDepth or: [ hasStencil ]) ifTrue: [
		depthStencilFormat := self depthStencilFormat.
		depthStencilDesc := AGPUTextureDescription new
			type: AGPU_TEXTURE_2D;
			width: screenWidth;
			height: screenHeight;
			depth: 1;
			layers: 1;
			miplevels: 1;
			format: depthStencilFormat;
			usage_modes: AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT;
			main_usage_mode: AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT;
			sample_count: 1.
		hasStencil ifTrue: [
			depthStencilDesc
				usage_modes: (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT bitOr: AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT);
				main_usage_mode: (AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT bitOr: AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT)
		].

		mainDepthbuffer := device createTexture: depthStencilDesc.
		mainDepthbufferView := mainDepthbuffer getOrCreateFullView.
	].

	mainFramebuffer := device createFrameBuffer: screenWidth height: screenHeight colorCount: 1 colorViews: (AGPU packListOfReferences: {mainColorbufferView}) depthStencilView: mainDepthbufferView.
	mainFramebufferForm := Form extent: screenWidth@screenHeight depth: 32.

]

{ #category : #initialization }
AGPUMorph >> createRenderPassWithColor: aColor [
	| colorAttachment depthStencilAttachment renderpassDescription |
	colorAttachment := nil.
	depthStencilAttachment := nil.

	[
		colorAttachment := AGPURenderpassColorAttachmentDescription externalNew.
		colorAttachment
			format: self colorAttachmentFormat;
			begin_action: AGPU_ATTACHMENT_CLEAR;
			end_action: AGPU_ATTACHMENT_KEEP;
			clear_value: aColor asAGPUColor4f;
			sample_count: 1;
			sample_quality: 0.

		depthStencilAttachment := AGPURenderpassDepthStencilDescription externalNew.
		depthStencilAttachment
			format: self depthStencilFormat;
			begin_action: AGPU_ATTACHMENT_CLEAR;
			end_action: AGPU_ATTACHMENT_KEEP;
			stencil_begin_action: AGPU_ATTACHMENT_CLEAR;
			stencil_end_action: AGPU_ATTACHMENT_KEEP;
			clear_value: (AGPUDepthStencilValue new depth: 1.0; stencil: 0; yourself);
			sample_count: 1;
			sample_quality: 0.

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
AGPUMorph >> depthStencilFormat [
	^ self hasStencil
		ifTrue: [ AGPU_TEXTURE_FORMAT_D32_FLOAT_S8X24_UINT ]
		ifFalse: [ AGPU_TEXTURE_FORMAT_D32_FLOAT ]
]

{ #category : #initialization }
AGPUMorph >> drawOn: aCanvas [
	self checkSession.
	self checkFramebuffer.

	perFrameSynchronization renderFrameWith: [ :aStateTracker |
		stateTracker := aStateTracker.
		self render.
	].

	self blitTexture: mainColorbuffer toScreen: aCanvas
]

{ #category : #initialization }
AGPUMorph >> hasDepth [
	^ self class usesDepth
]

{ #category : #initialization }
AGPUMorph >> hasStencil [
	^ self class usesStencil
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

	perFrameSynchronization := AGPUImplicitFrameSynchronization for: device.
	self createFramebuffer.

	session := AGPU session.

]

{ #category : #'as yet unclassified' }
AGPUMorph >> mainImmediateRenderer [
	^ perFrameSynchronization immediateRendererForPass: #main
]

{ #category : #initialization }
AGPUMorph >> render [
	"TODO: Perform rendering in the main framebuffer"
]
