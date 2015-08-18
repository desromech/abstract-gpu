#version 420
in vec4 fColor;

void main()
{
    gl_FragData[0] = fColor;
}

