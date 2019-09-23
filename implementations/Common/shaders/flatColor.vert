#version 450
#include "common_state.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) flat out vec4 outColor;
layout(location = 2) out vec4 outPosition;

void main()
{
    outColor = inColor;

    outPosition = matrices[modelViewMatrixIndex] * vec4(inPosition, 1.0);
    gl_ClipDistance[0] = dot(extraRenderingStates[extraRenderingStateIndex].userClipPlane, outPosition);
    gl_Position = matrices[projectionMatrixIndex] * outPosition;
}
