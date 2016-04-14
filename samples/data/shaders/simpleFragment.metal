using namespace metal;

struct VertexOutput
{
    float4 color;
    float4 position [[position]];
};

fragment float4 agpu_main(VertexOutput input [[stage_in]])
{
    return input.color;
}
