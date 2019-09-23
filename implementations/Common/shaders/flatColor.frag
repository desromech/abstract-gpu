#version 450

#include "fog.glsl"

layout(location = 0) flat in vec4 inColor;
layout(location = 2) in vec4 inPosition;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = applyFog(inColor, inPosition.xyz);
}
