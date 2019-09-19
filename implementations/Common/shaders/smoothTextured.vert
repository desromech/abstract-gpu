#version 450
#include "common_state.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 3) in vec2 inTexcoord;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outTexcoord;

void main()
{
    outColor = inColor;
    outTexcoord = matrices[textureMatrixIndex]*vec4(inTexcoord, 0.0, 1.0);

    gl_Position = matrices[projectionMatrixIndex] * (matrices[modelViewMatrixIndex] * vec4(inPosition, 1.0));
}
