using namespace metal;

struct TransformationState
{
    float4x4 projectionMatrix;
    float4x4 viewMatrix;
    float4x4 modelMatrix;
};

struct VertexInput
{
    float3 position [[attribute(0)]];
    float4 color [[attribute(1)]];
    float2 texcoord [[attribute(3)]];
};

struct VertexOutput
{
    float4 position [[position]];
    float4 color;
    float2 texcoord;
};

vertex VertexOutput agpu_main(VertexInput input [[stage_in]],
    constant TransformationState *transformationState [[buffer(1)]])
{
    VertexOutput out;
    constant auto &ts = *transformationState;
    out.position = ts.projectionMatrix * ts.viewMatrix * ts.modelMatrix * float4(input.position, 1.0);
    out.color = input.color;
    out.texcoord = input.texcoord;
    return out;
}
