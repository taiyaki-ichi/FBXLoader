
cbuffer SceneData : register(b0) {
	matrix view;
	matrix proj;
};

float4 main(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD) : SV_POSITION
{
	return mul(mul(proj, view), pos);
}