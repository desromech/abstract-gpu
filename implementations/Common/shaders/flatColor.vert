#version 450
#include "common_state.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) flat out vec4 outColor;
layout(location = 2) out vec4 outPosition;

void main()
{
    outColor = inColor;

    outPosition = TransformationState.modelViewMatrix * vec4(inPosition, 1.0);
    gl_ClipDistance[0] = dot(ExtraRenderingState.userClipPlane, outPosition);
    gl_Position = TransformationState.projectionMatrix * outPosition;
}
