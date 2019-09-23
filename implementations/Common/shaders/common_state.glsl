layout(set=1, binding=0) buffer MatrixBuffer
{
    mat4 matrices[];
};

const uint FogMode_None = 0;
const uint FogMode_Linear = 1;
const uint FogMode_Exponential = 2;
const uint FogMode_ExponentialSquared = 3;

struct FogState
{
    uint mode;
    float startDistance;
    float endDistance;
    float density;

    vec4 color;
};

struct ExtraRenderingState
{
    vec4 userClipPlane;

    FogState fogState;

};

layout(set=1, binding=3) buffer UserClipPlanesBuffer
{
    ExtraRenderingState extraRenderingStates[];
};

layout(std140, push_constant) uniform PushConstants
{
    uint projectionMatrixIndex;
    uint modelViewMatrixIndex;
    uint textureMatrixIndex;
    uint lightingStateIndex;
    uint materialStateIndex;
    uint extraRenderingStateIndex;
};
