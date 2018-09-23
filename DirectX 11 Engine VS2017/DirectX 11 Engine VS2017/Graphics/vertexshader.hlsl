float4 main(float4 inPos : POSITION, row_major float4x4 instancemat : INSTANCEMAT) : SV_POSITION
{
	float4 pos = mul(inPos, instancemat);
	return pos;
}