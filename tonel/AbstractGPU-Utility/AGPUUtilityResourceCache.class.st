"
I am a cache for common utility resources. These utility resources are the following:
- Shader signatures for mipmap generation in the GPU.
- Pipeline states for mipmap generation in the GPU.

"
Class {
	#name : #AGPUUtilityResourceCache,
	#superclass : #Object,
	#instVars : [
		'device',
		'mutex',
		'colorMipmapComputationPipelineState',
		'useComputeShaderForMipmapGeneration',
		'mipmapGenerationShaderSignature'
	],
	#pools : [
		'AGPUConstants'
	],
	#category : #'AbstractGPU-Utility'
}

{ #category : #'instance creation' }
AGPUUtilityResourceCache class >> for: device [
	^ self basicNew initializeWithDevice: device
]

{ #category : #'mipmap computation' }
AGPUUtilityResourceCache >> colorMipmapComputationPipelineState [
	^ mutex critical: [
		colorMipmapComputationPipelineState ifNil: [
			| builder |
			self privateUseComputeShaderForMipmapGeneration ifTrue: [
				builder := device createComputePipelineBuilder.
				builder
					setShaderSignature: self privateMipmapGenerationShaderSignature;
					attachShader: (self compileShader: self srgbMipmapComputationShader type: AGPU_COMPUTE_SHADER)
			] ifFalse: [
				self halt
			].
			colorMipmapComputationPipelineState := self finishBuildingRequiredPipelineStateWith: builder
		].
	
		colorMipmapComputationPipelineState
	]
]

{ #category : #'mipmap computation' }
AGPUUtilityResourceCache >> compileShader: sourceText type: shaderType [
	| shaderCompiler |
	shaderCompiler := device createOfflineShaderCompiler.
^ [
		shaderCompiler
			setShaderSource: AGPU_SHADER_LANGUAGE_VGLSL stage: shaderType sourceText: sourceText sourceTextLength: sourceText size;
			compileShader: AGPU_SHADER_LANGUAGE_DEVICE_SHADER options: '';
			getResultAsShader
		] on: AGPUError do: [ :e |
			e errorCode = AGPU_COMPILATION_ERROR ifTrue: [ 
				| logLength logBuffer compilationError |
				logLength := shaderCompiler getCompilationLogLength.
				logBuffer := ExternalAddress allocate: logLength + 1.
				compilationError := [
					shaderCompiler getCompilationLog: logLength + 1 buffer: logBuffer.
					logBuffer readString.
				] ensure: [ logBuffer free ].
				self error: compilationError
			] ifFalse: [
				e signal
			]
		]
]

{ #category : #private }
AGPUUtilityResourceCache >> finishBuildingRequiredPipelineStateWith: builder [
	| result |
	result := builder build.
	(result isNil or: [ result isNull ]) ifTrue: [ 
		| logLength logBuffer buildingError |
		logLength := builder getBuildingLogLength.
		logBuffer := ExternalAddress allocate: logLength + 1.
		buildingError := [
			builder getBuildingLog: logLength + 1 buffer: logBuffer.
			logBuffer readString.
		] ensure: [ logBuffer free ].
		self error: buildingError
	].

	^ result
]

{ #category : #initialization }
AGPUUtilityResourceCache >> initializeWithDevice: aDevice [
	device := aDevice.
	mutex := Semaphore forMutualExclusion.
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> mipmapComputationShader [
	^ '
#version 450
#line 5

layout(local_size_x = 16, local_size_y = 16) in;

layout (set=0, binding = 0, rgba8) uniform readonly image2D inputImage;
layout (set=0, binding = 1, rgba8) uniform writeonly image2D outputImage;

layout (push_constant) uniform Extents
{ 
	uvec2 sourceExtent;
	uvec2 destExtent;
};

shared vec4 fetchedTexels[16][16];

void main()
{
    ivec2 sourceCoord = ivec2(min(gl_GlobalInvocationID.xy, sourceExtent - 1));
    fetchedTexels[gl_LocalInvocationID.x][gl_LocalInvocationID.y] = imageLoad(inputImage, sourceCoord);
    barrier();

    uvec2 halfWorkGroupSize = gl_WorkGroupSize.xy/2;
    if(gl_LocalInvocationID.x >= halfWorkGroupSize.x || gl_LocalInvocationID.y >= halfWorkGroupSize.y)
        return;

    ivec2 destCoord = ivec2(gl_WorkGroupID.xy * halfWorkGroupSize + gl_LocalInvocationID.xy);
    uvec2 sourceTexel = gl_LocalInvocationID.xy*2;
    vec4 reducedTexel = (fetchedTexels[sourceTexel.x][sourceTexel.y] + fetchedTexels[sourceTexel.x + 1][sourceTexel.y] +
        fetchedTexels[sourceTexel.x][sourceTexel.y + 1] + fetchedTexels[sourceTexel.x + 1][sourceTexel.y + 1]) *0.25;

    if(destCoord.x < destExtent.x && destCoord.y < destExtent.y)
        imageStore(outputImage, destCoord, reducedTexel);
}
' withUnixLineEndings
]

{ #category : #'mipmap computation' }
AGPUUtilityResourceCache >> mipmapGenerationShaderSignature [
	^ mutex critical: [ self privateMipmapGenerationShaderSignature ]
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> normalMapMipmapComputationShader [
	^ '
#version 450
#line 5

layout(local_size_x = 16, local_size_y = 16) in;

layout (set=0, binding = 0, rgba8) uniform readonly image2D inputImage;
layout (set=0, binding = 1, rgba8) uniform writeonly image2D outputImage;

layout (push_constant) uniform Extents
{ 
	uvec2 sourceExtent;
	uvec2 destExtent;
};

shared vec3 fetchedTexels[16][16];

void main()
{
    ivec2 sourceCoord = ivec2(min(gl_GlobalInvocationID.xy, sourceExtent - 1));
    fetchedTexels[gl_LocalInvocationID.x][gl_LocalInvocationID.y] = imageLoad(inputImage, sourceCoord)).xyz*2.0 - 1.0;
    barrier();

    uvec2 halfWorkGroupSize = gl_WorkGroupSize.xy/2;
    if(gl_LocalInvocationID.x >= halfWorkGroupSize.x || gl_LocalInvocationID.y >= halfWorkGroupSize.y)
        return;

    ivec2 destCoord = ivec2(gl_WorkGroupID.xy * halfWorkGroupSize + gl_LocalInvocationID.xy);
    uvec2 sourceTexel = gl_LocalInvocationID.xy*2;
    vec3 reducedTexel = (fetchedTexels[sourceTexel.x][sourceTexel.y] + fetchedTexels[sourceTexel.x + 1][sourceTexel.y] +
        fetchedTexels[sourceTexel.x][sourceTexel.y + 1] + fetchedTexels[sourceTexel.x + 1][sourceTexel.y + 1]) normalized;

    if(destCoord.x < destExtent.x && destCoord.y < destExtent.y)
        imageStore(outputImage, destCoord, vec4(reducedTexel*0.5 + 0.5, 1.0));
}
' withUnixLineEndings
]

{ #category : #'mipmap computation' }
AGPUUtilityResourceCache >> privateMipmapGenerationShaderSignature [
	| builder |
	mipmapGenerationShaderSignature ifNotNil: [ ^ mipmapGenerationShaderSignature ].

	builder := device createShaderSignatureBuilder.

	self privateUseComputeShaderForMipmapGeneration ifTrue: [
		builder
			beginBindingBank: 256;
			"Input image"
			addBindingBankElement: AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE bindingPointCount: 1;
			"Output image"
			addBindingBankElement: AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE bindingPointCount: 1;

			"sourceWidth" addBindingConstant;
			"sourceHeight" addBindingConstant;
			"destinationWidth" addBindingConstant;
			"destinationHeight" addBindingConstant.
		mipmapGenerationShaderSignature := builder build.
	] ifFalse: [
		builder
			beginBindingBank: 1;
			addBindingBankElement: AGPU_SHADER_BINDING_TYPE_SAMPLER bindingPointCount: 1;
			beginBindingBank: 256;
			addBindingBankElement: AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE bindingPointCount: 1;
			"sourceWidth" addBindingConstant;
			"sourceHeight" addBindingConstant.

		mipmapGenerationShaderSignature := builder build.
		self halt
	].

	^ mipmapGenerationShaderSignature
]

{ #category : #'private - accessing' }
AGPUUtilityResourceCache >> privateUseComputeShaderForMipmapGeneration [
	^ useComputeShaderForMipmapGeneration ifNil: [ useComputeShaderForMipmapGeneration := (device isFeatureSupported: AGPU_FEATURE_COMPUTE_SHADER) ~= 0 ]
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> srgbMipmapComputationShader [
	^ '
#version 450
#line 5

layout(local_size_x = 16, local_size_y = 16) in;

layout (set=0, binding = 0, rgba8) uniform readonly image2D inputImage;
layout (set=0, binding = 1, rgba8) uniform writeonly image2D outputImage;

layout (push_constant) uniform Extents
{ 
	uvec2 sourceExtent;
	uvec2 destExtent;
};

shared vec4 fetchedTexels[16][16];

vec4 decodeSRGB(vec4 color)
{
    return vec4(pow(color.rgb, vec3(2.2)), color.a);
}

vec4 encodeSRGB(vec4 color)
{
    return vec4(pow(color.rgb, vec3(1.0/2.2)), color.a);
}

void main()
{
    ivec2 sourceCoord = ivec2(min(gl_GlobalInvocationID.xy, sourceExtent - 1));
    fetchedTexels[gl_LocalInvocationID.x][gl_LocalInvocationID.y] = decodeSRGB(imageLoad(inputImage, sourceCoord));
    barrier();

    uvec2 halfWorkGroupSize = gl_WorkGroupSize.xy/2;
    if(gl_LocalInvocationID.x >= halfWorkGroupSize.x || gl_LocalInvocationID.y >= halfWorkGroupSize.y)
        return;

    ivec2 destCoord = ivec2(gl_WorkGroupID.xy * halfWorkGroupSize + gl_LocalInvocationID.xy);
    uvec2 sourceTexel = gl_LocalInvocationID.xy*2;
    vec4 reducedTexel = (fetchedTexels[sourceTexel.x][sourceTexel.y] + fetchedTexels[sourceTexel.x + 1][sourceTexel.y] +
        fetchedTexels[sourceTexel.x][sourceTexel.y + 1] + fetchedTexels[sourceTexel.x + 1][sourceTexel.y + 1]) *0.25;

    if(destCoord.x < destExtent.x && destCoord.y < destExtent.y)
        imageStore(outputImage, destCoord, encodeSRGB(reducedTexel));
}
' withUnixLineEndings
]

{ #category : #'mipmap computation' }
AGPUUtilityResourceCache >> useComputeShaderForMipmapGeneration [
	^ mutex critical: [ self privateUseComputeShaderForMipmapGeneration ]
]
