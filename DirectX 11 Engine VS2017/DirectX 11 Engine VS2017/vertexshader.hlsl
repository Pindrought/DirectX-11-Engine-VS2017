cbuffer cbPerVertex : register(b0)
{
    float2 offset;
}

struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input) 
{
    VS_OUTPUT output;
    input.inPos.x += offset.x;
    input.inPos.y += offset.y;
    output.outPosition = float4(input.inPos, 1.0f);
    output.outTexCoord = input.inTexCoord;
    return output;
}