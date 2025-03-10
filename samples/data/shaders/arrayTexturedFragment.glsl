#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set=1, binding = 0) uniform texture2D AllTextures[1024];
layout (set=2, binding = 0) uniform sampler TextureSampler;

layout (location = 0) in vec4 fColor;
layout (location = 1) in vec2 fTexCoord;

layout (location = 0) out vec4 fbColor;

void main()
{
    vec4 color = fColor * texture(sampler2D(AllTextures[0], TextureSampler), fTexCoord);
    fbColor = color;
}
