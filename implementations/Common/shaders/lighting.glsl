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

struct MaterialState
{
    vec4 emission;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float shininess;
    uint padding1;
    uint padding2;
    uint padding3;
};

layout(std140, set=1, binding=2) buffer MaterialStateBuffer
{
    MaterialState materialStates[];
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inNormal;

struct LightingParameters
{
    vec3 P;
    vec3 V;
    vec3 N;
};

vec4 computeLightContributionWith(in MaterialState material, in LightState light, in LightingParameters parameters)
{
    // Compute the ambient contribution.
    vec4 lightContribution = material.ambient*light.ambientColor;

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
            lightContribution += (attenuation*NdotL) * material.diffuse * light.diffuseColor;

            // Compute the specular contribution.
            vec3 H = normalize(V + L);
            float NdotH = max(dot(N, H), 0.0);
            if(NdotH > 0.0)
            {
                lightContribution += (attenuation * pow(NdotH, material.shininess)) * material.specular * light.specularColor;
            }
        }
    }

    return lightContribution;
}

vec4 computingLightingWith(in MaterialState material, in LightingState lighting, in LightingParameters parameters)
{
    vec4 color = material.emission +
        material.ambient*lighting.ambientLighting;

    uint enabledLightMask = lighting.enabledLightMask;
    for(int i = 0; i < 8 && (enabledLightMask != 0); ++i, enabledLightMask >>=1)
    {
        if((enabledLightMask & 1u) == 0)
            continue;

        color += computeLightContributionWith(material, lighting.lights[i], parameters);
    }

    return clamp(color, 0.0, 1.0);
}

vec4 computingLighting()
{
    LightingParameters parameters;
    parameters.P = (matrices[modelViewMatrixIndex] * vec4(inPosition, 1.0)).xyz;
    parameters.V = normalize(-parameters.P);
    parameters.N = normalize((matrices[modelViewMatrixIndex] * vec4(inNormal, 0.0)).xyz);
    return computingLightingWith(materialStates[materialStateIndex], lightingStates[lightingStateIndex], parameters);
}
