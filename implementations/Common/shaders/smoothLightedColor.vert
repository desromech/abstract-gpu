#version 450
#include "common_state.glsl"

#include "lighting.glsl"

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = computingLighting();

    vec4 viewPosition = matrices[modelViewMatrixIndex] * vec4(inPosition, 1.0);
    gl_ClipDistance[0] = dot(userClipPlanes[clipPlaneIndex], viewPosition);
    gl_Position = matrices[projectionMatrixIndex] * viewPosition;
}
