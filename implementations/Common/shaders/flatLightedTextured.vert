#version 450
#include "common_state.glsl"

layout(location = 3) in vec2 inTexcoord;

#include "lighting.glsl"

layout(location = 0) flat out vec4 outColor;
layout(location = 1) out vec4 outTexcoord;
layout(location = 2) out vec4 outPosition;

void main()
{
    outColor = computingLighting();
    outTexcoord = matrices[textureMatrixIndex]*vec4(inTexcoord, 0.0, 1.0);

    outPosition = matrices[modelViewMatrixIndex] * vec4(inPosition, 1.0);
    gl_ClipDistance[0] = dot(extraRenderingStates[extraRenderingStateIndex].userClipPlane, outPosition);
    gl_Position = matrices[projectionMatrixIndex] * outPosition;
}
