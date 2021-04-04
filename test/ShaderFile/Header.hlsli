
struct DataType
{
	float4 svpos:SV_POSITION;//�V�X�e���p���_���W
	float4 pos:POSITION;//�V�X�e���p���_���W
	float4 normal:NORMAL0;//�@���x�N�g��
	float2 uv:TEXCOORD;//UV�l
	float3 ray:VECTOR;
};

cbuffer SceneData : register(b0) {
	matrix view;
	matrix proj;
	float3 eye;
};