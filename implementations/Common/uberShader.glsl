R"uberShader(
#line 3

// Flat Shading
// #define FLAT_SHADING.

// Enable lighting.
// #define LIGHTING_ENABLED
// #define PER_VERTEX_LIGHTING

// Enable/disable skinning.
// #define SKINNING_ENABLED

// Enable/disable texturing.
// #define TEXTURING_ENABLED

// One of the following must be enabled.
//#define BUILD_VERTEX_SHADER
//#define BUILD_FRAGMENT_SHADER

#define MAX_NUMBER_OF_BONES 128

#ifdef FLAT_SHADING
#define OPT_FLAT flat
#else
#define OPT_FLAT
#endif

struct LightState
{
    vec4 ambientColor;
    vec4 diffuseColor;
    vec4 specularColor;

    vec4 position;

    vec3 spotDirection;
    float spotCosCutoff;

    float spotExponent;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

layout(std140, set=1, binding=0) uniform LightingStateBlock
{
    vec4 ambientLighting;

    uint enabledLightMask;
    uint padding1;
    uint padding2;
    uint padding3;

    LightState lights[8];
} LightingState;

const uint FogMode_None = 0;
const uint FogMode_Linear = 1;
const uint FogMode_Exponential = 2;
const uint FogMode_ExponentialSquared = 3;

struct FogState
{
    uint mode;
    float startDistance;
    float endDistance;
    float density;

    vec4 color;
};

layout(set=2, binding=0) uniform ExtraRenderingStateBlock
{
    vec4 userClipPlane;

    FogState fogState;
} ExtraRenderingState;

layout(std140, set=3, binding=0) uniform MaterialStateBlock
{
    vec4 emission;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float shininess;
    uint padding1;
    uint padding2;
    uint padding3;
} MaterialState;

layout(set=4, binding=0) uniform TransformationStateBlock
{
    mat4 projectionMatrix;

    mat4 modelViewMatrix;
    mat4 inverseModelViewMatrix;

    mat4 textureMatrix;
} TransformationState;

layout(set=4, binding=0) uniform SkinningStateBlock
{
    mat4 boneMatrices[MAX_NUMBER_OF_BONES];
} SkinningState;

#ifdef LIGHTING_ENABLED

struct LightingParameters
{
    vec3 P;
    vec3 V;
    vec3 N;
};

vec4 computeLightContributionWith(in LightState light, in LightingParameters parameters)
{
    // Compute the ambient contribution.
    vec4 lightContribution = MaterialState.ambient*light.ambientColor;

    vec3 P = parameters.P;
    vec3 V = parameters.V;
    vec3 N = parameters.N;
    vec3 lightVector = light.position.xyz - parameters.P*light.position.w;
    vec3 L = normalize(lightVector);

    // Compute the diffuse contribution.
    float NdotL = max(dot(N, L), 0.0);
    if(NdotL > 0.0)
    {
        // Compute the spot light effect.
        float spotEffect = 1.0;
        if(light.spotCosCutoff > -0.5)
        {
            float spotCos = dot(L, light.spotDirection);
            if(spotCos < light.spotCosCutoff)
                spotEffect = 0.0;
            else
                spotEffect = pow(spotEffect, light.spotExponent);
        }

        // Only apply the lighting computation if inside of the spot light.
        if(spotEffect > 0.0)
        {
            float lightDistance = length(lightVector);
            float attenuation = spotEffect/(light.constantAttenuation + lightDistance*(light.linearAttenuation + lightDistance*light.quadraticAttenuation));
            lightContribution += (attenuation*NdotL) * MaterialState.diffuse * light.diffuseColor;

            // Compute the specular contribution.
            vec3 H = normalize(V + L);
            float NdotH = max(dot(N, H), 0.0);
            if(NdotH > 0.0)
            {
                lightContribution += (attenuation * pow(NdotH, MaterialState.shininess)) * MaterialState.specular * light.specularColor;
            }
        }
    }

    return lightContribution;
}

vec4 computingLightingWith(in LightingParameters parameters)
{
    vec4 color = MaterialState.emission +
        MaterialState.ambient*LightingState.ambientLighting;

    uint enabledLightMask = LightingState.enabledLightMask;
    for(int i = 0; i < 8 && (enabledLightMask != 0); ++i, enabledLightMask >>=1)
    {
        if((enabledLightMask & 1u) == 0)
            continue;

        color += computeLightContributionWith(LightingState.lights[i], parameters);
    }

    return clamp(color, 0.0, 1.0)*MaterialState.diffuse.a;
}
#endif

#if defined(BUILD_VERTEX_SHADER)
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexcoord;
layout(location = 4) in vec2 inTexcoord2;

#ifdef SKINNING_ENABLED
layout(location = 5) in ivec4 inBoneIndices;
layout(location = 6) in vec4 inBoneWeights;
#endif

layout(location = 0) OPT_FLAT out vec4 outColor;
layout(location = 1) out vec4 outTexcoord;
layout(location = 2) out vec4 outTexcoord2;
layout(location = 3) out vec4 outPosition;
layout(location = 4) out vec3 outNormal;

void main()
{
#ifdef SKINNING_ENABLED
    vec4 unskinnedPosition = vec4(inPosition, 1.0);
    vec4 unskinnedNormal = vec4(inNormal, 0.0);

    vec3 modelPosition = (SkinningState.boneMatrices[inBoneIndices.x]*unskinnedPosition).xyz*inBoneWeights.x;
    modelPosition += (SkinningState.boneMatrices[inBoneIndices.y]*unskinnedPosition).xyz*inBoneWeights.y;
    modelPosition += (SkinningState.boneMatrices[inBoneIndices.z]*unskinnedPosition).xyz*inBoneWeights.z;
    modelPosition += (SkinningState.boneMatrices[inBoneIndices.w]*unskinnedPosition).xyz*inBoneWeights.w;

    vec3 modelNormal = (SkinningState.boneMatrices[inBoneIndices.x]*unskinnedNormal).xyz*inBoneWeights.x;
    modelNormal += (SkinningState.boneMatrices[inBoneIndices.y]*unskinnedNormal).xyz*inBoneWeights.y;
    modelNormal += (SkinningState.boneMatrices[inBoneIndices.z]*unskinnedNormal).xyz*inBoneWeights.z;
    modelNormal += (SkinningState.boneMatrices[inBoneIndices.w]*unskinnedNormal).xyz*inBoneWeights.w;
#else
    vec3 modelPosition = inPosition;
    vec3 modelNormal = inNormal;
#endif

    vec4 viewPosition = TransformationState.modelViewMatrix * vec4(modelPosition, 1.0);
    vec3 viewNormal = (TransformationState.modelViewMatrix * vec4(modelNormal, 0.0)).xyz;

#if defined(LIGHTING_ENABLED) && defined(PER_VERTEX_LIGHTING)
    LightingParameters parameters;
    parameters.P = viewPosition.xyz;
    parameters.V = normalize(-viewPosition.xyz);
    parameters.N = viewNormal;
    vec4 color = computingLightingWith(parameters);
#else
    vec4 color = inColor;
#endif

    outColor = color;
    outTexcoord = TransformationState.textureMatrix*vec4(inTexcoord, 0.0, 1.0);
    outTexcoord2 = TransformationState.textureMatrix*vec4(inTexcoord2, 0.0, 1.0);

    outPosition = viewPosition;
    outNormal = viewNormal;
    gl_ClipDistance[0] = dot(ExtraRenderingState.userClipPlane, outPosition);
    gl_Position = TransformationState.projectionMatrix * outPosition;
}

#elif defined(BUILD_FRAGMENT_SHADER)

vec4 applyFogWithState(in FogState fogState, vec4 cleanColor, vec3 inPosition)
{
    uint mode = fogState.mode;
    if(mode == FogMode_None)
        return cleanColor;

    float d = length(inPosition);
    float f = 0.0;
    if(mode == FogMode_Linear)
    {
        f = max(0.0, (fogState.endDistance - d) / (fogState.endDistance - fogState.startDistance));
    }
    else if(mode == FogMode_Exponential)
    {
        f = exp(-d*fogState.density);
    }
    else if(mode == FogMode_ExponentialSquared)
    {
        float dc = d*fogState.density;
        f = exp(-dc*dc);
    }

    return mix(fogState.color, cleanColor, f);
}

vec4 applyFog(vec4 cleanColor, vec3 inPosition)
{
    return applyFogWithState(ExtraRenderingState.fogState, cleanColor, inPosition);
}

layout(set=0, binding=0) uniform sampler Sampler0;
layout(set=6, binding=0) uniform texture2D Texture0;

layout(location = 0) OPT_FLAT in vec4 inColor;
layout(location = 1) in vec4 inTexcoord;
layout(location = 2) in vec4 inTexcoord2;
layout(location = 3) in vec4 inPosition;
layout(location = 4) out vec3 outNormal;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 color = inColor;
#ifdef TEXTURING_ENABLED
    color = color*textureProj(sampler2D(Texture0, Sampler0), inTexcoord);
#endif

    outColor = applyFog(color, inPosition.xyz);
}

#else
#error BUILD_VERTEX_SHADER or BUILD_FRAGMENT_SHADER must be defined.
#endif
)uberShader"
