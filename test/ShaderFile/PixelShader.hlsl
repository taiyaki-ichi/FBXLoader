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

struct LightStruct
{
	float3 position;

	float3 ambient;
	float3 diffuse;
	float3 specular;

	float constant;
	float liner;
	float quadratic;
};

float3 calcLight(LightStruct lightStruct, float3 pos, float3 normal,float3 viewDir,float3 textureColor)
{
	float3 lightDir = normalize(lightStruct.position - pos);

	float diff = max(dot(normal, lightDir), 0.0);
	float3 lightDiffuse = lightStruct.diffuse * (diff * diffuse.xyz) * textureColor;

	float3 reflectDir = reflect(-lightDir, normal);
	//shininess=32
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	float3 lightSpecular = lightStruct.specular * (spec * specular.xyz);

	float3 lightAmbient = lightStruct.ambient * ambient;

	float distance = length(lightStruct.position - pos);
	float atteniation = 1.0 / (lightStruct.constant + lightStruct.liner * distance + lightStruct.quadratic * (distance * distance));

	return (lightAmbient + lightDiffuse + lightSpecular);// *atteniation;

}


float4 main(DataType input) : SV_TARGET
{
	float3 lightPos[4];
	lightPos[0] = float3(1000, 1000, 1000);
	lightPos[1] = float3(-1000, 1000, 1000);
	lightPos[2] = float3(1000, 1000, -1000);
	lightPos[3] = float3(-1000, 1000, -1000);

	LightStruct lightStruct;
	lightStruct.ambient = float3(0.2, 0.2, 0.2);
	lightStruct.diffuse = float3(0.5, 0.5, 0.5);
	lightStruct.specular = float3(1.0, 1.0, 1.0);
	lightStruct.constant = 0.1;
	lightStruct.liner = 0.09;
	lightStruct.quadratic = 0.0032;

	float3 n = normalize(input.normal).xyz;
	//target=0,0,0
	float3 viewDir = normalize(eye - input.pos).xyz;

	float3 textureColor=tex.Sample(smp, input.uv);

	float3 color = float3(0, 0, 0);
	for (int i = 0; i < 4; i++)
	{
		lightStruct.position = lightPos[i];
		color += calcLight(lightStruct, input.pos, n, viewDir, textureColor);
	}

	return float4(color, 1);

}