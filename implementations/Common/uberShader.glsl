R"uberShader(
#line 3

#define PI 3.141592653589793
#define PI_RECIPROCAL 0.3183098861837907
#define SquareRootOfTwoOverPi 0.7978845608028654

// Flat Shading
// #define FLAT_SHADING.

// Enable flat color material
// #define FLAT_COLOR_MATERIAL

// Enable lighting.
// #define LIGHTING_ENABLED
// #define PER_VERTEX_LIGHTING
// #define PER_FRAGMENT_LIGHTING
// #define PBR_METALLIC_ROUGHNESS

// Enable tangent space normal mapping.
// #define TANGENT_SPACE_ENABLED

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

#ifdef LIGHTING_ENABLED
struct LightState
{
#ifdef PBR_METALLIC_ROUGHNESS
    vec4 ambientColor;
    vec4 intensity;
    vec4 position;

    vec3 spotDirection;
    float spotCosCutoff;

    float spotExponent;
    float spotInnerCosCutoff;
    float radius;
    float padding;
    vec4 padding2;
#else
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
#endif
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
#endif

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
#if defined(PBR_METALLIC_ROUGHNESS)
    uint type;
    float roughnessFactor;
    float metallicFactor;
    float occlusionFactor;

    float alphaCutoff;
    float padding;
    float padding2;
    float padding3;

    vec4 emission;
    vec4 baseColor;
#elif defined(FLAT_COLOR_MATERIAL)
    uint type;
    float alphaCutoff;
    uint padding;
    uint padding2;

    vec4 color;
#else
    uint type;
    float shininess;
    float alphaCutoff;
    uint padding;

    vec4 emission;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
#endif
} MaterialState;

layout(set=4, binding=0) uniform TransformationStateBlock
{
    mat4 projectionMatrix;

    mat4 modelViewMatrix;
    mat4 inverseModelViewMatrix;

    mat4 textureMatrix;
} TransformationState;

#ifdef SKINNING_ENABLED
layout(set=5, binding=0) uniform SkinningStateBlock
{
    mat4 boneMatrices[MAX_NUMBER_OF_BONES];
} SkinningState;
#endif

#ifdef LIGHTING_ENABLED

struct LightingParameters
{
    vec4 ambientColor;
    vec4 emissionColor;
    vec4 baseColor;
    vec4 diffuse;

#ifdef PBR_METALLIC_ROUGHNESS
    vec4 Cdiffuse;
    vec3 F0;
    float alpha;

    float k;
    float NdotV;
    float occlusion;
    float roughness;
    float metallic;
#else
    vec4 specular;
#endif
    vec3 P;
    vec3 V;
    vec3 N;
};

vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
    float powFactor = 1.0f - cosTheta;
    float powFactor2 = powFactor * powFactor;
    float powFactor4 = powFactor2 * powFactor2;
    float powValue = powFactor4 * powFactor;

    return F0 + (vec3(1.0) - F0) * powValue;
}

float ggxSpecularDistribution(float alpha, float cosTheta)
{
	float alphaSquare = alpha*alpha;
	float den = cosTheta*cosTheta*(alphaSquare - 1.0) + 1.0;
	return alphaSquare / (PI * den*den);
}

float smithSchlickBeckmannReciprocalFunction(float k, float cosTheta)
{
    return cosTheta*(1.0 - k) + k;
}

float cookTorranceSmithSchlickGGXMasking(float k, float NdotL, float NdotV)
{
	return 1.0 / (4.0*smithSchlickBeckmannReciprocalFunction(k, NdotL)*smithSchlickBeckmannReciprocalFunction(k, NdotV));
}

float computeLightAttenuation(in LightState light, float distance)
{
#ifdef PBR_METALLIC_ROUGHNESS
    float a = distance / light.radius;
    float a2 = a*a;
    float a4 = a2*a2;
    float num = clamp(1.0f - a4, 0.0, 1.0);
    return num*num / (distance*distance + 1.0);
#else
    return 1.0 / (light.constantAttenuation + distance*(light.linearAttenuation + distance*light.quadraticAttenuation));
#endif
}

vec4 computeLightContributionWith(in LightState light, in LightingParameters parameters)
{
    vec3 P = parameters.P;
    vec3 V = parameters.V;
    vec3 N = parameters.N;

    // Compute the light vector.
    vec3 L = light.position.xyz;
    float lightDistance = 0.0;
    if(light.position.w != 0.0)
    {
        L = L - parameters.P;
        lightDistance = length(L);
        L = L / lightDistance;
    }
    else
    {
        L = normalize(L);
    }

    vec4 lightContribution = vec4(0.0);

    // Compute the spot light effect.
    float spotEffect = 1.0;
    if(light.spotCosCutoff > -0.5)
    {
        float spotCos = dot(L, light.spotDirection);
        if(spotCos < light.spotCosCutoff)
            return lightContribution;

        spotEffect = pow(spotEffect, light.spotExponent);
    }

    // Compute the full attenuation factor.
    float attenuation = spotEffect*computeLightAttenuation(light, lightDistance);
    if(attenuation <= 0.0)
        return lightContribution;

    // Compute the ambient contribution.
    lightContribution = parameters.ambientColor*light.ambientColor;

    // Compute the diffuse contribution.
    float NdotL = max(dot(N, L), 0.0);
    if(NdotL > 0.0)
    {
        // Only apply the lighting computation if inside of the spot light.
        if(spotEffect > 0.0)
        {
            // Compute the specular contribution.
            vec3 H = normalize(L + V);
            float NdotH = max(dot(N, H), 0.0);

#if defined(PBR_METALLIC_ROUGHNESS)
            float VdotH = max(dot(V, H), 0.0);

            vec3 F = fresnelSchlick(parameters.F0, VdotH);
            float D = ggxSpecularDistribution(parameters.alpha, NdotH);
            float G = cookTorranceSmithSchlickGGXMasking(parameters.k, NdotL, parameters.NdotV);

            lightContribution += vec4(light.intensity.rgb * (parameters.diffuse.rgb + F*D*G) * (NdotL*PI), 1.0);
#else
            float D = NdotH > 0.0 ? pow(NdotH, MaterialState.shininess) : 0.0;
            lightContribution += NdotL*(parameters.diffuse * light.diffuseColor + D*parameters.specular * light.specularColor);
#endif
        }
    }

    return lightContribution*attenuation;
}

vec4 computeLightingWith(in LightingParameters parameters)
{
#ifdef PBR_METALLIC_ROUGHNESS
    vec3 dielecticF0 = vec3(0.04);

    parameters.Cdiffuse = vec4(mix(parameters.baseColor.rgb * (1.0 - dielecticF0), vec3(0.0), parameters.metallic), parameters.baseColor.a);
    parameters.diffuse = parameters.Cdiffuse * PI_RECIPROCAL;
    parameters.F0 = mix(dielecticF0, parameters.baseColor.rgb, parameters.metallic);

    float directRoughness = mix(0.01, 1.0, parameters.roughness);
    parameters.alpha = directRoughness*directRoughness;

    float kRoughness = (directRoughness + 1.0);
    parameters.k = kRoughness*kRoughness / 8.0;

    parameters.NdotV = clamp(dot(parameters.N, parameters.V), 0.0, 1.0);
    parameters.ambientColor = parameters.baseColor * parameters.occlusion;

#else
    parameters.ambientColor = MaterialState.ambient * parameters.baseColor;
    parameters.diffuse = MaterialState.diffuse*parameters.baseColor;
    parameters.specular = MaterialState.specular;
#endif
    vec4 color = parameters.emissionColor;
    color += LightingState.ambientLighting * parameters.ambientColor;

    uint enabledLightMask = LightingState.enabledLightMask;
    for(int i = 0; i < 8 && (enabledLightMask != 0); ++i, enabledLightMask >>=1)
    {
        if((enabledLightMask & 1u) == 0)
            continue;

        color += computeLightContributionWith(LightingState.lights[i], parameters);
    }

#ifdef PBR_METALLIC_ROUGHNESS
    return vec4(color.rgb, parameters.baseColor.a);
#else
    return clamp(color, 0.0, 1.0)*parameters.diffuse.a;
#endif
}
#endif

#if defined(BUILD_VERTEX_SHADER)
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexcoord;
//layout(location = 4) in vec2 inTexcoord2;

#ifdef SKINNING_ENABLED
layout(location = 5) in vec4 inBoneIndices;
layout(location = 6) in vec4 inBoneWeights;
#endif

#ifdef TANGENT_SPACE_ENABLED
layout(location = 7) in vec4 inTangent4;
#endif

layout(location = 0) OPT_FLAT out vec4 outColor;
layout(location = 1) out vec4 outTexcoord;
//layout(location = 2) out vec4 outTexcoord2;
layout(location = 3) out vec4 outPosition;
layout(location = 4) out vec3 outNormal;

#ifdef TANGENT_SPACE_ENABLED
layout(location = 5) out vec3 outTangent;
layout(location = 6) out vec3 outBitangent;
#endif

void main()
{
#ifdef SKINNING_ENABLED
    vec4 unskinnedPosition = vec4(inPosition, 1.0);
    vec4 unskinnedNormal = vec4(inNormal, 0.0);

    ivec4 boneIndices = ivec4(inBoneIndices);
    vec3 modelPosition = (SkinningState.boneMatrices[boneIndices.x]*unskinnedPosition).xyz*inBoneWeights.x;
    modelPosition += (SkinningState.boneMatrices[boneIndices.y]*unskinnedPosition).xyz*inBoneWeights.y;
    modelPosition += (SkinningState.boneMatrices[boneIndices.z]*unskinnedPosition).xyz*inBoneWeights.z;
    modelPosition += (SkinningState.boneMatrices[boneIndices.w]*unskinnedPosition).xyz*inBoneWeights.w;

    vec3 modelNormal = (SkinningState.boneMatrices[boneIndices.x]*unskinnedNormal).xyz*inBoneWeights.x;
    modelNormal += (SkinningState.boneMatrices[boneIndices.y]*unskinnedNormal).xyz*inBoneWeights.y;
    modelNormal += (SkinningState.boneMatrices[boneIndices.z]*unskinnedNormal).xyz*inBoneWeights.z;
    modelNormal += (SkinningState.boneMatrices[boneIndices.w]*unskinnedNormal).xyz*inBoneWeights.w;

#   ifdef TANGENT_SPACE_ENABLED
    vec4 unskinnedTangent = vec4(inTangent4.xyz, 0.0);
    vec3 modelTangent = (SkinningState.boneMatrices[boneIndices.x]*unskinnedTangent).xyz*inBoneWeights.x;
    modelNormal += (SkinningState.boneMatrices[boneIndices.y]*unskinnedTangent).xyz*inBoneWeights.y;
    modelNormal += (SkinningState.boneMatrices[boneIndices.z]*unskinnedTangent).xyz*inBoneWeights.z;
    modelNormal += (SkinningState.boneMatrices[boneIndices.w]*unskinnedTangent).xyz*inBoneWeights.w;
#   endif
#else
    vec3 modelPosition = inPosition;
    vec3 modelNormal = inNormal;
#   ifdef TANGENT_SPACE_ENABLED
    vec3 modelTangent = inTangent4.xyz;
#   endif
#endif

    vec4 viewPosition = TransformationState.modelViewMatrix * vec4(modelPosition, 1.0);
    vec3 viewNormal = (TransformationState.modelViewMatrix * vec4(modelNormal, 0.0)).xyz;

#ifdef TANGENT_SPACE_ENABLED
    vec3 viewTangent = (TransformationState.modelViewMatrix * vec4(modelTangent, 0.0)).xyz;
    vec3 viewBitangent = cross(viewNormal, viewTangent) * inTangent4.w;
#endif

#if defined(LIGHTING_ENABLED) && defined(PER_VERTEX_LIGHTING)
    LightingParameters parameters;
    parameters.baseColor = vec4(1.0);
    parameters.emissionColor = MaterialState.emission;
    parameters.P = viewPosition.xyz;
    parameters.V = normalize(-viewPosition.xyz);
    parameters.N = normalize(viewNormal);
    vec4 color = computeLightingWith(parameters);
#else
    vec4 color = inColor;
#   if defined(FLAT_COLOR_MATERIAL)
    color *= MaterialState.color;
#   endif
#endif

    outColor = color;
    outTexcoord = TransformationState.textureMatrix*vec4(inTexcoord, 0.0, 1.0);
    //outTexcoord2 = TransformationState.textureMatrix*vec4(inTexcoord2, 0.0, 1.0);

    outPosition = viewPosition;
    outNormal = viewNormal;
#ifdef TANGENT_SPACE_ENABLED
    outTangent = viewTangent;
    outBitangent = viewBitangent;
#endif

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
layout(set=6, binding=0) uniform texture2D AlbedoTexture;
layout(set=6, binding=1) uniform texture2D EmissionTexture;
layout(set=6, binding=2) uniform texture2D NormalTexture;
layout(set=6, binding=3) uniform texture2D RMATexture;

layout(location = 0) OPT_FLAT in vec4 inColor;
layout(location = 1) in vec4 inTexcoord;
//layout(location = 2) in vec4 inTexcoord2;
layout(location = 3) in vec4 inPosition;
layout(location = 4) in vec3 inNormal;
#ifdef TANGENT_SPACE_ENABLED
layout(location = 5) in vec3 inTangent;
layout(location = 6) in vec3 inBitangent;
#endif

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 color = inColor;
#ifdef TEXTURING_ENABLED
    color *= textureProj(sampler2D(AlbedoTexture, Sampler0), inTexcoord);
#endif

    // Alpha cutoff.
#if defined(FLAT_COLOR_MATERIAL) || defined(LIGHTING_ENABLED)
    if(color.a < MaterialState.alphaCutoff)
        discard;
#endif

#if defined(LIGHTING_ENABLED) && defined(PER_FRAGMENT_LIGHTING)
    vec4 emissionColor = MaterialState.emission*inColor;
#   ifdef TEXTURING_ENABLED
    emissionColor *= textureProj(sampler2D(EmissionTexture, Sampler0), inTexcoord);
#   endif

    LightingParameters parameters;
    parameters.baseColor = color;
    parameters.emissionColor = emissionColor;

#   if defined(PBR_METALLIC_ROUGHNESS)
    parameters.baseColor *= MaterialState.baseColor;
    parameters.occlusion = MaterialState.occlusionFactor;
    parameters.roughness = MaterialState.roughnessFactor;
    parameters.metallic = MaterialState.metallicFactor;

#       ifdef TEXTURING_ENABLED
    vec4 rma = textureProj(sampler2D(RMATexture, Sampler0), inTexcoord);
    parameters.occlusion *= rma.r;
    parameters.roughness *= rma.g;
    parameters.metallic *= rma.b;
#       endif
#   endif
    parameters.P = inPosition.xyz;
    parameters.V = normalize(-inPosition.xyz);

#ifdef TANGENT_SPACE_ENABLED
    vec3 t = normalize(inTangent);
    vec3 b = normalize(inBitangent);
    vec3 n = normalize(inNormal);
    vec3 tangentNormal = textureProj(sampler2D(NormalTexture, Sampler0), inTexcoord).xyz*2.0 - 1.0;
    parameters.N = normalize(mat3(t,b,n)*tangentNormal);
#else
    parameters.N = normalize(inNormal);
#endif

    color = computeLightingWith(parameters);
#endif

    outColor = applyFog(color, inPosition.xyz);
}

#else
#error BUILD_VERTEX_SHADER or BUILD_FRAGMENT_SHADER must be defined.
#endif
)uberShader"
