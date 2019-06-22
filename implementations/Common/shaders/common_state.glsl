layout(set=1, binding=0) buffer MatrixBuffer
{
    mat4 matrices[];
};

layout(push_constant) uniform PushConstants
{
    uint projectionMatrixIndex;
    uint modelViewMatrixIndex;
};
