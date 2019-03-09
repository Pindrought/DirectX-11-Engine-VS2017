struct PS_INPUT
{
	float4 inPosition : SV_POSITION;
	float2 inTexCoord : TEXCOORD;
};

Texture2D objTexture : TEXTURE: register(t0);
SamplerState objSamplerState : SAMPLER: register(s0);

void main(PS_INPUT input)
{
	float4 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
	if (sampleColor.a < 0.25)
		discard;
}