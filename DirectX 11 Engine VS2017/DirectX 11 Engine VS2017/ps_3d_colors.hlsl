struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float4 inColor : COLOR;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return input.inColor;
    //return float4(float3(input.inColor.r, input.inColor.g, input.inColor.b), 0.5f);
}