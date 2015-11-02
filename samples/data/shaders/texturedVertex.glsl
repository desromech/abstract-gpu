#version 400

#pragma agpu uniform_binding TransformationBuffer 0

#pragma agpu attribute_location vPosition 0
#pragma agpu attribute_location vColor 1
#pragma agpu attribute_location vTexCoord 3

layout(std140) uniform TransformationBuffer
{
    mat4 projectionMatrix;
    mat4 modelMatrix;
    mat4 viewMatrix;
};

in vec3 vPosition;
in vec4 vColor;
in vec2 vTexCoord;

out vec4 fColor;
out vec2 fTexCoord;

void main()
{
    fColor = vColor;
    fTexCoord = vTexCoord;
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vPosition, 1.0);
}
