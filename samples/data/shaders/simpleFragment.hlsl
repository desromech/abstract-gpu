struct FragmentInput
{
    float4 color : COLOR;
};

float4 main(FragmentInput input) : SV_TARGET
{
    return input.color;
}
