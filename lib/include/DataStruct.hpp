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
		std::vector<unsigned char>//バイナリデータ
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
	//以下、Objects内の構造体
	//どれも仮の姿
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

		//Phongの反射モデル参照
		//影つけるやつ
		double shininess;
		double shininessFactor;

		//自身が放つ光
		Vector3D emissive;
		double emissiveFactor;
		std::optional<std::string> emissiveTexturePath;

		//透過度
		//alpha成分のみ作用
		double transparent;
		double transparentFactor;

		Vector3D reflectionColor;
		double reflectionFactor;
		std::optional<std::string> reflectionColorTexturePath;

		//BumpMap,NormapMapとか追加するかも
	};


	template<typename Vector2D,typename Vector3D>
	struct Objects
	{
		std::unordered_map<std::int64_t, GeometryMesh<Vector2D, Vector3D>> geometryMeshes{};
		std::unordered_map<std::int64_t, ModelMesh<Vector3D>> modelMeshes{};
		std::unordered_map<std::int64_t, Material<Vector3D>> materials{};
	};

	
	//Connectionsの構造体
	using ObjectOrPropertyIndex = std::variant<std::int64_t, std::pair<std::int64_t, std::string>>;
	using Connections = std::vector<std::pair<ObjectOrPropertyIndex, ObjectOrPropertyIndex>>;


	//GeometryMeshにModelMeshのローカルの変換を適用した
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