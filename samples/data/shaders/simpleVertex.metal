using namespace metal;

struct TransformationState
{
    float4x4 projectionMatrix;
    float4x4 viewMatrix;
    float4x4 modelMatrix;
};

struct Vertex
{
    float4 position;
    float4 normal;
    float4 color;
    float2 texcoord;
};

struct VertexOutput
{
    float4 color;
    float4 position [[position]];
};

vertex VertexOutput agpu_main(constant TransformationState *transformationState [[buffer(0)]],
    constant Vertex *vertexInput [[buffer(1)]],
    uint vertexId [[vertex_id]])
{
    Vertex input = vertexInput[vertexId];
    VertexOutput out;
    out.position = input.position;
    out.color = input.color;
    return out;
}
