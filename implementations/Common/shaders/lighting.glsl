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

struct LightingState
{
    vec4 ambientLighting;
    
    uint enabledLightMask;
    uint padding1;
    uint padding2;
    uint padding3;
    
    LightState lights[8];
};

layout(std140, set=1, binding=1) buffer LightingStateBuffer
{
    LightingState lightingStates[];
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inNormal;

vec4 computingLighting()
{
    vec3 P = (matrices[modelViewMatrixIndex] * vec4(inPosition, 1.0)).xyz;
    vec3 V = normalize(-P);
    vec3 N = normalize((matrices[modelViewMatrixIndex] * vec4(inNormal, 0.0)).xyz);
    vec3 L = normalize(vec3(1.0, 1.0, 1.0));
    
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = abs(dot(N, V));
    return vec4(inColor.rgb * (0.2 + 0.8*NdotL), inColor.a);
    //return vec4(N*0.5 + 0.5, 1.0);
}
