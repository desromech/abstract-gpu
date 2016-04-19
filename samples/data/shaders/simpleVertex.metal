using namespace metal;

struct TransformationState
{
    float4x4 projectionMatrix;
    float4x4 viewMatrix;
    float4x4 modelMatrix;
};

struct VertexInput
{
    float4 position [[attribute(0)]];
    float4 color [[attribute(1)]];
};

struct VertexOutput
{
    float4 color;
    float4 position [[position]];
};

vertex VertexOutput agpu_main(VertexInput input [[stage_in]],
    constant TransformationState *transformationState [[buffer(1)]])
{
    VertexOutput out;
    out.position = input.position;
    out.color = input.color;
    return out;
}
