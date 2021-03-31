#pragma once
#include"GeometryStruct.hpp"
#include"../../GlobalSettings/GlobalSettingsStruct.hpp"
#include"../../PrimitiveData/PrimitiveDataFunction.hpp"

namespace FBXL
{
	//GeometryMeshのノードからGeometryMeshとそのインデックスの取得
	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::pair<GeometryMesh<Vector2D, Vector3D>, std::int64_t> GetGeometryMesh(Node&& geormetryMesh, const GlobalSettings& globalSettings);


	namespace {

		//geometryMeshのノードからシンプルな頂点データなどを取得
		template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
		std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>> GetVertexAndMaterialNumberPairs(const Node* geometryMesh, const GlobalSettings& globalSettings);

		//geometryMeshから取得したシンプルなデータを加工
		template<typename Vector2D, typename Vector3D>
		std::pair<std::vector<Vertex<Vector2D, Vector3D>>, std::vector<std::int32_t>>
			GetVerticesAndMaterialRange(std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>>&& pair);


		//GeometyMeshNodeからマテリアルのインデックス配列を取得
		inline std::optional<std::vector<std::int32_t>> GetMaterialIndeces(const Node* geometyMesh);

		//GeometyMeshNodeからUVの取得
		template<typename Vector2D, typename CreateVector2DPolicy>
		std::vector<Vector2D> GetUVs(const Node* geometryMesh);

		//GeometyMeshNodeから法線のデータとポリゴンに対応じているのか頂点に対応しているのか
		inline std::pair<std::vector<double>, bool> GetNormalData(const Node* geometryMesh);

		//GlobalSettingを適用するためのインターフェース
		template<typename CreateVector3DPolicy>
		struct CreateVector3DInterfacePolicy
		{
			static auto Invoke(double a, double b, double c, const GlobalSettings& globalSettings)
			{
				double nums[] = { a,b,c };
				return CreateVector3DPolicy::Create(
					nums[globalSettings.coordAxis] * globalSettings.coordAxisSign,
					nums[globalSettings.upAxis] * globalSettings.upAxisSign,
					nums[globalSettings.frontAxis] * globalSettings.coordAxisSign);
			}
		};

	}


	//
	//以下、実装
	//

	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	std::pair<GeometryMesh<Vector2D, Vector3D>, std::int64_t> GetGeometryMesh(Node&& geormetryMesh, const GlobalSettings& globalSettings)
	{
		static_assert(std::is_invocable_r_v<Vector2D, decltype(CreateVector2DPolicy::Create), double, double>,
			"Vecto2D CreateVector2DPolicy::Create(double,double) is not declared");

		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		assert(geormetryMesh.name == "Geometry" && GetProperty<std::string>(&geormetryMesh, 2) == "Mesh");

		GeometryMesh<Vector2D, Vector3D> result{};

		auto index = GetProperty<std::int64_t>(&geormetryMesh, 0).value();

		auto vertexAndMaterialNumPairs = GetVertexAndMaterialNumberPairs<Vector2D, Vector3D, CreateVector2DPolicy, CreateVector3DPolicy>(&geormetryMesh, globalSettings);
		auto [vertices, materialRange] = GetVerticesAndMaterialRange(std::move(vertexAndMaterialNumPairs));
		result.vertices = std::move(vertices);
		result.materialRange = std::move(materialRange);

		return std::make_pair(std::move(result), index);
	}


	namespace {

		template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
		std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>> GetVertexAndMaterialNumberPairs(const Node* geometryMesh, const GlobalSettings& globalSettings)
		{
			std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>> result{};

			auto verticesNode = GetSingleChildrenNode(geometryMesh, "Vertices");
			auto vertices = GetProperty<std::vector<double>>(verticesNode.value(), 0).value();

			auto indexNode = GetSingleChildrenNode(geometryMesh, "PolygonVertexIndex");
			auto indeces = GetProperty<std::vector<std::int32_t>>(indexNode.value(), 0).value();

			auto [normals, normalIsByPolygon] = GetNormalData(geometryMesh);
			auto uvs = GetUVs<Vector2D, CreateVector2DPolicy>(geometryMesh);

			auto materialIndeces = GetMaterialIndeces(geometryMesh);


			//assert((normalIsByPolygon && normals.size() == indeces.size()) ||
				//(!normalIsByPolygon && normals.size() == vertices.size()));

			//uvはByPolygonVertex一択のはず
			assert(uvs.size() == indeces.size());


			auto pushBack = [&vertices, &normals, normalIsByPolygon, &uvs, &result, &materialIndeces, &globalSettings](std::size_t index1, std::size_t index2, std::size_t index3, std::size_t offset) {

				Vertex<Vector2D, Vector3D> tmpVec;
				tmpVec.position = CreateVector3DInterfacePolicy<CreateVector3DPolicy>::Invoke(vertices[index1], vertices[index2], vertices[index3], globalSettings);

				if (normalIsByPolygon)
					tmpVec.normal = CreateVector3DInterfacePolicy<CreateVector3DPolicy>::Invoke(normals[offset * 3], normals[offset * 3 + 1], normals[offset * 3 + 2], globalSettings);
				else
					tmpVec.normal = CreateVector3DInterfacePolicy<CreateVector3DPolicy>::Invoke(normals[index1], normals[index2], normals[index3], globalSettings);

				tmpVec.uv = uvs[offset];


				if (materialIndeces && materialIndeces.value().size() > 1)
					result.emplace_back(std::make_pair(std::move(tmpVec), materialIndeces.value()[offset]));
				else
					result.emplace_back(std::make_pair(std::move(tmpVec), 0));
			};

			std::size_t offset = 0;
			std::size_t i = 0;
			std::size_t j = 0;
			Vertex<Vector2D, Vector3D> tmpVec;
			while (i < indeces.size())
			{
				j = i + 1;

				while (indeces[j + 1] >= 0)
				{
					pushBack(indeces[i] * 3, indeces[i] * 3 + 1, indeces[i] * 3 + 2, offset);
					pushBack(indeces[j] * 3, indeces[j] * 3 + 1, indeces[j] * 3 + 2, offset);
					pushBack(indeces[j + 1] * 3, indeces[j + 1] * 3 + 1, indeces[j + 1] * 3 + 2, offset);

					j++;
				}

				pushBack(indeces[i] * 3, indeces[i] * 3 + 1, indeces[i] * 3 + 2, offset);
				pushBack(indeces[j] * 3, indeces[j] * 3 + 1, indeces[j] * 3 + 2, offset);
				pushBack((-indeces[j + 1] - 1) * 3, (-indeces[j + 1] - 1) * 3 + 1, (-indeces[j + 1] - 1) * 3 + 2, offset);

				i = j + 2;

				offset++;
			}

			return result;
		}

		template<typename Vector2D, typename Vector3D>
		std::pair<std::vector<Vertex<Vector2D, Vector3D>>, std::vector<std::int32_t>> GetVerticesAndMaterialRange(std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>>&& pair)
		{
			std::pair<std::vector<Vertex<Vector2D, Vector3D>>, std::vector<std::int32_t>> result{};

			if (pair.size() == 0)
				return result;

			std::sort(pair.begin(), pair.end(), [](auto& a, auto& b) {
				return a.second < b.second;
				});

			result.first.resize(pair.size());
			result.second.resize(pair[pair.size() - 1].second + 1);

			std::fill(result.second.begin(), result.second.end(), 0);

			for (std::size_t i = 0; i < pair.size(); i++) {
				result.first[i] = std::move(pair[i].first);
				result.second[pair[i].second]++;
			}

			return result;
		}

		inline std::optional<std::vector<std::int32_t>> GetMaterialIndeces(const Node* geometyMesh)
		{
			auto layerElementMaterialNode = GetSingleChildrenNode(geometyMesh, "LayerElementMaterial");

			if (layerElementMaterialNode)
			{
				auto materialNode = GetSingleChildrenNode(layerElementMaterialNode.value(), "Materials").value();

				return GetProperty<std::vector<std::int32_t>>(materialNode, 0).value();
			}
			else
				return std::nullopt;
		}



		template<typename Vector2D, typename CreateVector2DPolicy>
		std::vector<Vector2D> GetUVs(const Node* geometryMesh)
		{

			auto layerElementUVNode = GetSingleChildrenNode(geometryMesh, "LayerElementUV");

			auto uvNode = GetSingleChildrenNode(layerElementUVNode.value(), "UV");
			auto uvIndexNode = GetSingleChildrenNode(layerElementUVNode.value(), "UVIndex");

			auto uv = GetProperty<std::vector<double>>(uvNode.value(), 0).value();
			auto uvIndex = GetProperty<std::vector<std::int32_t>>(uvIndexNode.value(), 0).value();

			auto isVailedIndex = [&uv](std::int32_t index) {
				return 0 <= index && index < uv.size();
			};


			std::vector<Vector2D> result{};
			result.reserve(uvIndex.size());
			for (std::size_t i = 0; i < uvIndex.size(); i++) {
				if (isVailedIndex(uvIndex[i] * 2) && isVailedIndex(uvIndex[i] * 2 + 1))
					result.push_back(CreateVector2DPolicy::Create(uv[uvIndex[i] * 2], uv[uvIndex[i] * 2 + 1]));
				else
					result.push_back(CreateVector2DPolicy::Create(0.0, 0.0));
			}

			return result;
		}

		inline std::pair<std::vector<double>, bool> GetNormalData(const Node* geometryMesh)
		{
			auto layerElementNormalNode = GetSingleChildrenNode(geometryMesh, "LayerElementNormal");

			auto modelMesh = GetSingleChildrenNode(layerElementNormalNode.value(), "Normals");

			auto mappingInformationTypeNode = GetSingleChildrenNode(layerElementNormalNode.value(), "MappingInformationType");
			bool isByPolygon{};
			if (mappingInformationTypeNode)
				isByPolygon = (GetProperty<std::string>(mappingInformationTypeNode.value(), 0) == "ByPolygonVertex");

			auto result = GetProperty<std::vector<double>>(modelMesh.value(), 0).value();

			return std::make_pair(std::move(result), isByPolygon);
		}
	}
}