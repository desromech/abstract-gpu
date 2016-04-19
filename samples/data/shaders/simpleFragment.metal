using namespace metal;

struct VertexOutput
{
    float4 position [[position]];
    float4 color;
};

fragment float4 agpu_main(VertexOutput input [[stage_in]])
{
    return input.color;
}
