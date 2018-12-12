cbuffer lightbuffer : register(b0)
{
    float3 ambientLightColor;
    float ambientLightStrength;
    float3 dynamicLightColor;
    float dynamicLightDiffuseStrength;
    float3 dynamicLightPosition;
    float dynamicLightSpecularStrength;
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
    
    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    float3 lightDirection = normalize(input.inWorldPos - dynamicLightPosition); //Normalized Vector from light->pixel
    float3 viewDirection = normalize(viewPos - input.inWorldPos);
    float3 reflectDirection = reflect(lightDirection, input.inNormal);

    float3 ambientLight = ambientLightColor * ambientLightStrength;

    float3 diffuseLight = max(dot(input.inNormal, -lightDirection), 0.0) * dynamicLightColor * dynamicLightDiffuseStrength;
    
    float specularColor = pow(max(dot(viewDirection, reflectDirection), 0.0), 8);
    float3 specularLight = dynamicLightSpecularStrength * specularColor * dynamicLightColor;
    
    float3 lightColor = saturate(ambientLight + diffuseLight) + specularLight;

    float3 finalColor = (lightColor) * sampleColor;
    return float4(finalColor, 1.0f);
}
