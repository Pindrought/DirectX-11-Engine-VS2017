#pragma pack_matrix( row_major ) //Set matrices to be in row_major format so we don't have to transpose in our C++ code

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
}; 

struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outWorldPos : WORLDPOSITION;
};

VS_OUTPUT main(VS_INPUT input) 
{
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPos, 1.0f), wvpMatrix);
    output.outTexCoord = input.inTexCoord;
    output.outNormal = mul(float4(input.inNormal, 1.0f), worldMatrix);
    output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMatrix);

    return output;
}