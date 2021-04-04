
struct DataType
{
	float4 svpos:SV_POSITION;//システム用頂点座標
	float4 pos:POSITION;//システム用頂点座標
	float4 normal:NORMAL0;//法線ベクトル
	float2 uv:TEXCOORD;//UV値
	float3 ray:VECTOR;
};

cbuffer SceneData : register(b0) {
	matrix view;
	matrix proj;
	float3 eye;
};