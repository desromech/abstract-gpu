#version 450

layout(location = 0) flat in vec4 inColor;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = inColor;
}
