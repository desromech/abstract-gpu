#version 450

layout(std430, binding = 0) buffer Left
{
    float left[];
};

layout(std430, binding = 1) buffer Right
{
    float right[];
};

layout(std430, binding = 2) buffer Result
{
    float result[];
};

void main()
{
    uint i = gl_GlobalInvocationID.x;
    result[i] = left[i] + right[i];
}
