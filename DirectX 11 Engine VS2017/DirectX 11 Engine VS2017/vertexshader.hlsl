cbuffer cbPerObject : register(b0)
{
    float4x4 rotationMat;
};

struct VS_INPUT
{
    float2 inPos : POSITION;
    float3 inColor : COLOR;
};

struct VS_OUTPUT
{
    float4 outPos : SV_POSITION;
    float3 outColor : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPos = float4(input.inPos, 0, 1);
    output.outPos = mul(output.outPos, rotationMat);
    output.outColor = input.inColor;
    return output;
}