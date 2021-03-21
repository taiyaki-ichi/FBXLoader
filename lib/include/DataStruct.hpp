#pragma once
#include<variant>
#include<vector>
#include<string>
#include<optional>
#include<type_traits>
#include<unordered_map>

namespace FBXL
{
	using DataVariant = std::variant <
		std::int16_t,
		bool,
		std::int32_t,
		float,
		double,
		std::int64_t,
		std::vector<float>,
		std::vector<double>,
		std::vector<std::int64_t>,
		std::vector<std::int32_t>,
		std::vector<bool>,
		std::string,
		std::vector<unsigned char>//�o�C�i���f�[�^
	>;


	struct Node {
		std::string name{};
		std::vector<DataVariant> properties{};
		std::vector<Node> children{};
	};


	struct PrimitiveData {
		std::string filePath{};
		std::uint32_t version{};
		std::vector<Node> nodes{};
	};



	//
	//�ȉ��AObjects���̍\����
	//�ǂ�����̎p
	//

	
	template<typename Vector2D,typename Vector3D>
	struct Vertex
	{
		Vector3D position;
		Vector3D normal;
		Vector2D uv;
	};
	
	template<typename Vector2D,typename Vector3D>
	struct GeometryMesh
	{
		std::vector<Vertex<Vector2D,Vector3D>> vertices;
		std::vector<std::int32_t> materialRange{};
	};



	template<typename Vector3D>
	struct ModelMesh
	{
		Vector3D localTranslation;
		Vector3D localRotation;
		Vector3D localScaling;
	};


	template<typename Vector3D>
	struct Material
	{
		Vector3D diffuseColor;
		double diffuseFactor;
		std::optional<std::string> diffuseColorTexturePath;

		Vector3D ambientColor;
		double ambientFactor;
		std::optional<std::string> ambientColorTexturePath;

		Vector3D specularColor;
		double specularFactor;
		std::optional<std::string> specularColorTexturePath;

		//Phong�̔��˃��f���Q��
		//�e������
		double shininess;
		double shininessFactor;

		//���g������
		Vector3D emissive;
		double emissiveFactor;
		std::optional<std::string> emissiveTexturePath;

		//���ߓx
		//alpha�����̂ݍ�p
		double transparent;
		double transparentFactor;

		Vector3D reflectionColor;
		double reflectionFactor;
		std::optional<std::string> reflectionColorTexturePath;

		//BumpMap,NormapMap�Ƃ��ǉ����邩��
	};


	template<typename Vector2D,typename Vector3D>
	struct Objects
	{
		std::unordered_map<std::int64_t, GeometryMesh<Vector2D, Vector3D>> geometryMeshes{};
		std::unordered_map<std::int64_t, ModelMesh<Vector3D>> modelMeshes{};
		std::unordered_map<std::int64_t, Material<Vector3D>> materials{};
	};

	
	//Connections�̍\����
	using ObjectOrPropertyIndex = std::variant<std::int64_t, std::pair<std::int64_t, std::string>>;
	using Connections = std::vector<std::pair<ObjectOrPropertyIndex, ObjectOrPropertyIndex>>;


	//GeometryMesh��ModelMesh�̃��[�J���̕ϊ���K�p����
	template<typename Vector2D,typename Vector3D>
	using Model3DParts = std::pair<GeometryMesh<Vector2D, Vector3D>, std::vector<std::int64_t>>;


	template<typename Vector2D,typename Vector3D>
	struct Model3D
	{
		std::vector<Vertex<Vector2D, Vector3D>> vertices;
		std::vector<std::int32_t> materialRange{};

		std::vector<Material<Vector3D>> material{};
	};

}