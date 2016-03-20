#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#ifdef VULKAN
layout (set=1, binding = 0) uniform texture2D diffuseTexture;
layout (set=2, binding = 0) uniform sampler diffuseSampler;
#else
#pragma agpu sampler_binding diffuseTexture 0
uniform sampler2D diffuseTexture;
#endif

layout (location = 0) in vec4 fColor;
layout (location = 1) in vec2 fTexCoord;

layout (location = 0) out vec4 fbColor;

void main()
{
    vec4 color = fColor;
#ifdef VULKAN
    color *= texture2D(sampler2D(diffuseTexture, diffuseSampler), fTexCoord);
#else
    color *= texture(diffuseTexture, fTexCoord);
#endif
    fbColor = color;
}
