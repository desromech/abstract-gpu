#version 450

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 inTexcoord;

layout(location = 0) out vec4 outColor;

layout(set=0, binding=0) uniform sampler Sampler0;
layout(set=2, binding=0) uniform texture2D Texture0;

void main()
{
    outColor = inColor*texture(sampler2D(Texture0, Sampler0), inTexcoord);
}
