#pragma once
#include<variant>
#include<vector>
#include<string>
#include<unordered_map>
#include"include/DataStruct.hpp"

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


	template<typename Vector2D, typename Vector3D>
	struct GeometryMesh
	{
		std::vector<Vertex<Vector2D, Vector3D>> vertices;
		std::vector<std::int32_t> materialRange{};
	};

	template<typename Vector3D>
	struct ModelMesh
	{
		std::optional<Vector3D> localTranslation = std::nullopt;
		std::optional<Vector3D> localRotation = std::nullopt;
		std::optional<Vector3D> localScaling = std::nullopt;
	};

	struct Texture
	{
		std::string fileName{};
		std::string relativeFileName{};
	};


	template<typename Vector2D, typename Vector3D>
	using Objects = std::tuple<
		std::unordered_map<std::int64_t, ModelMesh<Vector3D>>,
		std::unordered_map<std::int64_t, GeometryMesh<Vector2D, Vector3D>>,
		std::unordered_map<std::int64_t, Material<Vector3D>>,
		std::unordered_map<std::int64_t, Texture>
	>;


	//Connectionsの構造体
	using ObjectOrPropertyIndex = std::variant<std::int64_t, std::pair<std::int64_t, std::string>>;
	using Connections = std::vector<std::pair<ObjectOrPropertyIndex, ObjectOrPropertyIndex>>;


	//GeometryMeshにModelMeshのローカルの変換を適用した
	template<typename Vector2D, typename Vector3D>
	using Model3DParts = std::pair<GeometryMesh<Vector2D, Vector3D>, std::vector<std::int64_t>>;

	
	struct GlobalSettings
	{
		std::int32_t coordAxis;
		std::int32_t coordAxisSign;
		std::int32_t upAxis;
		std::int32_t upAxisSign;
		std::int32_t frontAxis;
		std::int32_t frontAxisSign;

	};
	
}