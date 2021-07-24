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
		'mipmapGenerationShaderSignature',
		'screenQuadVertexShader',
		'mipmapGenerationSampler',
		'mipmapGenerationRepeatSampler',
		'mipmapGenerationSamplerBinding',
		'mipmapGenerationRepeatSamplerBinding',
		'mipmapGenerationRenderpass',
		'nonColorMipmapComputationPipelineState',
		'normalMapMipmapComputationPipelineState',
		'cubeScreenQuadVertexShader',
		'lightprobePreprocessingShaderSignature',
		'lightprobePreprocessingSampler',
		'lightprobePreprocessingSamplerBinding',
		'hdrCubeFaceGenerationRenderpass',
		'hdriToEnvironmentMapComputationPipelineState',
		'equirectangularMapSampler',
		'equirectangularMapSamplerBinding',
		'hdriDecodeComputationPipelineState',
		'diffuseLightProbeComputationPipelineState',
		'specularLightProbeComputationPipelineState'
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
			colorMipmapComputationPipelineState := self privateBuildMipmapComputationPipelineStateWithComputeShader: self srgbMipmapComputationShader fragmentComputationShader: self srgbMipmapComputationFragmentShader
		]
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

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> cubeScreenQuadFlippedYVertexShaderSource [
	^ '
#version 450
#line 5

layout(location = 0) out vec3 outDirection;
		
const vec2 screenQuadVertices[3] = vec2[3](
    vec2(-1.0, -1.0),		
    vec2(-1.0, 3.0),
    vec2(3.0, -1.0)
);

// Vulkan spec 15.6.4. Cube Map Face Selection
const vec3 faceCoordinateTransformS[6] = vec3[6](
	// Positive X
	vec3(0.0, 0.0, -1.0),
	// Negative X
	vec3(0.0, 0.0,  1.0),
	// Positive Y
	vec3(1.0, 0.0,  0.0),
	// Negative Y
	vec3(1.0, 0.0,  0.0),
	// Positive Z
	vec3(1.0, 0.0,  0.0),
	// Negative Z
	vec3(-1.0, 0.0,  0.0)
);

const vec3 faceCoordinateTransformT[6] = vec3[6](
	// Positive X
	vec3(0.0, -1.0, 0.0),
	// Negative X
	vec3(0.0, -1.0, 0.0),
	// Positive Y
	vec3(0.0, 0.0, 1.0),
	// Negative Y
	vec3(0.0, 0.0, -1.0),
	// Positive Z
	vec3(0.0, -1.0, 0.0),
	// Negative Z
	vec3(0.0, -1.0, 0.0)
);

const vec3 faceCoordinateTransformR[6] = vec3[6](
	// Positive X
	vec3(1.0, 0.0, 0.0),
	// Negative X
	vec3(-1.0, 0.0, 0.0),
	// Positive Y
	vec3(0.0, 1.0, 0.0),
	// Negative Y
	vec3(0.0, -1.0, 0.0),
	// Positive Z
	vec3(0.0, 0.0, 1.0),
	// Negative Z
	vec3(0.0, 0.0, -1.0)
);

layout (push_constant) uniform FaceIndex
{ 
	uint faceIndex;
};

void main()
{
	vec2 position = screenQuadVertices[gl_VertexIndex];
	vec3 str = vec3(position*vec2(1.0, -1.0), 1.0);
	
	mat3 faceTransform = mat3(faceCoordinateTransformS[faceIndex], faceCoordinateTransformT[faceIndex], faceCoordinateTransformR[faceIndex]);
	outDirection = faceTransform * str;
		
	gl_Position = vec4(position, 0.0, 1.0);
}

' withUnixLineEndings
]

{ #category : #shaders }
AGPUUtilityResourceCache >> cubeScreenQuadVertexShader [
	^ mutex critical: [ self privateCubeScreenQuadVertexShader ]

]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> cubeScreenQuadVertexShaderSource [
	^ '
#version 450
#line 5

layout(location = 0) out vec3 outDirection;

const vec2 screenQuadVertices[3] = vec2[3](
    vec2(-1.0, -1.0),		
    vec2(-1.0, 3.0),
    vec2(3.0, -1.0)
);

// Vulkan spec 15.6.4. Cube Map Face Selection
const vec3 faceCoordinateTransformS[6] = vec3[6](
	// Positive X
	vec3(0.0, 0.0, -1.0),
	// Negative X
	vec3(0.0, 0.0,  1.0),
	// Positive Y
	vec3(1.0, 0.0,  0.0),
	// Negative Y
	vec3(1.0, 0.0,  0.0),
	// Positive Z
	vec3(1.0, 0.0,  0.0),
	// Negative Z
	vec3(-1.0, 0.0,  0.0)
);

const vec3 faceCoordinateTransformT[6] = vec3[6](
	// Positive X
	vec3(0.0, -1.0, 0.0),
	// Negative X
	vec3(0.0, -1.0, 0.0),
	// Positive Y
	vec3(0.0, 0.0, 1.0),
	// Negative Y
	vec3(0.0, 0.0, -1.0),
	// Positive Z
	vec3(0.0, -1.0, 0.0),
	// Negative Z
	vec3(0.0, -1.0, 0.0)
);

const vec3 faceCoordinateTransformR[6] = vec3[6](
	// Positive X
	vec3(1.0, 0.0, 0.0),
	// Negative X
	vec3(-1.0, 0.0, 0.0),
	// Positive Y
	vec3(0.0, 1.0, 0.0),
	// Negative Y
	vec3(0.0, -1.0, 0.0),
	// Positive Z
	vec3(0.0, 0.0, 1.0),
	// Negative Z
	vec3(0.0, 0.0, -1.0)
);

layout (push_constant) uniform FaceIndex
{ 
	uint faceIndex;
};

void main()
{
	vec2 position = screenQuadVertices[gl_VertexIndex];
	vec3 str = vec3(position, 1.0);
	
	mat3 faceTransform = mat3(faceCoordinateTransformS[faceIndex], faceCoordinateTransformT[faceIndex], faceCoordinateTransformR[faceIndex]);
	outDirection = faceTransform * str;
		
	gl_Position = vec4(position, 0.0, 1.0);
}

' withUnixLineEndings
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> diffuseLightProbeComputationFragmentShader [
	^ '
#version 450
#line 5

layout (set=0, binding = 0) uniform sampler inputSampler;
layout (set=1, binding = 0) uniform textureCube inputTexture;

layout (location = 0) in vec3 inDirection;
layout (location = 0) out vec4 outColor;

void main()
{
	outColor = texture(samplerCube(inputTexture, inputSampler), inDirection);
}

' withUnixLineEndings
]

{ #category : #'lightprobe preprocessing' }
AGPUUtilityResourceCache >> diffuseLightProbeComputationPipelineState [
	^ mutex critical: [
		diffuseLightProbeComputationPipelineState ifNil: [
			diffuseLightProbeComputationPipelineState := self privateBuildLightProbeCubeFaceComputationPipelineStateWithFragmentShader: self diffuseLightProbeComputationFragmentShader
		]
	]
]

{ #category : #'lightprobe preprocessing' }
AGPUUtilityResourceCache >> equirectangularMapSamplerBinding [
	^ equirectangularMapSamplerBinding
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

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> hasBottomLeftTextureCoordinates [
	^ device hasBottomLeftTextureCoordinates ~= 0
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> hasInvertedProjectionY [
	^ device hasTopLeftNdcOrigin ~= 0
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> hasTextureInvertedProjectionY [
	^ device hasTopLeftNdcOrigin = device hasBottomLeftTextureCoordinates
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> hasTopDownTextureCoordinates [
	^ device hasBottomLeftTextureCoordinates = 0
]

{ #category : #'lightprobe preprocessing' }
AGPUUtilityResourceCache >> hdrCubeFaceGenerationRenderpass [
	^ mutex critical: [ self privateHdrCubeFaceGenerationRenderpass ]
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> hdriDecodeComputationFragmentShader [
	^ '
#version 450
#line 5

layout (set=0, binding = 0) uniform sampler inputSampler;
layout (set=1, binding = 0) uniform texture2D inputTexture;

layout (location = 0) in vec2 inTexcoord;
layout (location = 0) out vec4 outColor;

void main()
{
	vec4 rgbe = texture(sampler2D(inputTexture, inputSampler), inTexcoord);
	int exponent = int(rgbe.a*255.0);
	
	vec3 decodedSample = exponent > 0
		? ldexp(rgbe.rgb, ivec3(exponent - 129))
		: vec3(0.0);
	outColor = vec4(decodedSample, 1.0);
}

' withUnixLineEndings
]

{ #category : #'lightprobe preprocessing' }
AGPUUtilityResourceCache >> hdriDecodeComputationPipelineState [
	^ mutex critical: [
		hdriDecodeComputationPipelineState ifNil: [
			hdriDecodeComputationPipelineState := self privateBuildLightProbeComputationPipelineStateWithFragmentShader: self hdriDecodeComputationFragmentShader
		]
	]
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> hdriToEnvironmentMapComputationFragmentShader [
	^ '
#version 450
#line 5

layout (set=0, binding = 0) uniform sampler inputSampler;
layout (set=1, binding = 0) uniform texture2D inputTexture;

layout (location = 0) in vec3 inDirection;
layout (location = 0) out vec4 outColor;

void main()
{
	vec3 direction = normalize(inDirection);
	vec2 uv = vec2(atan(direction.z, direction.x), asin(direction.y)) *
		vec2(/*0.5 / pi */ 0.15915494309189535, /*0.5 / (pi/2) */ -0.3183098861837907) + 0.5;	
	
	outColor = texture(sampler2D(inputTexture, inputSampler), uv);
}

' withUnixLineEndings
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> hdriToEnvironmentMapComputationFragmentShaderFlippedY [
	^ '
#version 450
#line 5

layout (set=0, binding = 0) uniform sampler inputSampler;
layout (set=1, binding = 0) uniform texture2D inputTexture;

layout (location = 0) in vec3 inDirection;
layout (location = 0) out vec4 outColor;

void main()
{
	vec3 direction = normalize(inDirection);
	vec2 uv = vec2(atan(direction.z, direction.x), asin(direction.y)) *
		vec2(/*0.5 / pi */ 0.15915494309189535, /*0.5 / (pi/2) */ -0.3183098861837907) + 0.5;	
	
	outColor = texture(sampler2D(inputTexture, inputSampler), uv);
}

' withUnixLineEndings
]

{ #category : #'lightprobe preprocessing' }
AGPUUtilityResourceCache >> hdriToEnvironmentMapComputationPipelineState [
	^ mutex critical: [
		hdriToEnvironmentMapComputationPipelineState ifNil: [
			hdriToEnvironmentMapComputationPipelineState := self privateBuildLightProbeCubeFaceComputationPipelineStateWithFragmentShader: (self hasBottomLeftTextureCoordinates
				ifTrue: [ self hdriToEnvironmentMapComputationFragmentShaderFlippedY ]
				ifFalse: [ self hdriToEnvironmentMapComputationFragmentShader ])
		]
	]
]

{ #category : #initialization }
AGPUUtilityResourceCache >> initializeWithDevice: aDevice [
	device := aDevice.
	mutex := Semaphore forMutualExclusion.
]

{ #category : #'lightprobe preprocessing' }
AGPUUtilityResourceCache >> lightProbePreprocessingShaderSignature [
	^ mutex critical: [ self privateLightprobePreprocessingShaderSignature ]
]

{ #category : #'lightprobe preprocessing' }
AGPUUtilityResourceCache >> lightprobePreprocessingSamplerBinding [
	^ lightprobePreprocessingSamplerBinding
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
AGPUUtilityResourceCache >> mipmapGenerationRenderpass [
	^ mutex critical: [ self privateMipmapGenerationRenderpass ]
]

{ #category : #'mipmap computation' }
AGPUUtilityResourceCache >> mipmapGenerationRepeatSamplerBinding [
	^ mipmapGenerationRepeatSamplerBinding
]

{ #category : #'mipmap computation' }
AGPUUtilityResourceCache >> mipmapGenerationSamplerBinding [
	^ mipmapGenerationSamplerBinding
]

{ #category : #'mipmap computation' }
AGPUUtilityResourceCache >> mipmapGenerationShaderSignature [
	^ mutex critical: [ self privateMipmapGenerationShaderSignature ]
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> nonColorMipmapComputationFragmentShader [
	^ '
#version 450
#line 5

layout (set=0, binding = 0) uniform sampler inputSampler;
layout (set=1, binding = 0) uniform texture2D inputTexture;

layout (location = 0) in vec2 inTexcoord;
layout (location = 0) out vec4 outMiptexel;

layout (push_constant) uniform Extents
{ 
	uvec2 sourceExtent;
	uvec2 destinationExtent;
};

vec4 fetchTexel(uvec2 texcoord)
{ 
	vec2 samplingTexcoord = (vec2(texcoord) + 0.5) / vec2(sourceExtent);
	return texture(sampler2D(inputTexture, inputSampler), samplingTexcoord);
}

void main()
{
	uvec2 destinationTexcoord = uvec2(inTexcoord*vec2(destinationExtent));
	uvec2 sourceTexcoordTopLeft = destinationTexcoord * 2u;
	
	vec4 result = (fetchTexel(sourceTexcoordTopLeft) + fetchTexel(sourceTexcoordTopLeft + uvec2(1u, 0u)) +
		fetchTexel(sourceTexcoordTopLeft + uvec2(0u, 1u)) + fetchTexel(sourceTexcoordTopLeft + uvec2(1u, 1u))) * 0.25;
	outMiptexel = result;
}

' withUnixLineEndings
]

{ #category : #'mipmap computation' }
AGPUUtilityResourceCache >> nonColorMipmapComputationPipelineState [
	^ mutex critical: [
		nonColorMipmapComputationPipelineState ifNil: [
			nonColorMipmapComputationPipelineState := self privateBuildMipmapComputationPipelineStateWithComputeShader: self nonColorMipmapComputationShader fragmentComputationShader: self nonColorMipmapComputationFragmentShader
		]
	]
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> nonColorMipmapComputationShader [
	^ '
#version 450
#line 5

layout(local_size_x = 16, local_size_y = 16) in;

layout (set=0, binding = 0) uniform sampler inputSampler;
layout (set=1, binding = 0) uniform texture2D inputTexture;

layout (set=1, binding = 1, rgba8) uniform writeonly image2D outputImage;

layout (push_constant) uniform Extents
{ 
	uvec2 sourceExtent;
	uvec2 destExtent;
};
	
shared vec4 fetchedTexels[16][16];

void main()
{
    ivec2 sourceCoord = ivec2(min(gl_GlobalInvocationID.xy, sourceExtent - 1));
    fetchedTexels[gl_LocalInvocationID.x][gl_LocalInvocationID.y] = texelFetch(sampler2D(inputTexture, inputSampler), sourceCoord, 0).bgra;
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

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> normalMapMipmapComputationFragmentShader [
	^ '
#version 450
#line 5

layout (set=0, binding = 0) uniform sampler inputSampler;
layout (set=1, binding = 0) uniform texture2D inputTexture;

layout (location = 0) in vec2 inTexcoord;
layout (location = 0) out vec4 outMiptexel;

layout (push_constant) uniform Extents
{ 
	uvec2 sourceExtent;
	uvec2 destinationExtent;
};

vec3 fetchTexel(uvec2 texcoord)
{ 
	vec2 samplingTexcoord = (vec2(texcoord) + 0.5) / vec2(sourceExtent);
	return texture(sampler2D(inputTexture, inputSampler), samplingTexcoord).xyz*2.0 - 1.0;
}

void main()
{
	uvec2 destinationTexcoord = uvec2(inTexcoord*vec2(destinationExtent));
	uvec2 sourceTexcoordTopLeft = destinationTexcoord * 2u;
	
	vec3 result = normalize(fetchTexel(sourceTexcoordTopLeft) + fetchTexel(sourceTexcoordTopLeft + uvec2(1u, 0u)) +
		fetchTexel(sourceTexcoordTopLeft + uvec2(0u, 1u)) + fetchTexel(sourceTexcoordTopLeft + uvec2(1u, 1u)));
	outMiptexel = vec4(result*0.5 + 0.5, 1.0);
}

' withUnixLineEndings
]

{ #category : #'mipmap computation' }
AGPUUtilityResourceCache >> normalMapMipmapComputationPipelineState [
	^ mutex critical: [
		normalMapMipmapComputationPipelineState ifNil: [
			normalMapMipmapComputationPipelineState := self privateBuildMipmapComputationPipelineStateWithComputeShader: self normalMapMipmapComputationShader fragmentComputationShader: self normalMapMipmapComputationFragmentShader
		]
	]
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> normalMapMipmapComputationShader [
	^ '
#version 450
#line 5

layout(local_size_x = 16, local_size_y = 16) in;

layout (set=0, binding = 0) uniform sampler inputSampler;

layout (set=1, binding = 0) uniform texture2D inputTexture;
layout (set=1, binding = 1, rgba8) uniform writeonly image2D outputImage;

layout (push_constant) uniform Extents
{ 
	uvec2 sourceExtent;
	uvec2 destExtent;
};

shared vec3 fetchedTexels[16][16];

void main()
{
    ivec2 sourceCoord = ivec2(min(gl_GlobalInvocationID.xy, sourceExtent - 1));
    fetchedTexels[gl_LocalInvocationID.x][gl_LocalInvocationID.y] = texelFetch(sampler2D(inputTexture, inputSampler), sourceCoord, 0).bgr*2.0 - 1.0;
    barrier();

    uvec2 halfWorkGroupSize = gl_WorkGroupSize.xy/2;
    if(gl_LocalInvocationID.x >= halfWorkGroupSize.x || gl_LocalInvocationID.y >= halfWorkGroupSize.y)
        return;

    ivec2 destCoord = ivec2(gl_WorkGroupID.xy * halfWorkGroupSize + gl_LocalInvocationID.xy);
    uvec2 sourceTexel = gl_LocalInvocationID.xy*2;
    vec3 reducedTexel = normalize(fetchedTexels[sourceTexel.x][sourceTexel.y] + fetchedTexels[sourceTexel.x + 1][sourceTexel.y] +
        fetchedTexels[sourceTexel.x][sourceTexel.y + 1] + fetchedTexels[sourceTexel.x + 1][sourceTexel.y + 1]);

    if(destCoord.x < destExtent.x && destCoord.y < destExtent.y)
        imageStore(outputImage, destCoord, vec4(reducedTexel*0.5 + 0.5, 1.0));
}
' withUnixLineEndings
]

{ #category : #private }
AGPUUtilityResourceCache >> privateBuildLightProbeComputationPipelineStateWithFragmentShader: fragmentComputationShader [
	| builder |
	builder := device createPipelineBuilder.
	builder
		setShaderSignature: self privateLightprobePreprocessingShaderSignature;
		setRenderTargetFormat: 0 format: AGPU_TEXTURE_FORMAT_R16G16B16A16_FLOAT;
		setDepthStencilFormat: AGPU_TEXTURE_FORMAT_UNKNOWN;
		attachShader: self privateScreenQuadVertexShader;
		attachShader: (self compileShader: fragmentComputationShader type: AGPU_FRAGMENT_SHADER);
		setPrimitiveType: AGPU_TRIANGLES.

	^ self finishBuildingRequiredPipelineStateWith: builder

]

{ #category : #private }
AGPUUtilityResourceCache >> privateBuildLightProbeCubeFaceComputationPipelineStateWithFragmentShader: fragmentComputationShader [
	| builder |
	builder := device createPipelineBuilder.
	builder
		setShaderSignature: self privateLightprobePreprocessingShaderSignature;
		setRenderTargetFormat: 0 format: AGPU_TEXTURE_FORMAT_R16G16B16A16_FLOAT;
		setDepthStencilFormat: AGPU_TEXTURE_FORMAT_UNKNOWN;
		attachShader: self privateCubeScreenQuadVertexShader;
		attachShader: (self compileShader: fragmentComputationShader type: AGPU_FRAGMENT_SHADER);
		setPrimitiveType: AGPU_TRIANGLES.

	^ self finishBuildingRequiredPipelineStateWith: builder

]

{ #category : #private }
AGPUUtilityResourceCache >> privateBuildMipmapComputationPipelineStateWithComputeShader: computeShader fragmentComputationShader: fragmentComputationShader [
	| builder |
	self privateUseComputeShaderForMipmapGeneration ifTrue: [
		builder := device createComputePipelineBuilder.
		builder
			setShaderSignature: self privateMipmapGenerationShaderSignature;
			attachShader: (self compileShader: computeShader type: AGPU_COMPUTE_SHADER)
	] ifFalse: [
		builder := device createPipelineBuilder.
		builder
			setShaderSignature: self privateMipmapGenerationShaderSignature;
			setRenderTargetFormat: 0 format: AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM;
			setDepthStencilFormat: AGPU_TEXTURE_FORMAT_UNKNOWN;
			attachShader: self privateScreenQuadVertexShader;
			attachShader: (self compileShader: fragmentComputationShader type: AGPU_FRAGMENT_SHADER);
			setPrimitiveType: AGPU_TRIANGLES
	].
	^ self finishBuildingRequiredPipelineStateWith: builder

]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> privateCubeScreenQuadVertexShader [
	^ cubeScreenQuadVertexShader ifNil: [
		| source |
		source := self hasInvertedProjectionY
			ifTrue: [ self cubeScreenQuadVertexShaderSource ]
			ifFalse: [ self cubeScreenQuadFlippedYVertexShaderSource ].	

		cubeScreenQuadVertexShader := self compileShader: source type: AGPU_VERTEX_SHADER
	]
]

{ #category : #'private - accessing' }
AGPUUtilityResourceCache >> privateHdrCubeFaceGenerationRenderpass [
	| colorAttachment |
	hdrCubeFaceGenerationRenderpass ifNotNil: [ ^ hdrCubeFaceGenerationRenderpass ].
	colorAttachment := AGPURenderpassColorAttachmentDescription externalNew.
	[
		| renderpassDescription |
		colorAttachment
			format: AGPU_TEXTURE_FORMAT_R16G16B16A16_FLOAT;
			begin_action: AGPU_ATTACHMENT_DISCARD;
			end_action: AGPU_ATTACHMENT_KEEP;
			sample_count: 1;
			sample_quality: 0;
			yourself.
		renderpassDescription := AGPURenderpassDescription new.
		renderpassDescription
			color_attachment_count: 1;
			color_attachments: colorAttachment.
		hdrCubeFaceGenerationRenderpass := device createRenderPass: renderpassDescription.
	] ensure: [ colorAttachment free ].
	^ hdrCubeFaceGenerationRenderpass
]

{ #category : #'private - accessing' }
AGPUUtilityResourceCache >> privateLightprobePreprocessingShaderSignature [
	| builder |
	lightprobePreprocessingShaderSignature ifNotNil: [ ^ lightprobePreprocessingShaderSignature ].

	builder := device createShaderSignatureBuilder.
	builder
		beginBindingBank: 2;
		addBindingBankElement: AGPU_SHADER_BINDING_TYPE_SAMPLER bindingPointCount: 1;
		
		beginBindingBank: 128;
		"Input image"
		addBindingBankElement: AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE bindingPointCount: 1.

	builder
		"faceIndex"
		addBindingConstant;
		"roughness"
		addBindingConstant.

	lightprobePreprocessingShaderSignature := builder build.

	equirectangularMapSampler := device createSampler: (AGPUSamplerDescription new
		filter: AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST;
		address_u: AGPU_TEXTURE_ADDRESS_MODE_WRAP;
		address_v: AGPU_TEXTURE_ADDRESS_MODE_CLAMP;
		address_w: AGPU_TEXTURE_ADDRESS_MODE_CLAMP;
		max_lod: 0.0;
		yourself).
	
	equirectangularMapSamplerBinding := lightprobePreprocessingShaderSignature createShaderResourceBinding: 0.
	equirectangularMapSamplerBinding bindSampler: 0 sampler: equirectangularMapSampler.

	lightprobePreprocessingSampler := device createSampler: (AGPUSamplerDescription new
		filter: AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST;
		address_u: AGPU_TEXTURE_ADDRESS_MODE_CLAMP;
		address_v: AGPU_TEXTURE_ADDRESS_MODE_CLAMP;
		address_w: AGPU_TEXTURE_ADDRESS_MODE_CLAMP;
		max_lod: 0.0;
		yourself).
	
	lightprobePreprocessingSamplerBinding := lightprobePreprocessingShaderSignature createShaderResourceBinding: 0.
	lightprobePreprocessingSamplerBinding bindSampler: 0 sampler: lightprobePreprocessingSampler.

	^ lightprobePreprocessingShaderSignature
]

{ #category : #'private - accessing' }
AGPUUtilityResourceCache >> privateMipmapGenerationRenderpass [
	| colorAttachment |
	mipmapGenerationRenderpass ifNotNil: [ ^ mipmapGenerationRenderpass ].
	colorAttachment := AGPURenderpassColorAttachmentDescription externalNew.
	[
		| renderpassDescription |
		colorAttachment
			format: AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM;
			begin_action: AGPU_ATTACHMENT_DISCARD;
			end_action: AGPU_ATTACHMENT_KEEP;
			sample_count: 1;
			sample_quality: 0;
			yourself.
		renderpassDescription := AGPURenderpassDescription new.
		renderpassDescription
			color_attachment_count: 1;
			color_attachments: colorAttachment.
		mipmapGenerationRenderpass := device createRenderPass: renderpassDescription.
	] ensure: [ colorAttachment free ].
	^ mipmapGenerationRenderpass
]

{ #category : #'private - accessing' }
AGPUUtilityResourceCache >> privateMipmapGenerationShaderSignature [
	| builder |
	mipmapGenerationShaderSignature ifNotNil: [ ^ mipmapGenerationShaderSignature ].

	builder := device createShaderSignatureBuilder.
	builder
		beginBindingBank: 2;
		addBindingBankElement: AGPU_SHADER_BINDING_TYPE_SAMPLER bindingPointCount: 1;
		
		beginBindingBank: 256;
		"Input image"
		addBindingBankElement: AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE bindingPointCount: 1.


	self privateUseComputeShaderForMipmapGeneration ifTrue: [
		builder
			"Output image"
			addBindingBankElement: AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE bindingPointCount: 1
	].

	builder
		"sourceWidth" addBindingConstant;
		"sourceHeight" addBindingConstant;
		"destinationWidth" addBindingConstant;
		"destinationHeight" addBindingConstant.

	mipmapGenerationShaderSignature := builder build.

	mipmapGenerationSampler := device createSampler: (AGPUSamplerDescription new
		filter: AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST;
		address_u: AGPU_TEXTURE_ADDRESS_MODE_CLAMP;
		address_v: AGPU_TEXTURE_ADDRESS_MODE_CLAMP;
		address_w: AGPU_TEXTURE_ADDRESS_MODE_CLAMP;
		max_lod: 0.0;
		yourself).
	
	mipmapGenerationSamplerBinding := mipmapGenerationShaderSignature createShaderResourceBinding: 0.
	mipmapGenerationSamplerBinding bindSampler: 0 sampler: mipmapGenerationSampler.

	mipmapGenerationRepeatSampler := device createSampler: (AGPUSamplerDescription new
		filter: AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST;
		address_u: AGPU_TEXTURE_ADDRESS_MODE_WRAP;
		address_v: AGPU_TEXTURE_ADDRESS_MODE_WRAP;
		address_w: AGPU_TEXTURE_ADDRESS_MODE_WRAP;
		max_lod: 0.0;
		yourself).
	
	mipmapGenerationRepeatSamplerBinding := mipmapGenerationShaderSignature createShaderResourceBinding: 0.
	mipmapGenerationRepeatSamplerBinding bindSampler: 0 sampler: mipmapGenerationRepeatSampler.

	^ mipmapGenerationShaderSignature
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> privateScreenQuadVertexShader [
	^ screenQuadVertexShader ifNil: [
		| source |
		source := self hasTextureInvertedProjectionY
			ifTrue: [ self screenQuadFlippedYVertexShaderSource ]
			ifFalse: [ self screenQuadVertexShaderSource ].

		screenQuadVertexShader := self compileShader: source type: AGPU_VERTEX_SHADER
	]
]

{ #category : #'private - accessing' }
AGPUUtilityResourceCache >> privateUseComputeShaderForMipmapGeneration [
	^ useComputeShaderForMipmapGeneration ifNil: [
			useComputeShaderForMipmapGeneration := (device isFeatureSupported: AGPU_FEATURE_COMPUTE_SHADER) ~= 0
	]
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> screenQuadFlippedYVertexShaderSource [
	^ '
#version 450
#line 5

layout(location = 0) out vec2 outTexcoord;

const vec2 screenQuadVertices[3] = vec2[3](
    vec2(-1.0, -1.0),
    vec2(-1.0, 3.0),
    vec2(3.0, -1.0)
);

void main()
{
    outTexcoord = screenQuadVertices[gl_VertexIndex]*vec2(0.5, -0.5) + 0.5;
    gl_Position = vec4(screenQuadVertices[gl_VertexIndex], 0.0, 1.0);
}

' withUnixLineEndings
]

{ #category : #shaders }
AGPUUtilityResourceCache >> screenQuadVertexShader [
	^ mutex critical: [ self privateScreenQuadVertexShader ]

]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> screenQuadVertexShaderSource [
	^ '
#version 450
#line 5

layout(location = 0) out vec2 outTexcoord;

const vec2 screenQuadVertices[3] = vec2[3](
    vec2(-1.0, -1.0),
    vec2(-1.0, 3.0),
    vec2(3.0, -1.0)
);

void main()
{
    outTexcoord = screenQuadVertices[gl_VertexIndex]*0.5 + 0.5;
    gl_Position = vec4(screenQuadVertices[gl_VertexIndex], 0.0, 1.0);
}

' withUnixLineEndings
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> specularLightProbeComputationFragmentShader [
	^ '
#version 450
#line 5

layout (set=0, binding = 0) uniform sampler inputSampler;
layout (set=1, binding = 0) uniform textureCube inputTexture;

layout (location = 0) in vec3 inDirection;
layout (location = 0) out vec4 outColor;

layout (push_constant) uniform FaceIndex
{ 
	uint faceIndex;
	float roughness;
};

void main()
{
	outColor = texture(samplerCube(inputTexture, inputSampler), inDirection);
}

' withUnixLineEndings
]

{ #category : #'lightprobe preprocessing' }
AGPUUtilityResourceCache >> specularLightProbeComputationPipelineState [
	^ mutex critical: [
		specularLightProbeComputationPipelineState ifNil: [
			specularLightProbeComputationPipelineState := self privateBuildLightProbeCubeFaceComputationPipelineStateWithFragmentShader: self specularLightProbeComputationFragmentShader
		]
	]
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> srgbMipmapComputationFragmentShader [
	^ '
#version 450
#line 5

layout (set=0, binding = 0) uniform sampler inputSampler;
layout (set=1, binding = 0) uniform texture2D inputTexture;

layout (location = 0) in vec2 inTexcoord;
layout (location = 0) out vec4 outMiptexel;

layout (push_constant) uniform Extents
{ 
	uvec2 sourceExtent;
	uvec2 destinationExtent;
};

// See https://en.wikipedia.org/wiki/SRGB [May, 2021] for these conversion formulas.
vec4 encodeSRGB(vec4 color)
{
	vec3 isPowerSection = vec3(lessThanEqual(color.rgb, vec3(0.0031308)));
	vec3 linearSection = 12.92 * color.rgb;
	vec3 powerSection = 1.055 * pow(color.rgb, vec3(1.0/2.4)) - 0.055;
	return vec4(mix(linearSection, powerSection, isPowerSection), color.a);
}

vec4 fetchTexel(uvec2 texcoord)
{ 
	vec2 samplingTexcoord = (vec2(texcoord) + 0.5) / vec2(sourceExtent);
	return texture(sampler2D(inputTexture, inputSampler), samplingTexcoord);
}

void main()
{
	uvec2 destinationTexcoord = uvec2(inTexcoord*vec2(destinationExtent));
	uvec2 sourceTexcoordTopLeft = destinationTexcoord * 2u;
	
	vec4 result = (fetchTexel(sourceTexcoordTopLeft) + fetchTexel(sourceTexcoordTopLeft + uvec2(1u, 0u)) +
		fetchTexel(sourceTexcoordTopLeft + uvec2(0u, 1u)) + fetchTexel(sourceTexcoordTopLeft + uvec2(1u, 1u))) * 0.25;
	outMiptexel = encodeSRGB(result);
}

' withUnixLineEndings
]

{ #category : #'private - shaders' }
AGPUUtilityResourceCache >> srgbMipmapComputationShader [
	^ '
#version 450
#line 5

layout(local_size_x = 16, local_size_y = 16) in;

layout (set=0, binding = 0) uniform sampler inputSampler;
layout (set=1, binding = 0) uniform texture2D inputTexture;

layout (set=1, binding = 1, rgba8) uniform writeonly image2D outputImage;

layout (push_constant) uniform Extents
{ 
	uvec2 sourceExtent;
	uvec2 destExtent;
};
	
shared vec4 fetchedTexels[16][16];

vec4 encodeSRGB(vec4 color)
{
	vec3 isPowerSection = vec3(lessThanEqual(color.rgb, vec3(0.0031308)));
	vec3 linearSection = 12.92 * color.rgb;
	vec3 powerSection = 1.055 * pow(color.rgb, vec3(1.0/2.4)) - 0.055;
	return vec4(mix(linearSection, powerSection, isPowerSection), color.a);
}

void main()
{
    ivec2 sourceCoord = ivec2(min(gl_GlobalInvocationID.xy, sourceExtent - 1));
    fetchedTexels[gl_LocalInvocationID.x][gl_LocalInvocationID.y] = texelFetch(sampler2D(inputTexture, inputSampler), sourceCoord, 0).bgra;
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
