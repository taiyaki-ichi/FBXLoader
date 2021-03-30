#include"Header.hlsli"

cbuffer Material : register(b1) {
	float4 diffuse;
	float4 specular;
	float3 ambient;
};

float4 main(DataType data) : SV_TARGET
{
	float3 lightVec = normalize(float3(1,0,0));

	float3 refLight = normalize(reflect(lightVec, data.normal.xyz));
	float specularB = pow(saturate(dot(refLight, -data.ray)), specular.a);

	return max(saturate(diffuse) + saturate(float4(specularB * specular.rgb, 1)), float4(ambient, 1));
}