layout(set=1, binding=0) buffer MatrixBuffer
{
    mat4 matrices[];
};

layout(set=1, binding=3) buffer UserClipPlanesBuffer
{
    vec4 userClipPlanes[];
};

layout(std140, push_constant) uniform PushConstants
{
    uint projectionMatrixIndex;
    uint modelViewMatrixIndex;
    uint textureMatrixIndex;
    uint lightingStateIndex;
    uint materialStateIndex;
    uint clipPlaneIndex;
};
