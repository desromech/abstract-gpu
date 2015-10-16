Texture2D diffuseTexture : register(t0);
SamplerState diffuseSampler : register(s0);

struct FragmentInput
{
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

float4 main(FragmentInput input) : SV_TARGET
{
    float4 textureColor = diffuseTexture.Sample(diffuseSampler, input.texcoord);
    return textureColor;
    //return input.color*float4(input.texcoord, 0.0, 1.0);//*textureColor;
}
