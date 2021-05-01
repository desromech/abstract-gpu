Class {
	#name : #AGPUSampleImmediateRenderer4,
	#superclass : #AGPUSampleImmediateRendererCommon,
	#instVars : [
		'texture',
		'textureUploader',
		'cubeMesh',
		'cameraPositionX',
		'cameraPositionY',
		'cameraPositionZ',
		'dragStartPosition',
		'normalTexture',
		'currentLightingModel'
	],
	#category : #'AbstractGPU-Samples'
}

{ #category : #initialization }
AGPUSampleImmediateRenderer4 >> drawCubeMeshMaterialRowUsingExplicitBuffers: useExplicitBuffers on: immediateRenderer [
	| gap baseColors currentX specular |
	gap := 2.0.
	
	baseColors := {
		Color r: 0.8 g: 0.1 b: 0.1.
		Color r: 0.1 g: 0.8 b: 0.1.
		Color r: 0.1 g: 0.1 b: 0.8.

		Color r: 0.8 g: 0.8 b: 0.8.

		Color r: 0.8 g: 0.8 b: 0.1.
		Color r: 0.1 g: 0.8 b: 0.8.
		Color r: 0.8 g: 0.1 b: 0.8.
	}.
	
	currentX := 	baseColors size * gap negated / 2.0.
	specular := AGPUVector4f new x: 0.04; y: 0.04; z: 0.04; w: 1.0; yourself.
	baseColors do: [ :baseColor |
		| material |
		self hasMetallicRoughnessLightingModel ifTrue: [ 
			material := AGPUImmediateRendererMaterial new 
				metallic_roughness: (AGPUImmediateRendererMaterialMetallicRoughness new
					base_color: baseColor asAGPUColor4f;
					roughness_factor: 0.4;
					metallic_factor: 0.0;
					occlusion_factor: 1.0;
					yourself);
				yourself
		] ifFalse: [
			self hasFlatColorLightingModel ifTrue: [ 
				material := AGPUImmediateRendererMaterial new 
					flat_color: (AGPUImmediateRendererMaterialFlatColor new
						color: baseColor asAGPUColor4f;
						yourself);
					yourself
			] ifFalse: [
				| diffuseFactor |
				diffuseFactor := 1.0 - 0.04.
				material := AGPUImmediateRendererMaterial new 
					classic: (AGPUImmediateRendererMaterialClassic new
						ambient: baseColor asAGPUColor4f;
						diffuse: (AGPUVector4f new
							x: baseColor red*diffuseFactor;
							y: baseColor green*diffuseFactor;
							z: baseColor blue*diffuseFactor;
							w: baseColor alpha;
							yourself);
						specular: specular;
						shininess: 50.0;
						yourself);
					yourself
			].
		].

		immediateRenderer
			pushMatrix;
			setMaterial: material;
			translate: currentX y: 0.0 z: 0.0.
		cubeMesh drawWithImmediateRenderer: immediateRenderer.
		immediateRenderer popMatrix.
		
		currentX := currentX + gap
	].

		
]

{ #category : #initialization }
AGPUSampleImmediateRenderer4 >> drawCubeMeshRowUsingExplicitBuffers: useExplicitBuffers on: immediateRenderer [
	"Bottom unlighted."
	immediateRenderer
		pushMatrix;
		translate: 0.0 y: -2.0 z: 0.0;
		setLightingEnabled: false.
	self drawCubeMeshMaterialRowUsingExplicitBuffers: useExplicitBuffers on: immediateRenderer.
	immediateRenderer
		popMatrix.
	
	"Top lighted"
	immediateRenderer
		pushMatrix;
		setLightingEnabled: true.
	self drawCubeMeshMaterialRowUsingExplicitBuffers: useExplicitBuffers on: immediateRenderer.
	immediateRenderer
		popMatrix.
	
]

{ #category : #initialization }
AGPUSampleImmediateRenderer4 >> drawCubesUsingExplicitBuffers: useExplicitBuffers on: immediateRenderer [

	"First row: without texture"
	immediateRenderer
		setTexturingEnabled: false;
		pushMatrix;
		setLightingEnabled: false.	
	self drawCubeMeshRowUsingExplicitBuffers: useExplicitBuffers on: immediateRenderer.
	immediateRenderer popMatrix.

	"Second row: with textures"
	immediateRenderer
		setTexturingEnabled: true;
		bindTextureIn: texture binding: AGPU_IMMEDIATE_RENDERER_TEXTURE_BINDING_ALBEDO;
		pushMatrix;
		translate: 0.0 y: 0.0 z: -2.0;
		setLightingEnabled: false.
	self drawCubeMeshRowUsingExplicitBuffers: useExplicitBuffers on: immediateRenderer.
	immediateRenderer popMatrix.		
		
	"Third row: without textures and fog."
	immediateRenderer
		setTexturingEnabled: false;
		setFogMode: AGPU_IMMEDIATE_RENDERER_FOG_MODE_LINEAR;
		setFogColor: 0.0 g: 0.0 b: 0.0 a: 0.0;
		setFogDistances: 0.0 end: 10.0;	
		pushMatrix;
		translate: 0.0 y: 0.0 z: -4.0;
		setLightingEnabled: false.	
	self drawCubeMeshRowUsingExplicitBuffers: useExplicitBuffers on: immediateRenderer.
	immediateRenderer popMatrix.

	"Fourth row: with textures and fog."
	immediateRenderer
		setTexturingEnabled: true;
		pushMatrix;
		translate: 0.0 y: 0.0 z: -6.0;
		setLightingEnabled: false.	
	self drawCubeMeshRowUsingExplicitBuffers: useExplicitBuffers on: immediateRenderer.
	immediateRenderer popMatrix.

	"Fifth row: without textures and fog."
	immediateRenderer
		setTexturingEnabled: false;
		setFogMode: AGPU_IMMEDIATE_RENDERER_FOG_MODE_EXPONENTIAL;
		setFogDensity: 0.1;
		pushMatrix;
		translate: 0.0 y: 0.0 z: -8.0;
		setLightingEnabled: false.	
	self drawCubeMeshRowUsingExplicitBuffers: useExplicitBuffers on: immediateRenderer.
	immediateRenderer popMatrix.

	"Sixth row: with textures and fog."
	immediateRenderer
		setTexturingEnabled: true;
		pushMatrix;
		translate: 0.0 y: 0.0 z: -10.0;
		setLightingEnabled: false.	
	self drawCubeMeshRowUsingExplicitBuffers: useExplicitBuffers on: immediateRenderer.
	immediateRenderer popMatrix.

	"Seventh row: without textures and fog."
	immediateRenderer
		setTexturingEnabled: false;
		setFogMode: AGPU_IMMEDIATE_RENDERER_FOG_MODE_EXPONENTIAL_SQUARED;
		setFogDensity: 0.1;
		pushMatrix;
		translate: 0.0 y: 0.0 z: -12.0;
		setLightingEnabled: false.	
	self drawCubeMeshRowUsingExplicitBuffers: useExplicitBuffers on: immediateRenderer.
	immediateRenderer popMatrix.

	"Eighth row: with textures and fog."
	immediateRenderer
		setTexturingEnabled: true;
		pushMatrix;
		translate: 0.0 y: 0.0 z: -14.0;
		setLightingEnabled: false.	
		self drawCubeMeshRowUsingExplicitBuffers: useExplicitBuffers on: immediateRenderer.
	immediateRenderer popMatrix.

	immediateRenderer setFogMode: AGPU_IMMEDIATE_RENDERER_FOG_MODE_NONE
]

{ #category : #'event handling' }
AGPUSampleImmediateRenderer4 >> handlesKeyboard: anEvent [
	^ anEvent wasHandled not
]

{ #category : #'event handling' }
AGPUSampleImmediateRenderer4 >> handlesMouseDown: evt [
	^ true
]

{ #category : #'event handling' }
AGPUSampleImmediateRenderer4 >> handlesMouseWheel: evt [
	^ true
]

{ #category : #testing }
AGPUSampleImmediateRenderer4 >> hasFlatColorLightingModel [
	^ currentLightingModel = AGPU_IMMEDIATE_RENDERER_LIGHTING_MODEL_FLAT_COLOR
]

{ #category : #testing }
AGPUSampleImmediateRenderer4 >> hasMetallicRoughnessLightingModel [
	^ currentLightingModel = AGPU_IMMEDIATE_RENDERER_LIGHTING_MODEL_METALLIC_ROUGHNESS
]

{ #category : #initialization }
AGPUSampleImmediateRenderer4 >> initialize [
	super initialize.
	cameraPositionX := 0.0.
	cameraPositionY := 0.0.
	cameraPositionZ := 3.0.
	currentLightingModel := AGPU_IMMEDIATE_RENDERER_LIGHTING_MODEL_PER_VERTEX.
]

{ #category : #initialization }
AGPUSampleImmediateRenderer4 >> initializeForNewSession [
	super initializeForNewSession.

	textureUploader := AGPUTextureUploader for: device.
	texture := textureUploader createAndUploadTextureForForm: self checkboardTextureForm.
	normalTexture := textureUploader createAndUploadTextureForNormalMapForm: self checkboardNormalMapTextureForm.
	cubeMesh := AGPUSampleMesh new addCubeMinX: -0.5 y: -0.5 z: -0.5 maxX: 0.5 y: 0.5 z: 0.5.
]

{ #category : #'event handling' }
AGPUSampleImmediateRenderer4 >> keyDown: anEvent [
	anEvent keyCharacter = Character tab ifTrue: [ self useNextLightingModel. ^ true ].
	^ false
]

{ #category : #'event handling' }
AGPUSampleImmediateRenderer4 >> mouseDown: aMorphicEvent [
	self takeKeyboardFocus.
	dragStartPosition := aMorphicEvent position
]

{ #category : #'event handling' }
AGPUSampleImmediateRenderer4 >> mouseMove: event [
	| delta |
	dragStartPosition ifNil: [ ^ self ].
	event isDraggingEvent ifFalse: [ ^ self ].
	
	delta := (event position - dragStartPosition) * 0.01.
	dragStartPosition := event position.
	cameraPositionX := cameraPositionX + delta x.
	cameraPositionY := cameraPositionY - delta y.
	self changed.
]

{ #category : #'event handling' }
AGPUSampleImmediateRenderer4 >> mouseUp: aMorphicEvent [
	dragStartPosition := nil
]

{ #category : #'event handling' }
AGPUSampleImmediateRenderer4 >> mouseWheel: event [
	event isUp ifTrue: [ 
		cameraPositionZ := cameraPositionZ - 0.2
	].
	event isDown ifTrue: [ 
		cameraPositionZ := cameraPositionZ + 0.2
	].
	self changed.
]

{ #category : #initialization }
AGPUSampleImmediateRenderer4 >> render [
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
		translate: cameraPositionX negated y: cameraPositionY negated z: cameraPositionZ negated.
		
	self setupLightingOn: immediateRenderer.
	immediateRenderer
		setSamplingMode: AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR maxAnisotropy: 0 addressU: AGPU_TEXTURE_ADDRESS_MODE_WRAP addressV: AGPU_TEXTURE_ADDRESS_MODE_WRAP addressW: AGPU_TEXTURE_ADDRESS_MODE_WRAP.
	
	"Draw cubes with implicit rendering."
	immediateRenderer pushMatrix.
	self drawCubesUsingExplicitBuffers: false on: immediateRenderer.
	immediateRenderer popMatrix.

	"Draw cubes with explicit buffers."
	immediateRenderer
		pushMatrix;
		translate: 0.0 y: 4.0 z: 0.0.
	self drawCubesUsingExplicitBuffers: true on: immediateRenderer.
	immediateRenderer popMatrix.

	immediateRenderer
		endRendering.
		
	stateTracker
		endRenderPass

]

{ #category : #initialization }
AGPUSampleImmediateRenderer4 >> setupLightingOn: immediateRenderer [
	| lightDirection lightPosition intensity |
	immediateRenderer setLightingModel: currentLightingModel.
	self hasFlatColorLightingModel ifTrue: [
		^ self
	].

	lightDirection := AGPUVector4f new
		x: 0.3483665071458088;
		y: 0.6270597128624559;
		z: -0.6967330142916176;
		w: 0.0;
		yourself.	
	lightPosition := AGPUVector4f new
		x: 0.0;
		y: 0.0;
		z: -6.0;
		w: 1.0;
		yourself.	
			
	self hasMetallicRoughnessLightingModel ifTrue: [
		intensity := AGPUVector3f new x: 0.8; y: 0.8; z: 0.8; yourself.
	
		immediateRenderer setLightingModel: AGPU_IMMEDIATE_RENDERER_LIGHTING_MODEL_METALLIC_ROUGHNESS.
		immediateRenderer setLight: 0 enabled: true state:
			(AGPUImmediateRendererLight new pbr: (AGPUImmediateRendererLightPbr new
				intensity: intensity;
				position: lightDirection;
				spot_cutoff: 180.0;
				spot_inner_cutoff: 180.0;
				radius: 1.0;
				yourself); yourself).

		intensity := AGPUVector3f new x: 2.0; y: 2.0; z: 2.0; yourself.
		immediateRenderer setLight: 1 enabled: true state:
			(AGPUImmediateRendererLight new pbr: (AGPUImmediateRendererLightPbr new
				intensity: intensity;
				position: lightPosition;
				spot_cutoff: 180.0;
				spot_inner_cutoff: 180.0;
				radius: 100.0;
				yourself); yourself).
		^ self
	].

	intensity := AGPUVector4f new x: 0.8; y: 0.8; z: 0.8; w: 1.0; yourself.
	immediateRenderer setLight: 0 enabled: true state:
			(AGPUImmediateRendererLight new pbr: (AGPUImmediateRendererLightClassic new
				diffuse: intensity;
				specular: intensity;
				position: lightDirection;
				constant_attenuation: 1.0;
				spot_cutoff: 180.0;
				yourself); yourself).
				
	intensity := AGPUVector4f new x: 2.0; y: 2.0; z: 2.0; w: 1.0; yourself.
	immediateRenderer setLight: 1 enabled: true state:
			(AGPUImmediateRendererLight new pbr: (AGPUImmediateRendererLightClassic new
				diffuse: intensity;
				specular: intensity;
				position: lightDirection;
				constant_attenuation: 1.0;
				quadratic_attenuation: 0.01;
				spot_cutoff: 180.0;
				yourself); yourself).

]

{ #category : #actions }
AGPUSampleImmediateRenderer4 >> useNextLightingModel [
	currentLightingModel := (currentLightingModel + 1) % AGPU_IMMEDIATE_RENDERER_LIGHTING_MODEL_COUNT.
	self changed
]
