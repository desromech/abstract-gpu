#version 130
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

in vec3 vPosition;
in vec4 vColor;

out vec4 fColor;

void main()
{
    fColor = vColor;
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vPosition, 1.0);
}

