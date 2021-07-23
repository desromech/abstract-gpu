"
I am an utility for generating texture mipmaps on the gpu.
"
Class {
	#name : #AGPUTextureMipmapGenerator,
	#superclass : #Object,
	#instVars : [
		'device',
		'utilityResourceCache',
		'allocatedShaderResourceBindings',
		'allocatedShaderResourceBindingCount',
		'intermediateTextureSize',
		'intermediateCopyBufferPitchAlignment',
		'intermediateCopyBufferPitch',
		'intermediateCopyBuffer',
		'intermediateTexture',
		'intermediateTextureView',
		'intermediateTextureFramebuffer'
	],
	#pools : [
		'AGPUConstants'
	],
	#category : #'AbstractGPU-Utility'
}

{ #category : #'as yet unclassified' }
AGPUTextureMipmapGenerator class >> boxFilterMipmapQuality [
	^ 1
]

{ #category : #'as yet unclassified' }
AGPUTextureMipmapGenerator class >> defaultIntermediateTextureSize [
	^ 512
]

{ #category : #'instance creation' }
AGPUTextureMipmapGenerator class >> for: device [
	^ self for: device utilityResourceCache: (AGPUUtilityResourceCache for: device)
]

{ #category : #'instance creation' }
AGPUTextureMipmapGenerator class >> for: device utilityResourceCache: utilityResourceCache [
	^ self basicNew initializeWithDevice: device utilityResourceCache: utilityResourceCache
]

{ #category : #'as yet unclassified' }
AGPUTextureMipmapGenerator class >> noMipmapQuality [
	^ 0
]

{ #category : #private }
AGPUTextureMipmapGenerator >> allocateShaderResourceBinding [
	| result |
	allocatedShaderResourceBindingCount >= allocatedShaderResourceBindings size ifTrue: [ 
		| newBinding |
		newBinding := utilityResourceCache mipmapGenerationShaderSignature createShaderResourceBinding: 1.
		allocatedShaderResourceBindings add: newBinding.
	].
	allocatedShaderResourceBindingCount := allocatedShaderResourceBindingCount + 1.
	result := allocatedShaderResourceBindings at: allocatedShaderResourceBindingCount.
	^ result
]

{ #category : #accessing }
AGPUTextureMipmapGenerator >> colorComputationPipelineStateWithQuality: quality [
	quality = 1 ifTrue: [ ^ utilityResourceCache colorMipmapComputationPipelineState ].
	^ nil
]

{ #category : #private }
AGPUTextureMipmapGenerator >> copyFrom: sourceTexture usageMode: sourceUsageMode into: destinationTexture usageMode: destinationUsageMode level: levelIndex width: width height: height on: commandList [
	| copyRegion pitch slicePitch |
	copyRegion := AGPURegion3d new
			width: width;
			height: height;
			depth: 1;
			yourself.
	pitch := width * 4 alignedTo: intermediateCopyBufferPitchAlignment.
	slicePitch := pitch * height.

	commandList
		pushTextureTransitionBarrier: sourceTexture old_usage: sourceUsageMode new_usage: AGPU_TEXTURE_USAGE_COPY_SOURCE
			subresource_range: (AGPUTextureSubresourceRange new
				aspect: AGPU_TEXTURE_ASPECT_COLOR;
				layer_count: 1;
				level_count: 1;
				yourself);
		pushTextureTransitionBarrier: destinationTexture old_usage: destinationUsageMode new_usage: AGPU_TEXTURE_USAGE_COPY_DESTINATION
			subresource_range: (AGPUTextureSubresourceRange new
				aspect: AGPU_TEXTURE_ASPECT_COLOR;
				layer_count: 1;
				base_miplevel: levelIndex;
				level_count: 1;
				yourself);

		copyTextureToBuffer: sourceTexture buffer: intermediateCopyBuffer 
			copy_region: (AGPUBufferImageCopyRegion new
				buffer_pitch: pitch;
				buffer_slice_pitch: slicePitch;
				texture_region: copyRegion;
				texture_subresource_level: (AGPUTextureSubresourceLevel new
					aspect: AGPU_TEXTURE_ASPECT_COLOR;
					layer_count: 1;
					yourself) ;
				texture_usage_mode: AGPU_TEXTURE_USAGE_COPY_SOURCE;
				yourself);
				
		memoryBarrier: 	AGPU_PIPELINE_STAGE_TRANSFER dest_stage: AGPU_PIPELINE_STAGE_TRANSFER
				source_accesses: AGPU_ACCESS_TRANSFER_WRITE dest_accesses: AGPU_ACCESS_TRANSFER_READ;

		copyBufferToTexture: intermediateCopyBuffer texture: destinationTexture copy_region:(AGPUBufferImageCopyRegion new
				buffer_pitch: pitch;
				buffer_slice_pitch: slicePitch;
				texture_region: copyRegion;
				texture_subresource_level: (AGPUTextureSubresourceLevel new
					aspect: AGPU_TEXTURE_ASPECT_COLOR;
					miplevel: levelIndex;
					layer_count: 1;
					yourself) ;
				texture_usage_mode: AGPU_TEXTURE_USAGE_COPY_DESTINATION;
				yourself);

		popTextureTransitionBarrier;
		popTextureTransitionBarrier.
]

{ #category : #private }
AGPUTextureMipmapGenerator >> countMiplevelsForExtent: anExtent [
	^ self countMiplevelsForWidth: anExtent x height: anExtent y
]

{ #category : #private }
AGPUTextureMipmapGenerator >> countMiplevelsForSize: size [
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
AGPUTextureMipmapGenerator >> countMiplevelsForWidth: width height: height [
	^ self countMiplevelsForSize: (width max: height)
]

{ #category : #API }
AGPUTextureMipmapGenerator >> emitMipmapGenerationCommandsWith: computationPipelineState on: commandList retainResourcesOn: resourcesList forTexture: texture extent: extent repeated: repeated sampleFromSuccessiveLevels: sampleFromSuccessiveLevels [
	utilityResourceCache useComputeShaderForMipmapGeneration ifTrue: [ 
		self emitMipmapGenerationCommandsWithComputePipelineState: computationPipelineState on: commandList retainResourcesOn: resourcesList forTexture: texture extent: extent repeated: repeated sampleFromSuccessiveLevels: sampleFromSuccessiveLevels 
	] ifFalse: [ 
		self emitMipmapGenerationCommandsWithRenderPipelineState: computationPipelineState on: commandList retainResourcesOn: resourcesList forTexture: texture extent: extent repeated: repeated sampleFromSuccessiveLevels: sampleFromSuccessiveLevels 
	]
]

{ #category : #private }
AGPUTextureMipmapGenerator >> emitMipmapGenerationCommandsWithComputePipelineState: computationPipelineState on: commandList retainResourcesOn: resourcesList forTexture: texture extent: extent repeated: repeated sampleFromSuccessiveLevels: sampleFromSuccessiveLevels [ 
	| currentWidth currentHeight quickConstants samplerBinding sourceColorViewDescription currentLevelRange currentLevelIndex intermediateTextureRange sourceColorView |
	(extent x <= 1 and: [ extent y <= 1 ]) ifTrue: [ ^ self ].

	self ensureIntermediateResourcesForExtent: extent.
	
	currentWidth := extent x.
	currentHeight := extent y.
	
	quickConstants := WordArray new: 4.
	samplerBinding := repeated
		ifFalse: [utilityResourceCache mipmapGenerationSamplerBinding]
		ifTrue: [utilityResourceCache mipmapGenerationRepeatSamplerBinding].

	resourcesList
		add: intermediateTexture;
		add: intermediateCopyBuffer;
		add: intermediateTextureView.
	
	sourceColorViewDescription := AGPUTextureViewDescription new.
	texture getFullViewDescription: sourceColorViewDescription.

	currentLevelRange := AGPUTextureSubresourceRange new
		aspect: AGPU_TEXTURE_ASPECT_COLOR;
		layer_count: 1;
		level_count: 1;
		yourself.
	currentLevelIndex := 0.

	intermediateTextureRange := AGPUTextureSubresourceRange new
		aspect: AGPU_TEXTURE_ASPECT_COLOR;
		layer_count: 1;
		level_count: 1;
		yourself.

	commandList
		setShaderSignature: utilityResourceCache mipmapGenerationShaderSignature;
		usePipelineState: computationPipelineState;
		useComputeShaderResources: samplerBinding.

	currentLevelRange base_miplevel: currentLevelIndex.
	sourceColorViewDescription subresource_range: currentLevelRange.
		
	sourceColorView := nil.
	[ currentWidth > 1 or: [ currentHeight > 1 ] ] whileTrue: [
		| binding nextWidth nextHeight |
		nextWidth := currentWidth // 2 max: 1.
		nextHeight := currentHeight // 2 max: 1.

		(sampleFromSuccessiveLevels or: [ sourceColorView isNil ]) ifTrue: [ 
			sourceColorView := texture createView: sourceColorViewDescription.
			resourcesList add: sourceColorView.
		].
		
		binding := self allocateShaderResourceBinding.
		binding
			bindSampledTextureView: 0 view: sourceColorView;
			bindStorageImageView: 1 view: intermediateTextureView.
			
		quickConstants
			at: 1 put: currentWidth;
			at: 2 put: currentHeight;
			at: 3 put: nextWidth;
			at: 4 put: nextHeight.

		commandList
			useComputeShaderResources: binding;
			pushConstants: 0 size: 16 values: quickConstants;
			dispatchCompute: (currentWidth + 15) // 16 group_count_y: (currentHeight + 15) // 16 group_count_z: 1.

		currentLevelIndex := currentLevelIndex + 1.
		currentWidth := nextWidth.
		currentHeight := nextHeight.

		currentLevelRange base_miplevel: currentLevelIndex.
		sourceColorViewDescription subresource_range: currentLevelRange.

		self copyFrom: intermediateTexture usageMode: AGPU_TEXTURE_USAGE_STORAGE
			into: texture usageMode: AGPU_TEXTURE_USAGE_SAMPLED level: currentLevelIndex
			width: currentWidth height: currentHeight on: commandList.
	].

]

{ #category : #private }
AGPUTextureMipmapGenerator >> emitMipmapGenerationCommandsWithRenderPipelineState: computationPipelineState on: commandList retainResourcesOn: resourcesList forTexture: texture extent: extent repeated: repeated sampleFromSuccessiveLevels: sampleFromSuccessiveLevels [ 
	| currentWidth currentHeight quickConstants samplerBinding sourceColorViewDescription currentLevelRange currentLevelIndex intermediateTextureRange sourceColorView renderpass |
	(extent x <= 1 and: [ extent y <= 1 ]) ifTrue: [ ^ self ].

	self ensureIntermediateResourcesForExtent: extent.
	
	renderpass := utilityResourceCache mipmapGenerationRenderpass.
		
	currentWidth := extent x.
	currentHeight := extent y.
	
	quickConstants := WordArray new: 4.
	samplerBinding := repeated
		ifFalse: [utilityResourceCache mipmapGenerationSamplerBinding]
		ifTrue: [utilityResourceCache mipmapGenerationRepeatSamplerBinding].

	resourcesList
		add: intermediateTexture;
		add: intermediateTextureFramebuffer;
		add: intermediateCopyBuffer;
		add: intermediateTextureView.
	
	sourceColorViewDescription := AGPUTextureViewDescription new.
	texture getFullViewDescription: sourceColorViewDescription.

	currentLevelRange := AGPUTextureSubresourceRange new
		aspect: AGPU_TEXTURE_ASPECT_COLOR;
		layer_count: 1;
		level_count: 1;
		yourself.
	currentLevelIndex := 0.

	intermediateTextureRange := AGPUTextureSubresourceRange new
		aspect: AGPU_TEXTURE_ASPECT_COLOR;
		layer_count: 1;
		level_count: 1;
		yourself.

	commandList
		setShaderSignature: utilityResourceCache mipmapGenerationShaderSignature;
		usePipelineState: computationPipelineState;
		useShaderResources: samplerBinding.

	currentLevelRange base_miplevel: currentLevelIndex.
	sourceColorViewDescription subresource_range: currentLevelRange.
		
	sourceColorView := nil.
	[ currentWidth > 1 or: [ currentHeight > 1 ] ] whileTrue: [
		| binding nextWidth nextHeight |
		nextWidth := currentWidth // 2 max: 1.
		nextHeight := currentHeight // 2 max: 1.

		(sampleFromSuccessiveLevels or: [ sourceColorView isNil ]) ifTrue: [ 
			sourceColorView := texture createView: sourceColorViewDescription.
			resourcesList add: sourceColorView.
		].
		
		binding := self allocateShaderResourceBinding.
		binding bindSampledTextureView: 0 view: sourceColorView.
			
		quickConstants
			at: 1 put: currentWidth;
			at: 2 put: currentHeight;
			at: 3 put: nextWidth;
			at: 4 put: nextHeight.

		commandList
			beginRenderPass: renderpass framebuffer: intermediateTextureFramebuffer bundle_content: 0;
			setViewport: 0 y: 0 w: nextWidth h: nextHeight;
			setScissor: 0 y: 0 w: nextWidth h: nextHeight;
			useShaderResources: binding;
			pushConstants: 0 size: 16 values: quickConstants;
			drawArrays: 3 instance_count: 1 first_vertex: 0 base_instance: 0;
			endRenderPass.

		currentLevelIndex := currentLevelIndex + 1.
		currentWidth := nextWidth.
		currentHeight := nextHeight.

		currentLevelRange base_miplevel: currentLevelIndex.
		sourceColorViewDescription subresource_range: currentLevelRange.

		self copyFrom: intermediateTexture usageMode: AGPU_TEXTURE_USAGE_COPY_SOURCE
			into: texture usageMode: AGPU_TEXTURE_USAGE_SAMPLED level: currentLevelIndex
			width: currentWidth height: currentHeight on: commandList.
	].

]

{ #category : #private }
AGPUTextureMipmapGenerator >> ensureIntermediateResourcesForExtent: requiredExtent [
	| requiredSize textureDescription |
	requiredSize := requiredExtent x max: requiredExtent y.
	intermediateTextureSize >= requiredSize ifTrue: [ ^ self ].
	
	intermediateTextureSize := requiredSize asLargerPowerOfTwo max: self class defaultIntermediateTextureSize.
	
	intermediateCopyBufferPitchAlignment := device getLimitValue: AGPU_LIMIT_MIN_TEXTURE_DATA_PITCH_ALIGNMENT.
	intermediateCopyBufferPitch := intermediateTextureSize * 4 alignedTo: intermediateCopyBufferPitchAlignment.
	
	"We need an intermediate copy buffer for reinterpreting between"
	intermediateCopyBuffer := device createBuffer: (AGPUBufferDescription new
		size: intermediateCopyBufferPitch * intermediateTextureSize;
		heap_type: AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
		usage_modes: (AGPU_COPY_SOURCE_BUFFER bitOr: AGPU_COPY_DESTINATION_BUFFER);
		main_usage_mode: AGPU_COPY_SOURCE_BUFFER;
		yourself) initial_data: nil.

	textureDescription := AGPUTextureDescription new
			type: AGPU_TEXTURE_2D;
			width: intermediateTextureSize;
			height: intermediateTextureSize;
			depth: 1;
			layers: 1;
			miplevels: 1;
			sample_count: 1;
			format: AGPU_TEXTURE_FORMAT_R8G8B8A8_UNORM;
			heap_type: AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
			usage_modes: (AGPU_TEXTURE_USAGE_STORAGE bitOr: AGPU_TEXTURE_USAGE_COPY_SOURCE);
			main_usage_mode: AGPU_TEXTURE_USAGE_STORAGE;
			yourself.

	utilityResourceCache useComputeShaderForMipmapGeneration ifFalse: [
		textureDescription
			format: AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM;
			usage_modes: AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT;
			main_usage_mode: AGPU_TEXTURE_USAGE_COPY_SOURCE.
	].

	intermediateTexture := device createTexture: textureDescription.
	
	utilityResourceCache useComputeShaderForMipmapGeneration ifTrue: [
		intermediateTextureView := intermediateTexture getOrCreateFullView.
	] ifFalse: [
		| intermediateTextureViewDescription |
		intermediateTextureViewDescription := AGPUTextureViewDescription new.
		intermediateTexture getFullViewDescription: intermediateTextureViewDescription.
		intermediateTextureViewDescription usage_mode: AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT.
		intermediateTextureView := intermediateTexture createView: intermediateTextureViewDescription.

		intermediateTextureFramebuffer := device createFrameBuffer: intermediateTextureSize height: intermediateTextureSize colorCount: 1 colorViews: (AGPU packListOfReferences: {intermediateTextureView}) depthStencilView: nil.
	].

]

{ #category : #initialization }
AGPUTextureMipmapGenerator >> initializeWithDevice: aDevice utilityResourceCache: anUtilityResourceCache [
	device := aDevice.
	utilityResourceCache := anUtilityResourceCache.
	allocatedShaderResourceBindings := OrderedCollection new.
	intermediateTextureSize := 0.
]

{ #category : #accessing }
AGPUTextureMipmapGenerator >> nonColorComputationPipelineStateWithQuality: quality [
	quality = 1 ifTrue: [ ^ utilityResourceCache nonColorMipmapComputationPipelineState].
	^ nil
]

{ #category : #accessing }
AGPUTextureMipmapGenerator >> normalMapComputationPipelineStateWithQuality: quality [
	quality = 1 ifTrue: [ ^ utilityResourceCache normalMapMipmapComputationPipelineState ].
	^ nil
]

{ #category : #initialization }
AGPUTextureMipmapGenerator >> reset [
	allocatedShaderResourceBindingCount := 0.
]

{ #category : #testing }
AGPUTextureMipmapGenerator >> shouldGenerateMipmapsForExtent: anExtent [
	^ anExtent x > 1 and: [ anExtent x isPowerOfTwo and: [
		anExtent y > 1 and: [ anExtent y isPowerOfTwo ]]]
	
]
