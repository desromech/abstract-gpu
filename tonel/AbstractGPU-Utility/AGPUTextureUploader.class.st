"
I am an utility that facilitates uploading textures into the GPU.
"
Class {
	#name : #AGPUTextureUploader,
	#superclass : #Object,
	#instVars : [
		'device',
		'uploadBuffer',
		'uploadBufferCapacity',
		'utilityResourceCache',
		'commandList',
		'commandQueue',
		'commandListAllocator',
		'activeResources',
		'hasPendingOperation',
		'pendingOperationFence',
		'mipmapGenerator',
		'lightProbePreprocessor'
	],
	#pools : [
		'AGPUConstants'
	],
	#category : #'AbstractGPU-Utility'
}

{ #category : #'as yet unclassified' }
AGPUTextureUploader class >> defaultUploadBufferCapacity [
	^ AGPUTextureMipmapGenerator defaultIntermediateTextureSize * AGPUTextureMipmapGenerator defaultIntermediateTextureSize * 4
]

{ #category : #'instance creation' }
AGPUTextureUploader class >> for: device [
	^ self for: device utilityResourceCache: (AGPUUtilityResourceCache for: device)
]

{ #category : #'instance creation' }
AGPUTextureUploader class >> for: device utilityResourceCache: utilityResourceCache [
	^ self for: device utilityResourceCache: utilityResourceCache mipmapGenerator: (AGPUTextureMipmapGenerator for: device utilityResourceCache: utilityResourceCache)
]

{ #category : #'instance creation' }
AGPUTextureUploader class >> for: device utilityResourceCache: utilityResourceCache mipmapGenerator: mipmapGenerator [
	^ self basicNew initializeWithDevice: device utilityResourceCache: utilityResourceCache mipmapGenerator: mipmapGenerator lightProbePreprocessor: (AGPUTextureLightprobePreprocessor for: device utilityResourceCache: utilityResourceCache)
]

{ #category : #'instance creation' }
AGPUTextureUploader class >> for: device utilityResourceCache: utilityResourceCache mipmapGenerator: mipmapGenerator lightprobePreprocessor: lightprobePreprocessor [
	^ self basicNew initializeWithDevice: device utilityResourceCache: utilityResourceCache mipmapGenerator: mipmapGenerator lightProbePreprocessor: lightprobePreprocessor
]

{ #category : #private }
AGPUTextureUploader >> beginCommandList [
	commandList ifNil: [ 
		commandQueue := device getDefaultCommandQueue.
		commandListAllocator := device createCommandAllocator: AGPU_COMMAND_LIST_TYPE_DIRECT queue: commandQueue.
		commandList := device createCommandList: AGPU_COMMAND_LIST_TYPE_DIRECT allocator: commandListAllocator initial_pipeline_state: nil.
	] ifNotNil: [
		commandListAllocator reset.
		commandList reset: commandListAllocator initial_pipeline_state: nil
	].
	^ commandList
]

{ #category : #API }
AGPUTextureUploader >> createAndUploadLightProbeForHDRI: hdri [
	^ self createAndUploadLightProbeForHDRI: hdri
		environmentMapSize: AGPUTextureLightprobePreprocessor defaultEnvironmentMapSize
		diffuseLightProbeSize: AGPUTextureLightprobePreprocessor defaultDiffuseLightProbeSize
		specularLightProbeSize: AGPUTextureLightprobePreprocessor defaultSpecularLightProbeSize
]

{ #category : #API }
AGPUTextureUploader >> createAndUploadLightProbeForHDRI: hdri environmentMapSize: environmentMapSize diffuseLightProbeSize: diffuseLightProbeSize specularLightProbeSize: specularLightProbeSize [
	| hdriTexture lightProbe |
	hdriTexture := self createTextureForExtent: hdri extent format: AGPU_TEXTURE_FORMAT_R8G8B8A8_UNORM useMipmaps: false.
	lightProbe := AGPULightProbeTextures new
		environmentMapSize: environmentMapSize;
		environmentMap: (self createHDRCubeTextureForSize: environmentMapSize useMipmaps: false);
		
		diffuseLightProbeSize: diffuseLightProbeSize;
		diffuseLightProbe: (self createHDRCubeTextureForSize: diffuseLightProbeSize useMipmaps: false);
		
		specularLightProbeSize: specularLightProbeSize;
		specularLightProbe: (self createHDRCubeTextureForSize: specularLightProbeSize useMipmaps: true).
	
	self uploadHDRI: hdri intoTexture: hdriTexture thenProcessIntoLightProbe: lightProbe.
	^ lightProbe
]

{ #category : #API }
AGPUTextureUploader >> createAndUploadTextureForForm: aForm [
	^ self createAndUploadTextureForForm: aForm repeated: true quality: 1
]

{ #category : #API }
AGPUTextureUploader >> createAndUploadTextureForForm: aForm repeated: repeated quality: quality [
	| texture |
	texture := self createTextureForForm: aForm.
	self uploadForm: aForm intoTexture: texture repeated: repeated quality: quality.
	^ texture
]

{ #category : #API }
AGPUTextureUploader >> createAndUploadTextureForHDRI: hdri [
	| texture |
	texture := self createTextureForExtent: hdri extent format: AGPU_TEXTURE_FORMAT_R8G8B8A8_UNORM useMipmaps: false.
	self uploadHDRI: hdri intoTexture: texture.
	^ texture
]

{ #category : #API }
AGPUTextureUploader >> createAndUploadTextureForNonColorForm: aForm [
	^ self createAndUploadTextureForNonColorForm: aForm repeated: true quality: 1
]

{ #category : #API }
AGPUTextureUploader >> createAndUploadTextureForNonColorForm: aForm repeated: repeated quality: quality [
	| texture |
	texture := self createTextureForNonColorForm: aForm.
	self uploadNonColorForm: aForm intoTexture: texture repeated: repeated quality: quality.
	^ texture
]

{ #category : #API }
AGPUTextureUploader >> createAndUploadTextureForNormalMapForm: aForm [
	^ self createAndUploadTextureForNormalMapForm: aForm repeated: true quality: 1
]

{ #category : #API }
AGPUTextureUploader >> createAndUploadTextureForNormalMapForm: aForm repeated: repeated quality: quality [
	| texture |
	texture := self createTextureForNonColorForm: aForm.
	self uploadNormalMapForm: aForm intoTexture: texture repeated: repeated quality: quality.
	^ texture
]

{ #category : #API }
AGPUTextureUploader >> createColorCubeTextureForSize: extent useMipmaps: useMipmaps [
	^ self createCubeTextureForSize: extent format: AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM_SRGB useMipmaps: useMipmaps
]

{ #category : #API }
AGPUTextureUploader >> createColorTextureForExtent: extent depth: depth [
	^ self createTextureForExtent: extent format: AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM_SRGB
]

{ #category : #API }
AGPUTextureUploader >> createCubeTextureForSize: size format: format useMipmaps: useMipmaps [
	^ device createTexture: (AGPUTextureDescription new
		type: AGPU_TEXTURE_CUBE;
		width: size;
		height: size;
		depth: 1;
		layers: 6;
		miplevels: (useMipmaps
			ifTrue: [mipmapGenerator countMiplevelsForWidth: size height: size]
			ifFalse: [1]);
		sample_count: 1;
		format: format;
		heap_type: AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
		usage_modes: ((AGPU_TEXTURE_USAGE_COPY_DESTINATION bitOr: AGPU_TEXTURE_USAGE_COPY_SOURCE) bitOr: AGPU_TEXTURE_USAGE_SAMPLED);
		main_usage_mode: AGPU_TEXTURE_USAGE_SAMPLED;
		yourself)
]

{ #category : #API }
AGPUTextureUploader >> createHDRCubeTextureForSize: extent useMipmaps: useMipmaps [
	^ self createCubeTextureForSize: extent format: AGPU_TEXTURE_FORMAT_R16G16B16A16_FLOAT useMipmaps: useMipmaps
]

{ #category : #API }
AGPUTextureUploader >> createNonColorCubeTextureForSize: extent useMipmaps: useMipmaps [
	^ self createCubeTextureForSize: extent format: AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM useMipmaps: useMipmaps
]

{ #category : #API }
AGPUTextureUploader >> createNonColorTextureForExtent: extent depth: depth [
	^ self createTextureForExtent: extent format: AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM
]

{ #category : #API }
AGPUTextureUploader >> createTextureForExtent: extent format: format [
	^ self createTextureForExtent: extent format: format useMipmaps: true
]

{ #category : #API }
AGPUTextureUploader >> createTextureForExtent: extent format: format useMipmaps: useMipmaps [
	^ device createTexture: (AGPUTextureDescription new
		type: AGPU_TEXTURE_2D;
		width: extent x;
		height: extent y;
		depth: 1;
		layers: 1;
		miplevels: (useMipmaps
			ifTrue: [mipmapGenerator countMiplevelsForExtent: extent]
			ifFalse: [1]);
		sample_count: 1;
		format: format;
		heap_type: AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
		usage_modes: ((AGPU_TEXTURE_USAGE_COPY_DESTINATION bitOr: AGPU_TEXTURE_USAGE_COPY_SOURCE) bitOr: AGPU_TEXTURE_USAGE_SAMPLED);
		main_usage_mode: AGPU_TEXTURE_USAGE_SAMPLED;
		yourself)
]

{ #category : #API }
AGPUTextureUploader >> createTextureForForm: aForm [
	^ self createColorTextureForExtent: aForm extent depth: 32
]

{ #category : #API }
AGPUTextureUploader >> createTextureForHDRI: hdri [
	^ self createTextureForExtent: hdri extent format: AGPU_TEXTURE_FORMAT_R8G8B8A8_UNORM useMipmaps: false
]

{ #category : #API }
AGPUTextureUploader >> createTextureForNonColorForm: aForm [
	^ self createNonColorTextureForExtent: aForm extent depth: 32
]

{ #category : #private }
AGPUTextureUploader >> ensureLastOperationHasFinished [
	hasPendingOperation ifTrue: [ 
		pendingOperationFence waitOnClient.
		hasPendingOperation := false.
	].
	activeResources := OrderedCollection new.
	mipmapGenerator reset.
	lightProbePreprocessor reset.
]

{ #category : #private }
AGPUTextureUploader >> ensureUploadBufferWithCapacityFor: requiredCapacity [
	| alignedRequiredCapacity |
	alignedRequiredCapacity := (requiredCapacity alignedTo: (device getLimitValue: AGPU_LIMIT_MIN_TEXTURE_DATA_OFFSET_ALIGNMENT)) max: self class defaultUploadBufferCapacity.
	(uploadBufferCapacity ~~ nil and: [ uploadBufferCapacity >= alignedRequiredCapacity ]) ifTrue: [
		^ self
	].

	uploadBuffer := device createBuffer: ( AGPUBufferDescription new
		size: alignedRequiredCapacity;
		heap_type: AGPU_MEMORY_HEAP_TYPE_HOST_TO_DEVICE;
		usage_modes: AGPU_COPY_SOURCE_BUFFER;
		main_usage_mode: AGPU_COPY_SOURCE_BUFFER;
		mapping_flags: AGPU_MAP_WRITE_BIT) initial_data: nil.
	uploadBufferCapacity := alignedRequiredCapacity.
]

{ #category : #initialization }
AGPUTextureUploader >> initializeWithDevice: aDevice utilityResourceCache: anUtilityResourceCache mipmapGenerator: aMipmapGenerator lightProbePreprocessor: aLightprobePreprocessor [
	device := aDevice.
	utilityResourceCache := anUtilityResourceCache.
	mipmapGenerator := aMipmapGenerator.
	lightProbePreprocessor := aLightprobePreprocessor.
	hasPendingOperation := false.
]

{ #category : #accessing }
AGPUTextureUploader >> mipmapGenerator [
	^ mipmapGenerator
]

{ #category : #API }
AGPUTextureUploader >> uploadForm: form intoTexture: texture [
	^ self uploadForm: form intoTexture: texture repeated: true quality: 1
]

{ #category : #private }
AGPUTextureUploader >> uploadForm: form intoTexture: texture computeMipmapsWith: mipmapComputationPipelineState repeated: repeated quality: quality [
	self uploadForm: form intoTexture: texture extent: form extent computeMipmapsWith: mipmapComputationPipelineState repeated: repeated quality: quality
]

{ #category : #private }
AGPUTextureUploader >> uploadForm: form intoTexture: texture extent: textureExtent computeMipmapsWith: mipmapComputationPipelineState repeated: repeated quality: quality [
	| convertedForm pitchAlignment pitch slicePitch mappedBuffer destRowIndex destRowAdvance sourceRowLength sourceIndex sourceBits |
	self ensureLastOperationHasFinished.
	convertedForm := form asFormOfDepth: 32.
	pitchAlignment := device getLimitValue: AGPU_LIMIT_MIN_TEXTURE_DATA_PITCH_ALIGNMENT.
	pitch := convertedForm width * 4 alignedTo: pitchAlignment.
	slicePitch := pitch * convertedForm height.
	self ensureUploadBufferWithCapacityFor: slicePitch.
	
	mappedBuffer := (uploadBuffer mapBuffer: AGPU_WRITE_ONLY) getHandle.
	
	"We need to flip the form."
	destRowIndex := 1.
	destRowAdvance := pitch.
	
	"Do we need to flip the texture vertically?"
	device hasBottomLeftTextureCoordinates ~~ 0 ifTrue: [ 
		destRowIndex := 1 + (pitch * (convertedForm height - 1)).
		destRowAdvance := pitch negated.
	].
	
	sourceRowLength := form width.
	sourceIndex := 1.
	sourceBits := form bits.
	1 to: form height do: [ :y |
		| destIndex |
		destIndex := destRowIndex.
		1 to: sourceRowLength do: [ :x |
			mappedBuffer unsignedLongAt: destIndex put: (sourceBits at: sourceIndex).
			sourceIndex := sourceIndex + 1.
			destIndex := destIndex + 4
		].
	
		destRowIndex := destRowIndex + destRowAdvance
	].

	uploadBuffer unmapBuffer.

	activeResources
		add: texture;
		add: uploadBuffer.
	self beginCommandList
		pushTextureTransitionBarrier: texture old_usage: AGPU_TEXTURE_USAGE_SAMPLED new_usage: AGPU_TEXTURE_USAGE_COPY_DESTINATION subresource_range: (AGPUTextureSubresourceRange new
			aspect: AGPU_TEXTURE_ASPECT_COLOR;
			level_count: 1;
			layer_count: 1;
			yourself);
		copyBufferToTexture: uploadBuffer texture: texture copy_region: (AGPUBufferImageCopyRegion new
			buffer_pitch: pitch;
			buffer_slice_pitch: slicePitch;
			texture_usage_mode: AGPU_TEXTURE_USAGE_COPY_DESTINATION;
			texture_subresource_level: (AGPUTextureSubresourceLevel new
				aspect: AGPU_TEXTURE_ASPECT_COLOR;
				layer_count: 1
				yourself);
			texture_region: (AGPURegion3d new
				width: form width;
				height: form height;
				depth: 1;
				yourself);
			yourself);
		popTextureTransitionBarrier.
		
	(mipmapComputationPipelineState isNotNil and: [ mipmapGenerator shouldGenerateMipmapsForExtent: textureExtent ]) ifTrue: [
		mipmapGenerator emitMipmapGenerationCommandsWith: mipmapComputationPipelineState on: commandList retainResourcesOn: activeResources forTexture: texture extent: textureExtent repeated: repeated  sampleFromSuccessiveLevels: quality <= 1
	].
		
	commandList
		close.
	pendingOperationFence ifNil: [ pendingOperationFence := device createFence ].
	commandQueue
		addCommandList: commandList;
		signalFence: pendingOperationFence.
	hasPendingOperation := true

]

{ #category : #API }
AGPUTextureUploader >> uploadForm: form intoTexture: texture repeated: repeated quality: quality [
	^ self uploadForm: form intoTexture: texture computeMipmapsWith: (mipmapGenerator colorComputationPipelineStateWithQuality: quality) repeated: repeated quality: quality
]

{ #category : #API }
AGPUTextureUploader >> uploadHDRI: hdri intoTexture: texture [
	^ self uploadHDRI: hdri intoTexture: texture thenProcessWith: [:aCommandList :aListOfRetainedResources | ]
]

{ #category : #API }
AGPUTextureUploader >> uploadHDRI: hdri intoTexture: hdriTexture thenProcessIntoLightProbe: lightProbe [
	^ self uploadHDRI: hdri intoTexture: hdriTexture thenProcessWith: [:aCommandList :aListOfRetainedResources |
		
		lightProbePreprocessor
			emitCommandsForConvertingHDRI: hdriTexture getOrCreateFullView extent: hdri width @ hdri height intoCubeMap: lightProbe environmentMap size: lightProbe environmentMapSize on: aCommandList retainResourcesOn: aListOfRetainedResources;
			emitCommandsForGeneratingDiffuseLightProbeFrom: lightProbe environmentMap getOrCreateFullView intoCubeMap: lightProbe diffuseLightProbe size: lightProbe diffuseLightProbeSize on: aCommandList retainResourcesOn: aListOfRetainedResources;
			emitCommandsForGeneratingSpecularLightProbeFrom: lightProbe environmentMap getOrCreateFullView intoCubeMap: lightProbe specularLightProbe size: lightProbe specularLightProbeSize on: aCommandList retainResourcesOn: aListOfRetainedResources
	]
]

{ #category : #private }
AGPUTextureUploader >> uploadHDRI: hdri intoTexture: texture thenProcessWith: processBlock [

	| pitchAlignment pitch slicePitch mappedBuffer destRowIndex destRowAdvance sourceRowLength sourceRowIndex sourcePixels |
	self ensureLastOperationHasFinished.
	pitchAlignment := device getLimitValue: AGPU_LIMIT_MIN_TEXTURE_DATA_PITCH_ALIGNMENT.
	pitch := hdri pitch alignedTo: pitchAlignment.
	slicePitch := pitch * hdri height.
	self ensureUploadBufferWithCapacityFor: slicePitch.
	
	mappedBuffer := (uploadBuffer mapBuffer: AGPU_WRITE_ONLY) getHandle.
	
	"We need to flip the form."
	destRowIndex := 1.
	destRowAdvance := pitch.
	
	"Do we need to flip the texture vertically?"
	device hasBottomLeftTextureCoordinates ~= 0 ifTrue: [ 
		destRowIndex := 1 + (pitch * (hdri height - 1)).
		destRowAdvance := pitch negated.
	].
	
	sourceRowLength := hdri pitch.
	sourceRowIndex := 1.
	sourcePixels := hdri pixels.
	1 to: hdri height do: [ :y |
		mappedBuffer replaceFrom: destRowIndex to: destRowIndex + sourceRowLength - 1 with: sourcePixels startingAt: sourceRowIndex.
	
		destRowIndex := destRowIndex + destRowAdvance.
		sourceRowIndex := sourceRowIndex + sourceRowLength.
	].

	uploadBuffer unmapBuffer.

	activeResources
		add: texture;
		add: uploadBuffer.

	self beginCommandList
		pushTextureTransitionBarrier: texture old_usage: AGPU_TEXTURE_USAGE_SAMPLED new_usage: AGPU_TEXTURE_USAGE_COPY_DESTINATION subresource_range: (AGPUTextureSubresourceRange new
			aspect: AGPU_TEXTURE_ASPECT_COLOR;
			level_count: 1;
			layer_count: 1;
			yourself);
		copyBufferToTexture: uploadBuffer texture: texture copy_region: (AGPUBufferImageCopyRegion new
			buffer_pitch: pitch;
			buffer_slice_pitch: slicePitch;
			texture_usage_mode: AGPU_TEXTURE_USAGE_COPY_DESTINATION;
			texture_subresource_level: (AGPUTextureSubresourceLevel new
				aspect: AGPU_TEXTURE_ASPECT_COLOR;
				layer_count: 1
				yourself);
			texture_region: (AGPURegion3d new
				width: hdri width;
				height: hdri height;
				depth: 1;
				yourself);
			yourself);
		popTextureTransitionBarrier.
		
	processBlock value: commandList value: activeResources.

	commandList
		close.
	pendingOperationFence ifNil: [ pendingOperationFence := device createFence ].
	commandQueue
		addCommandList: commandList;
		signalFence: pendingOperationFence.
	hasPendingOperation := true

]

{ #category : #API }
AGPUTextureUploader >> uploadNonColorForm: form intoTexture: texture [
	^ self uploadNonColorForm: form intoTexture: texture repeated: true quality: 1
]

{ #category : #API }
AGPUTextureUploader >> uploadNonColorForm: form intoTexture: texture repeated: repeated quality: quality [
	^ self uploadForm: form intoTexture: texture computeMipmapsWith: (mipmapGenerator nonColorComputationPipelineStateWithQuality: quality) repeated: repeated quality: quality
]

{ #category : #API }
AGPUTextureUploader >> uploadNormalMapForm: form intoTexture: texture [
	^ self uploadNormalMapForm: form intoTexture: texture repeated: true quality: 1
]

{ #category : #API }
AGPUTextureUploader >> uploadNormalMapForm: form intoTexture: texture repeated: repeated quality: quality [
	^ self uploadForm: form intoTexture: texture computeMipmapsWith: (mipmapGenerator normalMapComputationPipelineStateWithQuality: quality) repeated: repeated quality: quality
]

{ #category : #accessing }
AGPUTextureUploader >> utilityResourceCache [
	^ utilityResourceCache
]
