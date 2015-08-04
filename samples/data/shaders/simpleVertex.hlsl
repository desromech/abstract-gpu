cbuffer TransformationBuffer : register(b0)
{
    matrix projectionMatrix;
    matrix modelMatrix;
    matrix viewMatrix;
}

struct VertexInput
{
    float3 position : A;
    float4 color : B;
};

struct VertexOutput
{
    float4 color : COLOR;
    float4 position : SV_POSITION;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.color = input.color;
    output.position = mul(projectionMatrix, mul(viewMatrix, mul(modelMatrix, float4(input.position, 1.0))));
    return output;
}
