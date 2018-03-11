#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set=1, binding = 0) uniform texture2D diffuseTexture;
layout (set=2, binding = 0) uniform sampler diffuseSampler;

layout (location = 0) in vec4 fColor;
layout (location = 1) in vec2 fTexCoord;

layout (location = 0) out vec4 fbColor;

void main()
{
    vec4 color = fColor * texture(sampler2D(diffuseTexture, diffuseSampler), fTexCoord);
    fbColor = color;
}
