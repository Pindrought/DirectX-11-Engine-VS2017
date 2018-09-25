struct PS_INPUT
{
    float4 inPos : SV_POSITION;
    float3 inColor : COLOR;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(input.inColor, 1);
}