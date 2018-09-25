struct VS_INPUT
{
    float2 inPos : POSITION;
    float3 inColor : COLOR;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outColor : COLOR;
};

VS_OUTPUT main(VS_INPUT input) 
{
    VS_OUTPUT output;
    output.outPosition = float4(input.inPos, 0.0f, 1.0f);
    output.outColor = input.inColor;
    return output;
}