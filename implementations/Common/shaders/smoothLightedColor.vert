#version 450
#include "common_state.glsl"

#include "lighting.glsl"

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = computingLighting();

    gl_Position = matrices[projectionMatrixIndex] * (matrices[modelViewMatrixIndex] * vec4(inPosition, 1.0));
}
