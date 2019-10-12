#version 450
#include "common_state.glsl"

#include "lighting.glsl"

layout(location = 0) flat out vec4 outColor;
layout(location = 2) out vec4 outPosition;


void main()
{
    outColor = computingLighting();

    outPosition = TransformationState.modelViewMatrix * vec4(inPosition, 1.0);
    gl_ClipDistance[0] = dot(ExtraRenderingState.userClipPlane, outPosition);
    gl_Position = TransformationState.projectionMatrix * outPosition;
}
