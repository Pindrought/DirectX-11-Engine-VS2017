struct dynamicLight
{
    float3 color;
    float strength;
    float3 position;
    float padding2;
};
cbuffer lightBuffer : register(b0)
{
    float3 ambientLightColor;
    float ambientLightStrength;
    float dynamicLightCount;
    float3 padding; //<--padding due to 16 byte alignment
    dynamicLight lights[10];
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WORLD_POSITION;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    //float3 sampleColor = input.inNormal;
    float3 ambientLight = ambientLightColor * ambientLightStrength; //ambient light

    float3 appliedLight = ambientLight;

    for (int i = 0; i < dynamicLightCount; i++)
    {
        float3 vectorToLight = normalize(lights[i].position - input.inWorldPos); //Normalized vector from pixel to light
        float3 diffuseLightRatio = max(dot(input.inNormal, vectorToLight), 0);
        float3 diffuseLight = diffuseLightRatio * lights[i].strength * lights[i].color;
        appliedLight += diffuseLight;
    }
    
    //appliedLight = saturate(appliedLight);

    float3 finalColor = sampleColor * appliedLight;

    return float4(finalColor, 1.0f);
}