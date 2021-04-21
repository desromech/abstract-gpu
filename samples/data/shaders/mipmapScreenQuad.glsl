#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set=0, binding = 0) uniform sampler inputSample;
layout (set=1, binding = 0) uniform texture2D inputTexture;

layout (location = 0) in vec2 inTexcoord;
layout (location = 0) out vec4 outMiptexel;

void main()
{
    outMiptexel = texture(sampler2D(inputTexture, inputSample), inTexcoord);
}
