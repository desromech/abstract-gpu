"
I am an utility for preprocessing light probes.
"
Class {
	#name : #AGPUTextureLightprobePreprocessor,
	#superclass : #Object,
	#instVars : [
		'device',
		'utilityResourceCache',
		'allocatedShaderResourceBindings',
		'allocatedShaderResourceBindingCount',
		'intermediateTextureSize',
		'intermediateTexture',
		'intermediateTextureFramebuffer',
		'intermediateTextureView',
		'decodedHDRITextureExtent',
		'decodedHDRITexture',
		'decodedHDRITextureView',
		'decodedHDRITextureViewAttachment',
		'decodedHDRITextureFramebuffer'
	],
	#pools : [
		'AGPUConstants'
	],
	#category : #'AbstractGPU-Utility'
}

{ #category : #inspecting }
AGPUTextureLightprobePreprocessor class >> computeMiplevelSizesFor: size [
	^ Array streamContents: [ :out |
		| currentSize|
		currentSize := size.
		[ 
			out nextPut: currentSize.
			currentSize := currentSize // 2.
		] doWhileTrue: [ currentSize >= 1 ]
	].
]

{ #category : #'as yet unclassified' }
AGPUTextureLightprobePreprocessor class >> defaultDiffuseLightProbeSize [
	^ 32
]

{ #category : #'as yet unclassified' }
AGPUTextureLightprobePreprocessor class >> defaultEnvironmentMapSize [
	^ 512
]

{ #category : #'as yet unclassified' }
AGPUTextureLightprobePreprocessor class >> defaultSpecularLightProbeSize [
	^ 128
]

{ #category : #'instance creation' }
AGPUTextureLightprobePreprocessor class >> for: device [
	^ self for: device utilityResourceCache: (AGPUUtilityResourceCache for: device)
]

{ #category : #'instance creation' }
AGPUTextureLightprobePreprocessor class >> for: device utilityResourceCache: utilityResourceCache [
	^ self basicNew initializeWithDevice: device utilityResourceCache: utilityResourceCache
]

{ #category : #private }
AGPUTextureLightprobePreprocessor >> allocateShaderResourceBinding [
	| result |
	allocatedShaderResourceBindingCount >= allocatedShaderResourceBindings size ifTrue: [ 
		| newBinding |
		newBinding := utilityResourceCache lightProbePreprocessingShaderSignature createShaderResourceBinding: 1.
		allocatedShaderResourceBindings add: newBinding.
	].
	allocatedShaderResourceBindingCount := allocatedShaderResourceBindingCount + 1.
	result := allocatedShaderResourceBindings at: allocatedShaderResourceBindingCount.
	^ result
]

{ #category : #inspecting }
AGPUTextureLightprobePreprocessor >> computeMiplevelSizesFor: size [
	^ self class computeMiplevelSizesFor: size
]

{ #category : #API }
AGPUTextureLightprobePreprocessor >> emitCommandsForConvertingHDRI: hdriTextureView extent: hdriTextureExtent intoCubeMap: cubeMapTexture size: cubeMapTextureSize on: commandList retainResourcesOn: resourcesList [
	| sourceBinding |
	self ensureDecodedHDRIWithExtent: hdriTextureExtent.
	
	sourceBinding := self allocateShaderResourceBinding.
	sourceBinding bindSampledTextureView: 0 view: hdriTextureView.
	
	resourcesList
		add: decodedHDRITextureView;
		add: decodedHDRITexture;
		add: decodedHDRITextureViewAttachment;
		add: decodedHDRITextureFramebuffer;
		add: sourceBinding.
	
		commandList beginRenderPass: utilityResourceCache hdrCubeFaceGenerationRenderpass framebuffer: decodedHDRITextureFramebuffer bundle_content: 0;
		setViewport: 0 y: 0 w: hdriTextureExtent x h: hdriTextureExtent y;
		setScissor: 0 y: 0 w: hdriTextureExtent x h: hdriTextureExtent y;
		setShaderSignature: utilityResourceCache lightProbePreprocessingShaderSignature;
		usePipelineState: utilityResourceCache hdriDecodeComputationPipelineState;
		useShaderResources: utilityResourceCache equirectangularMapSamplerBinding;
		useShaderResources: sourceBinding;
		drawArrays: 3 instance_count: 1 first_vertex: 0 base_instance: 0;
		endRenderPass.

	self emitCommandsForProcessingTexture: decodedHDRITextureView equirectangular: true with: utilityResourceCache hdriToEnvironmentMapComputationPipelineState intoCubeMap: cubeMapTexture size: cubeMapTextureSize on: commandList retainResourcesOn: resourcesList
]

{ #category : #API }
AGPUTextureLightprobePreprocessor >> emitCommandsForConvertingHDRI: hdriTextureView intoCubeMap: cubeMapTexture size: cubeMapTextureSize on: commandList retainResourcesOn: resourcesList [
	self emitCommandsForProcessingTexture: hdriTextureView equirectangular: true with: utilityResourceCache hdriToEnvironmentMapComputationPipelineState intoCubeMap: cubeMapTexture size: cubeMapTextureSize on: commandList retainResourcesOn: resourcesList
]

{ #category : #API }
AGPUTextureLightprobePreprocessor >> emitCommandsForGeneratingDiffuseLightProbeFrom: environmentMapTextureView intoCubeMap: cubeMapTexture size: cubeMapTextureSize on: commandList retainResourcesOn: resourcesList [
	self emitCommandsForProcessingTexture: environmentMapTextureView equirectangular: false with: utilityResourceCache diffuseLightProbeComputationPipelineState intoCubeMap: cubeMapTexture size: cubeMapTextureSize on: commandList retainResourcesOn: resourcesList
]

{ #category : #API }
AGPUTextureLightprobePreprocessor >> emitCommandsForGeneratingSpecularLightProbeFrom: environmentMapTextureView intoCubeMap: cubeMapTexture size: cubeMapTextureSize on: commandList retainResourcesOn: resourcesList [
	| mipSizes mipCount computationPipelineState |
	mipSizes := self computeMiplevelSizesFor: cubeMapTextureSize.
	mipCount := mipSizes size.
	computationPipelineState := utilityResourceCache specularLightProbeComputationPipelineState.
	mipSizes doWithIndex: [ :mipSize :mipIndex |
		| roughness |
		roughness := (mipIndex - 1.0) / (mipCount - 1.0).
		self emitCommandsForProcessingTexture: environmentMapTextureView equirectangular: false with: computationPipelineState intoCubeMap: cubeMapTexture size: mipSize level: mipIndex - 1 roughnessFactor: roughness on: commandList retainResourcesOn: resourcesList
	]
]

{ #category : #API }
AGPUTextureLightprobePreprocessor >> emitCommandsForProcessingTexture: sourceTextureView equirectangular: equirectangular with: computationPipelineState intoCubeMap: cubeMapTexture size: cubeMapTextureSize level: destinationLevel roughnessFactor: roughnessFactor on: commandList retainResourcesOn: resourcesList [
	| renderpass shaderSignature samplerBinding sourceBinding |
	self ensureIntermediateResourcesForSize: cubeMapTextureSize.
	renderpass := utilityResourceCache hdrCubeFaceGenerationRenderpass.
	shaderSignature := utilityResourceCache lightProbePreprocessingShaderSignature.
	samplerBinding := equirectangular
		ifTrue: [ utilityResourceCache equirectangularMapSamplerBinding ]
		ifFalse: [ utilityResourceCache lightprobePreprocessingSamplerBinding ].
	sourceBinding := self allocateShaderResourceBinding.
	sourceBinding bindSampledTextureView: 0 view: sourceTextureView.
	
	resourcesList
		add: intermediateTexture;
		add: intermediateTextureFramebuffer;
		add: intermediateTextureView;
		add: sourceTextureView;
		add: sourceBinding.

	0 to: 5 do: [ :faceId |
		commandList beginRenderPass: renderpass framebuffer: intermediateTextureFramebuffer bundle_content: 0;
		setViewport: 0 y: 0 w: cubeMapTextureSize h: cubeMapTextureSize;
		setScissor: 0 y: 0 w: cubeMapTextureSize h: cubeMapTextureSize;
		setShaderSignature: shaderSignature;
		usePipelineState: computationPipelineState;
		useShaderResources: samplerBinding;
		useShaderResources: sourceBinding;
		pushConstants: 0 size: 4 values: (WordArray with: faceId);
		pushConstants: 4 size: 4 values: (FloatArray with: roughnessFactor);
		drawArrays: 3 instance_count: 1 first_vertex: 0 base_instance: 0;
		endRenderPass;

		pushTextureTransitionBarrier: cubeMapTexture old_usage: AGPU_TEXTURE_USAGE_SAMPLED new_usage: AGPU_TEXTURE_USAGE_COPY_DESTINATION subresource_range: (AGPUTextureSubresourceRange new
				aspect: AGPU_TEXTURE_ASPECT_COLOR;
				layer_count: 1;
				level_count: 1;
				base_arraylayer: faceId;
				base_miplevel: destinationLevel;
				yourself);
		copyTexture: intermediateTexture dest_texture: cubeMapTexture copy_region: (AGPUImageCopyRegion new
			source_usage_mode: AGPU_TEXTURE_USAGE_COPY_SOURCE;
			source_subresource_level: (AGPUTextureSubresourceLevel new
				aspect: AGPU_TEXTURE_ASPECT_COLOR;
				layer_count: 1;
				yourself);
			destination_usage_mode: AGPU_TEXTURE_USAGE_COPY_DESTINATION;
			destination_subresource_level: (AGPUTextureSubresourceLevel new
				aspect: AGPU_TEXTURE_ASPECT_COLOR;
				miplevel: destinationLevel;
				base_arraylayer: faceId;
				layer_count: 1;
				yourself);
			extent: (AGPUSize3d new
				width: cubeMapTextureSize;
				height: cubeMapTextureSize;
				depth: 1;
				yourself)
			yourself);
		popTextureTransitionBarrier
	].

]

{ #category : #API }
AGPUTextureLightprobePreprocessor >> emitCommandsForProcessingTexture: hdriTextureView equirectangular: equirectangular with: computationPipelineState intoCubeMap: cubeMapTexture size: cubeMapTextureSize on: commandList retainResourcesOn: resourcesList [
	self emitCommandsForProcessingTexture: hdriTextureView equirectangular: equirectangular with: computationPipelineState intoCubeMap: cubeMapTexture size: cubeMapTextureSize level: 0 roughnessFactor: 0.0 on: commandList retainResourcesOn: resourcesList
]

{ #category : #API }
AGPUTextureLightprobePreprocessor >> ensureDecodedHDRIWithExtent: requiredExtent [
	| decodedHDRITextureViewDescription |
	requiredExtent = decodedHDRITextureExtent ifTrue: [ ^ self ].
	decodedHDRITextureExtent := requiredExtent.
	
	decodedHDRITexture := device createTexture: (AGPUTextureDescription new
		type: AGPU_TEXTURE_2D;
		width: decodedHDRITextureExtent x;
		height: decodedHDRITextureExtent y;
		depth: 1;
		layers: 1;
		miplevels: 1;
		sample_count: 1;
		format: AGPU_TEXTURE_FORMAT_R16G16B16A16_FLOAT;
		heap_type: AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
		usage_modes: (AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT bitOr: AGPU_TEXTURE_USAGE_SAMPLED);
		main_usage_mode: AGPU_TEXTURE_USAGE_SAMPLED;
		yourself).
		
	decodedHDRITextureViewDescription := AGPUTextureViewDescription new.
	decodedHDRITexture getFullViewDescription: decodedHDRITextureViewDescription.
	decodedHDRITextureViewDescription usage_mode: AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT.
	decodedHDRITextureView := decodedHDRITexture getOrCreateFullView.
	decodedHDRITextureViewAttachment := decodedHDRITexture createView: decodedHDRITextureViewDescription.

	decodedHDRITextureFramebuffer := device createFrameBuffer: decodedHDRITextureExtent x height: decodedHDRITextureExtent y colorCount: 1 colorViews: (AGPU packListOfReferences: {decodedHDRITextureViewAttachment}) depthStencilView: nil.
]

{ #category : #API }
AGPUTextureLightprobePreprocessor >> ensureIntermediateResourcesForSize: requiredSize [
	| intermediateTextureViewDescription |
	requiredSize <= intermediateTextureSize ifTrue: [ ^ self ].
	
	intermediateTextureSize := requiredSize asLargerPowerOfTwo max: self class defaultEnvironmentMapSize.
	intermediateTexture := device createTexture: (AGPUTextureDescription new
		type: AGPU_TEXTURE_2D;
		width: intermediateTextureSize;
		height: intermediateTextureSize;
		depth: 1;
		layers: 1;
		miplevels: 1;
		sample_count: 1;
		format: AGPU_TEXTURE_FORMAT_R16G16B16A16_FLOAT;
		heap_type: AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
		usage_modes: (AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT bitOr: AGPU_TEXTURE_USAGE_COPY_SOURCE);
		main_usage_mode: AGPU_TEXTURE_USAGE_COPY_SOURCE;
		yourself).
		
	intermediateTextureViewDescription := AGPUTextureViewDescription new.
	intermediateTexture getFullViewDescription: intermediateTextureViewDescription.
	intermediateTextureViewDescription usage_mode: AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT.
	intermediateTextureView := intermediateTexture createView: intermediateTextureViewDescription.

	intermediateTextureFramebuffer := device createFrameBuffer: intermediateTextureSize height: intermediateTextureSize colorCount: 1 colorViews: (AGPU packListOfReferences: {intermediateTextureView}) depthStencilView: nil.
]

{ #category : #initialization }
AGPUTextureLightprobePreprocessor >> initializeWithDevice: aDevice utilityResourceCache: anUtilityResourceCache [
	device := aDevice.
	utilityResourceCache := anUtilityResourceCache.
	allocatedShaderResourceBindings := OrderedCollection new.
	intermediateTextureSize := 0.
]

{ #category : #initialization }
AGPUTextureLightprobePreprocessor >> reset [
	allocatedShaderResourceBindingCount := 0.
]
