#pragma once
#include<variant>
#include<vector>
#include<string>
#include<optional>
#include<type_traits>
#include<unordered_map>

namespace FBXL
{
	
	template<typename Vector2D,typename Vector3D>
	struct Vertex
	{
		Vector3D position;
		Vector3D normal;
		Vector2D uv;
	};

	template<typename Vector3D>
	struct Material
	{
		Vector3D diffuseColor;
		double diffuseFactor;
		std::optional<std::string> diffuseColorTexturePath;

		Vector3D ambientColor;
		double ambientFactor;

		Vector3D specularColor;
		double specularFactor;

		//Phong�̔��˃��f���Q��
		//�e������
		double shininess;
		double shininessFactor;

		//���g������
		Vector3D emissive;
		double emissiveFactor;

		//���ߓx
		//alpha�����̂ݍ�p
		double transparent;
		double transparentFactor;

		Vector3D reflectionColor;
		double reflectionFactor;

		//BumpMap,NormapMap�Ƃ��ǉ����邩��
	};

	template<typename Vector2D, typename Vector3D>
	struct Model3D2
	{
		std::vector<std::size_t> indeces{};
		std::vector<Vertex<Vector2D, Vector3D>> vertices;
		std::vector<std::size_t> materialRange{};

		std::vector<Material<Vector3D>> material{};
	};

}