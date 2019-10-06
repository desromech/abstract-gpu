layout(set=4, binding=0) uniform TransformationStateBlock
{
    mat4 projectionMatrix;

    mat4 modelViewMatrix;
    mat4 inverseModelViewMatrix;

    mat4 textureMatrix;
} TransformationState;

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

layout(set=2, binding=0) uniform ExtraRenderingStateBlock
{
    vec4 userClipPlane;

    FogState fogState;
} ExtraRenderingState;
