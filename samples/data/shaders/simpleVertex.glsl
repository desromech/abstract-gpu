#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#ifdef VULKAN
layout(std140, set = 0, binding = 0) uniform TransformationBuffer
#else
layout(std140, binding = 0) uniform TransformationBuffer
#endif
{
    mat4 projectionMatrix;
    mat4 modelMatrix;
    mat4 viewMatrix;
};

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec4 vColor;

layout(location = 0) out vec4 fColor;

void main()
{
    fColor = vColor;
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vPosition, 1.0);
}
