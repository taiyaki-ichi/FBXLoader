#include"Header.hlsli"

cbuffer Material : register(b1) {
	float4 diffuse;
	float diffuseFactor;
	float4 specular;
	float specilarFactor;
	float3 ambient;
	float ambientFactor;
};

Texture2D<float4> tex:register(t0);

SamplerState smp:register(s0);

float4 main(DataType input) : SV_TARGET
{
	float3 lightVec[4];
    lightVec[0] = float3(1, -1, 1);
	lightVec[1] = float3(1, -1, -1);
	lightVec[2] = float3(-1, -1, 1);
	lightVec[3] = float3(-1,-1, -1);

	float3 lightColor = float3(1, 1, 1);

	float4 result = float4(0, 0, 0, 1);

	for (int i = 0; i < 4; i++)
	{
		float diffuseB = dot(-lightVec[i], input.normal);

		float3 refLight = normalize(reflect(lightVec[i], input.normal.xyz));
		float specularB = pow(saturate(dot(refLight, -input.ray)), specular.a);

		result += diffuse * diffuseB;
	}

	if (result.x > 1)
		result.x = 1;
	if (result.y > 1)
		result.y = 1;
	if (result.z > 1)
		result.z = 1;
	if (result.w > 1)
		result.w = 1;

	return result;

	//return float4(input.normal.xyz, 1);
}