#version 420
uniform sampler2D diffuseTexture;

in vec4 fColor;
in vec2 fTexCoord;

void main()
{
    gl_FragData[0] = fColor*texture(diffuseTexture, fTexCoord);
}
