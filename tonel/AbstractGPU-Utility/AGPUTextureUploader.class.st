"
I am an utility that facilitates uploading textures into the GPU with automatic mipmap generation.
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
		'intermediateUploadTextureSize',
		'intermediateUploadTexture',
		'activeResources',
		'allocatedShaderResourceBindings',
		'allocatedShaderResourceBindingCount',
		'hasPendingOperation',
		'pendingOperationFence'
	],
	#pools : [
		'AGPUConstants'
	],
	#category : #'AbstractGPU-Utility'
}

{ #category : #'as yet unclassified' }
AGPUTextureUploader class >> defaultIntermediateUploadTextureSize [
	^ 512
]

{ #category : #'as yet unclassified' }
AGPUTextureUploader class >> defaultUploadBufferCapacity [
	^ self defaultIntermediateUploadTextureSize * self defaultIntermediateUploadTextureSize * 4
]

{ #category : #'instance creation' }
AGPUTextureUploader class >> for: device [
	^ self for: device utilityResourceCache: (AGPUUtilityResourceCache for: device)
]

{ #category : #'instance creation' }
AGPUTextureUploader class >> for: device utilityResourceCache: utilityResourceCache [
	^ self basicNew initializeWithDevice: device utilityResourceCache: utilityResourceCache
]

{ #category : #private }
AGPUTextureUploader >> allocateShaderResourceBinding [
	| result |
	allocatedShaderResourceBindingCount >= allocatedShaderResourceBindings size ifTrue: [ 
		| newBinding bindingSlotIndex |
		bindingSlotIndex := utilityResourceCache useComputeShaderForMipmapGeneration
			ifTrue: [ 0 ]
			ifFalse: [ 1 ].
		newBinding := utilityResourceCache mipmapGenerationShaderSignature createShaderResourceBinding: bindingSlotIndex.
		allocatedShaderResourceBindings add: newBinding.
	].
	allocatedShaderResourceBindingCount := allocatedShaderResourceBindingCount + 1.
	result := allocatedShaderResourceBindings at: allocatedShaderResourceBindingCount.
	^ result
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

{ #category : #copying }
AGPUTextureUploader >> copyMiplevels: levelCount withExtent: extent fromUploadTexture: uploadTexture usage: uploadTextureOldUsage into: texture [
	| currentWidth currentHeight usageRange level |
	currentWidth := extent x.
	currentHeight := extent y.

	usageRange := AGPUTextureSubresourceRange new
			aspect: AGPU_TEXTURE_ASPECT_COLOR;
			level_count: levelCount;
			layer_count: 1;
			yourself.
	commandList
		pushTextureTransitionBarrier: uploadTexture old_usage: uploadTextureOldUsage new_usage: AGPU_TEXTURE_USAGE_COPY_SOURCE subresource_range: usageRange;
		pushTextureTransitionBarrier: texture old_usage: AGPU_TEXTURE_USAGE_SAMPLED new_usage: AGPU_TEXTURE_USAGE_COPY_DESTINATION subresource_range: usageRange.
	level := AGPUTextureSubresourceLevel new
		aspect: AGPU_TEXTURE_ASPECT_COLOR;
		layer_count: 1;
		yourself.
	[
		commandList copyTexture: uploadTexture dest_texture: texture
			copy_region: (AGPUImageCopyRegion new
			source_usage_mode: AGPU_TEXTURE_USAGE_COPY_SOURCE;
			source_subresource_level: level;
			destination_usage_mode: AGPU_TEXTURE_USAGE_COPY_DESTINATION;
			destination_subresource_level: level;
			extent: (AGPUSize3d new
				width: currentWidth;
				height: currentHeight;
				depth: 1;
				yourself);
			yourself).
		
		(currentWidth <= 1 and: [ currentHeight <= 1 ]) ifTrue: [ ^ self ].
		
		level miplevel: level miplevel + 1.
		currentWidth := currentWidth // 2 max: 1.
		currentHeight := currentHeight // 2 max: 1.
	] repeat.

	commandList
		popTextureTransitionBarrier;
		popTextureTransitionBarrier
]

{ #category : #copying }
AGPUTextureUploader >> copyMiplevelsWithExtent: extent fromUploadTexture: uploadTexture usage: uploadTextureOldUsage into: texture [
	| currentWidth currentHeight |
	currentWidth := extent x.
	currentHeight := extent y.

	[
		
		(currentWidth <= 1 and: [ currentHeight <= 1 ]) ifTrue: [ ^ self ]
	] repeat
]

{ #category : #private }
AGPUTextureUploader >> countMiplevelsForExtent: anExtent [
	^ self countMiplevelsForWidth: anExtent x height: anExtent y
]

{ #category : #private }
AGPUTextureUploader >> countMiplevelsForSize: size [
	| currentSize result |
	currentSize := size.
	result := 1.
	[ currentSize > 1 ] whileTrue: [ 
		result := result + 1.
		currentSize := currentSize // 2 max: 1.
	].
	^ result
]

{ #category : #private }
AGPUTextureUploader >> countMiplevelsForWidth: width height: height [
	^ self countMiplevelsForSize: (width max: height)
]

{ #category : #API }
AGPUTextureUploader >> createAndUploadTextureForForm: aForm [
	| texture |
	texture := self createTextureForForm: aForm.
	self uploadForm: aForm intoTexture: texture.
	^ texture
]

{ #category : #API }
AGPUTextureUploader >> createAndUploadTextureForNonColorForm: aForm [
	| texture |
	texture := self createTextureForNonColorForm: aForm.
	self uploadNonColorForm: aForm intoTexture: texture.
	^ texture
]

{ #category : #API }
AGPUTextureUploader >> createAndUploadTextureForNormalMapForm: aForm [
	| texture |
	texture := self createTextureForNonColorForm: aForm.
	self uploadNormalMapForm: aForm intoTexture: texture.
	^ texture
]

{ #category : #API }
AGPUTextureUploader >> createColorTextureForExtent: extent depth: depth [
	^ self createTextureForExtent: extent format: AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM_SRGB
]

{ #category : #API }
AGPUTextureUploader >> createNonColorTextureForExtent: extent depth: depth [
	^ self createTextureForExtent: extent format: AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM
]

{ #category : #API }
AGPUTextureUploader >> createTextureForExtent: extent format: format [
	^ device createTexture: (AGPUTextureDescription new
		type: AGPU_TEXTURE_2D;
		width: extent x;
		height: extent y;
		depth: 1;
		layers: 1;
		miplevels: (self countMiplevelsForExtent: extent);
		sample_count: 1;
		format: format;
		heap_type: AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
		usage_modes: (AGPU_TEXTURE_USAGE_UPLOADED bitOr: AGPU_TEXTURE_USAGE_SAMPLED);
		main_usage_mode: AGPU_TEXTURE_USAGE_SAMPLED;
		yourself)
]

{ #category : #API }
AGPUTextureUploader >> createTextureForForm: aForm [
	^ self createColorTextureForExtent: aForm extent depth: 32
]

{ #category : #API }
AGPUTextureUploader >> createTextureForNonColorForm: aForm [
	^ self createNonColorTextureForExtent: aForm extent depth: 32
]

{ #category : #private }
AGPUTextureUploader >> ensureIntermediateMipmapGenerationTextureForExtent: requiredExtent [
	| requiredSize description |
	self assert: requiredExtent x isPowerOfTwo.
	self assert: requiredExtent y isPowerOfTwo.
	requiredSize := (requiredExtent x max: requiredExtent y) max: self class defaultIntermediateUploadTextureSize.
	(intermediateUploadTextureSize ~~ nil and: [ intermediateUploadTextureSize >= requiredSize ]) ifTrue: [ 
		^ intermediateUploadTexture
	].

	description := AGPUTextureDescription new
		type: AGPU_TEXTURE_2D;
		format: AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM;
		width: requiredSize;
		height: requiredSize;
		depth: 1;
		layers: 1;
		miplevels: (self countMiplevelsForSize: requiredSize);
		sample_count: 1;
		heap_type: AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
		usage_modes: (AGPU_TEXTURE_USAGE_COPY_SOURCE bitOr: AGPU_TEXTURE_USAGE_COPY_DESTINATION);
		yourself.
	utilityResourceCache useComputeShaderForMipmapGeneration ifTrue: [
		description main_usage_mode: AGPU_TEXTURE_USAGE_STORAGE
	] ifFalse: [
		description usage_modes: (description usage_modes bitOr: AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT).
		description main_usage_mode: AGPU_TEXTURE_USAGE_SAMPLED
	].

	description usage_modes: (description usage_modes bitOr: description main_usage_mode).
	
	intermediateUploadTexture := device createTexture: description.
	intermediateUploadTextureSize := requiredSize.
	^ intermediateUploadTexture
]

{ #category : #private }
AGPUTextureUploader >> ensureLastOperationHasFinished [
	hasPendingOperation ifTrue: [ 
		pendingOperationFence waitOnClient.
		hasPendingOperation := false.
	].
	activeResources := OrderedCollection new.
	allocatedShaderResourceBindingCount := 0.
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

{ #category : #private }
AGPUTextureUploader >> generateMipmapForExtent: extent usingComputePipelineState: mipmapComputationPipelineState [
	| shaderSignature currentWidth currentHeight currentViewDescription currentRange lastView levelIndex extents |
	shaderSignature := utilityResourceCache mipmapGenerationShaderSignature.
	commandList
		setShaderSignature: shaderSignature;
		usePipelineState: mipmapComputationPipelineState.
	currentWidth := extent x.
	currentHeight := extent y.
	
	currentViewDescription := AGPUTextureViewDescription new.
	intermediateUploadTexture getFullViewDescription: currentViewDescription.
	currentRange := AGPUTextureSubresourceRange new
		aspect: AGPU_TEXTURE_ASPECT_COLOR;
		layer_count: 1;
		level_count: 1.
	currentViewDescription subresource_range: currentRange.

	lastView := intermediateUploadTexture createView: currentViewDescription.
	activeResources add: lastView.
	levelIndex := 1.

	extents := WordArray new: 4.	
	[ currentWidth > 1 or: [ currentHeight > 1 ] ] whileTrue: [
		| binding nextWidth nextHeight currentView |
		nextWidth := currentWidth // 2 max: 1.
		nextHeight := currentHeight // 2 max: 1.
		
		currentRange base_miplevel: levelIndex.
		currentViewDescription subresource_range: currentRange.
		currentView := intermediateUploadTexture createView: currentViewDescription.
		activeResources add: currentView.

		binding := self allocateShaderResourceBinding.
		binding
			bindStorageImageView: 0 view: lastView;
			bindStorageImageView: 1 view: currentView.
			
		extents
			at: 1 put: currentWidth;
			at: 2 put: currentHeight;
			at: 3 put: nextWidth;
			at: 4 put: nextHeight.
			
		commandList
			useComputeShaderResources: binding;
			pushConstants: 0 size: 16 values: extents;
			dispatchCompute: (currentWidth + 15) // 16 group_count_y: (currentHeight + 15) // 16 group_count_z: 1;
			textureMemoryBarrier: intermediateUploadTexture
				source_stage: AGPU_PIPELINE_STAGE_COMPUTE_SHADER
				dest_stage: (AGPU_PIPELINE_STAGE_COMPUTE_SHADER bitOr: AGPU_PIPELINE_STAGE_FRAGMENT_SHADER)
				source_accesses: AGPU_ACCESS_SHADER_WRITE dest_accesses: AGPU_ACCESS_SHADER_READ
				old_usage: AGPU_TEXTURE_USAGE_STORAGE new_usage: AGPU_TEXTURE_USAGE_STORAGE subresource_range: currentRange.
		currentWidth := nextWidth.
		currentHeight := nextHeight.
		lastView := currentView. 
		levelIndex := levelIndex + 1.
	]
]

{ #category : #private }
AGPUTextureUploader >> generateMipmapForExtent: extent usingGraphicsPipelineState: mipmapComputationPipelineState [
	| shaderSignature renderpass viewDescription currentRange currentWidth currentHeight quickConstants currentIntermediateTextureSize samplerBinding |
	shaderSignature := utilityResourceCache mipmapGenerationShaderSignature.
	renderpass := utilityResourceCache mipmapGenerationRenderpass.
	
	viewDescription := AGPUTextureViewDescription new.
	intermediateUploadTexture getFullViewDescription: viewDescription.
	currentRange := AGPUTextureSubresourceRange new
		aspect: AGPU_TEXTURE_ASPECT_COLOR;
		layer_count: 1;
		level_count: 1.
	viewDescription subresource_range: currentRange.

	commandList setShaderSignature: shaderSignature.

	currentWidth := extent x.
	currentHeight := extent y.
	
	quickConstants := WordArray new: 6.
	samplerBinding := utilityResourceCache mipmapGenerationSamplerBinding.

	currentIntermediateTextureSize := intermediateUploadTextureSize.
	[ currentWidth > 1 or: [ currentHeight > 1 ] ] whileTrue: [ 
		| nextWidth nextHeight nextIntermediateTextureSize sourceView destViewDescription destView framebuffer binding |
		nextWidth := currentWidth // 2.
		nextHeight := currentHeight // 2.
		nextIntermediateTextureSize := currentIntermediateTextureSize // 2 max: 1.

		viewDescription subresource_range: currentRange.
		sourceView := intermediateUploadTexture createView: viewDescription.
		activeResources add: sourceView.

		currentRange base_miplevel: currentRange base_miplevel + 1.
		
		destViewDescription := AGPUTextureViewDescription new.
		intermediateUploadTexture getFullViewDescription: destViewDescription.
		destViewDescription
			usage_mode: AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT;
			subresource_range: currentRange.
		destView := intermediateUploadTexture createView: destViewDescription.
		activeResources add: destView.

		framebuffer := device createFrameBuffer: nextIntermediateTextureSize height: nextIntermediateTextureSize colorCount: 1 colorViews: (AGPU packListOfReferences: {destView}) depthStencilView: nil.
		activeResources add: framebuffer.
		
		binding := self allocateShaderResourceBinding.
		binding bindSampledTextureView: 0 view: sourceView.
		
		quickConstants
			at: 1 put: currentWidth; 
			at: 2 put: currentHeight; 
			at: 3 put: currentIntermediateTextureSize;
			at: 4 put: currentIntermediateTextureSize;
			at: 5 put: nextWidth; 
			at: 6 put: nextHeight.
			
		commandList
			beginRenderPass: renderpass framebuffer: framebuffer bundle_content: 0;
			setViewport: 0 y: 0 w: nextWidth h: nextHeight;
			setScissor: 0 y: 0 w: nextWidth h: nextHeight;
			usePipelineState: mipmapComputationPipelineState;
			useShaderResources: samplerBinding;
			useShaderResources: binding;
			pushConstants: 0 size: 24 values: quickConstants;
			drawArrays: 3 instance_count: 1 first_vertex: 0 base_instance: 0;
			endRenderPass.
			
		currentWidth := nextWidth.
		currentHeight := nextHeight.
		currentIntermediateTextureSize := nextIntermediateTextureSize.
	].
]

{ #category : #initialization }
AGPUTextureUploader >> initializeWithDevice: aDevice utilityResourceCache: anUtilityResourceCache [
	device := aDevice.
	utilityResourceCache := anUtilityResourceCache.
	allocatedShaderResourceBindings := OrderedCollection new.
	hasPendingOperation := false.
]

{ #category : #asserting }
AGPUTextureUploader >> shouldGenerateMipmapsForExtent: anExtent [
	^ anExtent x > 1 and: [ anExtent x isPowerOfTwo and: [
		anExtent y > 1 and: [ anExtent y isPowerOfTwo ]]]
	
]

{ #category : #API }
AGPUTextureUploader >> uploadForm: form intoTexture: texture [
	^ self uploadForm: form intoTexture: texture computeMipmapsWith: utilityResourceCache colorMipmapComputationPipelineState
]

{ #category : #private }
AGPUTextureUploader >> uploadForm: form intoTexture: texture computeMipmapsWith: mipmapComputationPipelineState [
	| convertedForm pitchAlignment pitch slicePitch mappedBuffer destRowIndex destRowAdvance sourceRowLength sourceIndex sourceBits shouldGenerateMipmaps uploadTexture uploadTextureOldUsage levelCount |
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
	levelCount := self countMiplevelsForExtent: form extent.
	
	shouldGenerateMipmaps := self shouldGenerateMipmapsForExtent: form extent.
	shouldGenerateMipmaps ifTrue: [
		uploadTexture := self ensureIntermediateMipmapGenerationTextureForExtent: form extent.
		uploadTextureOldUsage := utilityResourceCache useComputeShaderForMipmapGeneration
			ifTrue: [AGPU_TEXTURE_USAGE_STORAGE]
			ifFalse: [AGPU_TEXTURE_USAGE_SAMPLED]
	] ifFalse: [
		uploadTexture := texture.
		uploadTextureOldUsage := AGPU_TEXTURE_USAGE_SAMPLED
	].

	activeResources
		add: texture;
		add: uploadTexture;
		add: uploadBuffer.
	self beginCommandList
		pushTextureTransitionBarrier: uploadTexture old_usage: uploadTextureOldUsage new_usage: AGPU_TEXTURE_USAGE_COPY_DESTINATION subresource_range: (AGPUTextureSubresourceRange new
			aspect: AGPU_TEXTURE_ASPECT_COLOR;
			level_count: 1;
			layer_count: 1;
			yourself);
		copyBufferToTexture: uploadBuffer texture: uploadTexture copy_region: (AGPUBufferImageCopyRegion new
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
		
	shouldGenerateMipmaps ifTrue: [
		utilityResourceCache useComputeShaderForMipmapGeneration ifTrue: [ 
			self generateMipmapForExtent: form extent usingComputePipelineState: mipmapComputationPipelineState
		] ifFalse: [ 
			self generateMipmapForExtent: form extent usingGraphicsPipelineState: mipmapComputationPipelineState
		]
	].
		
	uploadTexture ~~ texture ifTrue: [
		self copyMiplevels: levelCount withExtent: form extent fromUploadTexture: uploadTexture usage: uploadTextureOldUsage into: texture
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
AGPUTextureUploader >> uploadNonColorForm: form intoTexture: texture [
	^ self uploadForm: form intoTexture: texture computeMipmapsWith: utilityResourceCache nonColorMipmapComputationPipelineState
]

{ #category : #API }
AGPUTextureUploader >> uploadNormalMapForm: form intoTexture: texture [
	^ self uploadForm: form intoTexture: texture computeMipmapsWith: utilityResourceCache normalMapMipmapComputationPipelineState
]
