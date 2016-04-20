using namespace metal;

struct VertexOutput
{
    float4 position [[position]];
    float4 color;
    float2 texcoord;
};

fragment float4 agpu_main(VertexOutput input [[stage_in]],
    texture2d<float> diffuseTexture [[texture(0)]],
    sampler mainSampler [[sampler(0)]])
{
    float4 textureColor = diffuseTexture.sample(mainSampler, input.texcoord);
    return input.color * textureColor;
}
