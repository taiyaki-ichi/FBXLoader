#include"Header.hlsli"


DataType main(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD) 
{
	pos = mul(world, pos);

	DataType output;
	output.svpos = mul(mul(proj, view), pos);
	output.pos = pos;
	normal.w = 0;
	output.normal = mul(world, normalize(normal));
	output.uv = uv;
	output.ray = normalize(pos.xyz - mul(view, eye));

	return output;
}