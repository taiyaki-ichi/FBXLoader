#include"Header.hlsli"

cbuffer Material : register(b1) {
	float4 diffuse;
	float4 specular;
	float3 ambient;
};

Texture2D<float4> tex:register(t0);

SamplerState smp:register(s0);

float4 main(DataType data) : SV_TARGET
{
	float3 lightVec = normalize(float3(1,0,0));

	float4 texColor = tex.Sample(smp, data.uv);

	float3 refLight = normalize(reflect(lightVec, data.normal.xyz));
	float specularB = pow(saturate(dot(refLight, -data.ray)), specular.a);

	return max(saturate(diffuse * texColor) + saturate(float4(specularB * specular.rgb, 1)), float4(ambient * texColor, 1));
}