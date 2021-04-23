#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) out vec2 outTexcoord;

const vec2 screenQuadVertices[3] = vec2[3](
    vec2(-1.0, -1.0),
    vec2(-1.0, 3.0),
    vec2(3.0, -1.0)
);

void main()
{
    outTexcoord = screenQuadVertices[gl_VertexIndex]*0.5 + 0.5;
    gl_Position = vec4(screenQuadVertices[gl_VertexIndex], 0.0, 1.0);
}
