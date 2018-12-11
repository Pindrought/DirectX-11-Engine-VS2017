cbuffer lightbuffer : register(b0)
{
    float3 ambientLightColor;
    float ambientLightStrength;
    float3 directionalLightColor;
    float directionalLightStrength;
    float3 directionalLightDirection;
    float padding;
    float3 viewPos; //camera position
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WORLDPOSITION;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 viewDirection = normalize(viewPos - input.inWorldPos);
    float3 reflectDirection = reflect(directionalLightDirection, input.inNormal);
    float specularColor = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
    float3 specular = 1.0 * specularColor * directionalLightColor;

    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    float3 ambientLight = ambientLightColor * ambientLightStrength;
    float3 directionalLight = max(dot(input.inNormal, -directionalLightDirection), 0.0) * directionalLightColor * directionalLightStrength;
    float3 lightColor = ambientLight + specular;
    float3 finalColor = (lightColor) * sampleColor;
    return float4(finalColor, 1.0f);
}