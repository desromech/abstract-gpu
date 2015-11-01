#version 420
#pragma agpu sampler_binding diffuseTexture 0

uniform sampler2D diffuseTexture;

in vec4 fColor;
in vec2 fTexCoord;

void main()
{
    gl_FragData[0] = fColor*texture(diffuseTexture, fTexCoord);
}
