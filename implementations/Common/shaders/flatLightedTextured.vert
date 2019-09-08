#version 450
#include "common_state.glsl"

layout(location = 3) in vec2 inTexcoord;

#include "lighting.glsl"

layout(location = 0) flat out vec4 outColor;
layout(location = 1) out vec2 outTexcoord;

void main()
{
    outColor = computingLighting();
    outTexcoord = inTexcoord;

    gl_Position = matrices[projectionMatrixIndex] * (matrices[modelViewMatrixIndex] * vec4(inPosition, 1.0));
}
