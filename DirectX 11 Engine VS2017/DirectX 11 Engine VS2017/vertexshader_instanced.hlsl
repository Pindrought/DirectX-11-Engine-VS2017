cbuffer mycBuffer : register(b0)
{
    row_major float4x4 mat;
}; 

struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    row_major float4x4 instanceMat : INSTANCEMAT;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPos, 1.0f), input.instanceMat);
    output.outPosition = mul(output.outPosition, mat);

    output.outTexCoord = input.inTexCoord;
    return output;
}