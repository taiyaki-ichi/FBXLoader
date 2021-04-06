#pragma once
#include"Model3DPartsStruct.hpp"
#include"../FBXComponents/Connections/ConnectionsFunction.hpp"
#include"../FBXComponents/Objects/ObjectsFunction.hpp"
#include<iterator>

namespace FBXL
{
	//Model3DPartsの取得
	template<typename Vector2D, typename Vector3D, typename TranslationVector3DPolicy, typename RotationVector3DPolicy, typename ScallingVector3DPolicy>
	Model3DParts<Vector2D, Vector3D> GetModel3DParts(
		std::unordered_map<std::int64_t, ModelMesh<Vector3D>>&& modelMeshes,
		std::unordered_map<std::int64_t, GeometryMesh<Vector2D, Vector3D>>&& geometryMeshes,
		const std::unordered_map<std::int64_t, Material<Vector3D>>& materials,
		const Connections& connections);




	//Model3DPartsの合体
	template<typename Vector2D, typename Vector3D>
	Model3DParts<Vector2D, Vector3D> AppendModel3DParts(Model3DParts<Vector2D, Vector3D>&& a, Model3DParts<Vector2D, Vector3D>&& b);

	//GeometryMEshにModelMeshを適用
	template<typename Vector2D, typename Vector3D, typename TranslationVector3DPolicy, typename RotationVector3DPolicy, typename ScalinngVector3DPolicy>
	GeometryMesh<Vector2D, Vector3D> TransformGeometryMesh(GeometryMesh<Vector2D, Vector3D>&& geometryMesh, ModelMesh<Vector3D>&& modelMesh);

	template<typename Vector2D, typename Vector3D>
	using TrianglePolygon = std::array<Vertex<Vector2D, Vector3D>, 3>;

	template<typename Vector2D, typename Vector3D>
	std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>> ToTrianglePolygonPairsFromModel3DParts(Model3DParts<Vector2D, Vector3D>&&);

	template<typename Vector2D, typename Vector3D>
	Model3DParts<Vector2D, Vector3D> ToModel3DPartsFromTrianglePolygonPairs(std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>>&&);

	template<typename Vector2D, typename Vector3D>
	std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>> AppendTrianglePolygonPairs(
		std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>>&&, std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>>&&);

	//Model3DPartsを分解
	template<typename Vector2D, typename Vector3D>
	std::vector<std::pair<std::vector<Vertex<Vector2D, Vector3D>>, std::int64_t>> GetVertecesAndMaterialIndecsArray(Model3DParts<Vector2D, Vector3D>&& v);





	//
	//以下、実装
	//


	template<typename Vector2D, typename Vector3D, typename TranslationVector3DPolicy, typename RotationVector3DPolicy, typename ScallingVector3DPolicy>
	Model3DParts<Vector2D, Vector3D> GetModel3DParts(
		std::unordered_map<std::int64_t, ModelMesh<Vector3D>>&& modelMeshes,
		std::unordered_map<std::int64_t, GeometryMesh<Vector2D, Vector3D>>&& geometryMeshes,
		const std::unordered_map<std::int64_t, Material<Vector3D>>& materials,
		const Connections& connections)
	{
		//Model3DParts<Vector2D, Vector3D> result{};
		std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>> trianglePolygonPairs{};

		std::size_t i = 0;
		for (auto&& modelMesh : modelMeshes)
		{
			auto cs = GetConnectionByDestination(connections, modelMesh.first);

			std::optional<GeometryMesh<Vector2D, Vector3D>> geometryMesh = std::nullopt;
			std::vector<std::int64_t> materialIndex{};

			for (auto&& c : cs)
			{
				//オブジェクトとのつながりのみ
				if (c.index() == 0)
				{
					//geometryMeshを探す
					//moveしてしまう
					{
						auto iter = geometryMeshes.find(std::get<0>(c));
						if (iter != geometryMeshes.end())
						{
							geometryMesh = std::move(iter->second);
							geometryMeshes.erase(iter);
						}
					}

					//Materialを探す
					//そのインデックスだけメモ
					//順番あっているか分からん
					{
						auto iter = materials.find(std::get<0>(c));
						if (iter != materials.end())
							materialIndex.push_back(std::get<0>(c));
					}
				}
			}

			if (geometryMesh)
			{
				//assert(geometryMesh.value().materialRange.size() == materialIndex.size());

				auto g = TransformGeometryMesh<Vector2D, Vector3D, TranslationVector3DPolicy, RotationVector3DPolicy, ScallingVector3DPolicy>(
					std::move(geometryMesh.value()), std::move(modelMesh.second));
				//result = AppendModel3DParts<Vector2D, Vector3D>(std::move(result), std::make_pair(std::move(g), std::move(materialIndex)));
				auto tmp = ToTrianglePolygonPairsFromModel3DParts(std::make_pair(std::move(g), std::move(materialIndex)));
				trianglePolygonPairs = AppendTrianglePolygonPairs(std::move(trianglePolygonPairs), std::move(tmp));

				i++;
			}
		}

		i;

		//return result;
		return ToModel3DPartsFromTrianglePolygonPairs(std::move(trianglePolygonPairs));
	}


	template<typename Vector2D, typename Vector3D>
	Model3DParts<Vector2D, Vector3D> AppendModel3DParts(Model3DParts<Vector2D, Vector3D>&& a, Model3DParts<Vector2D, Vector3D>&& b)
	{
		Model3DParts<Vector2D, Vector3D> result{};

		auto vertex = GetVertecesAndMaterialIndecsArray<Vector2D, Vector3D>(std::move(a));

		{
			auto tmpV = GetVertecesAndMaterialIndecsArray<Vector2D, Vector3D>(std::move(b));
			std::move(tmpV.begin(), tmpV.end(), std::back_inserter(vertex));
		}

		std::sort(vertex.begin(), vertex.end(), [](auto& a, auto& b) {
			return a.second < b.second;
			});


		for (auto&& v : vertex)
		{
			if (result.second.size() == 0 || result.second.back() != v.second)
			{
				result.first.materialRange.push_back(v.first.size());
				std::move(v.first.begin(), v.first.end(), std::back_inserter(result.first.vertices));
				result.second.push_back(v.second);
			}
			else //if (result.second.back() == v.second)
			{
				result.first.materialRange.back() += v.first.size();
				std::move(v.first.begin(), v.first.end(), std::back_inserter(result.first.vertices));
			}
		}

		return result;
	}

	template<typename Vector2D, typename Vector3D, typename TranslationVector3DPolicy, typename RotationVector3DPolicy, typename ScalinngVector3DPolicy>
	GeometryMesh<Vector2D, Vector3D> TransformGeometryMesh(GeometryMesh<Vector2D, Vector3D>&& geometryMesh, ModelMesh<Vector3D>&& modelMesh)
	{
		for (std::size_t i = 0; i < geometryMesh.vertices.size(); i++)
		{
			if (modelMesh.localScaling)
				geometryMesh.vertices[i].position = ScalinngVector3DPolicy::Scalling(std::move(geometryMesh.vertices[i].position), modelMesh.localScaling.value());
			if (modelMesh.localRotation) {
				geometryMesh.vertices[i].position = RotationVector3DPolicy::Rotation(std::move(geometryMesh.vertices[i].position), modelMesh.localRotation.value());
				geometryMesh.vertices[i].normal = RotationVector3DPolicy::Rotation(std::move(geometryMesh.vertices[i].normal), modelMesh.localRotation.value());
			}
			if (modelMesh.localTranslation)
				geometryMesh.vertices[i].position = TranslationVector3DPolicy::Translation(std::move(geometryMesh.vertices[i].position), modelMesh.localTranslation.value());

		}

		return geometryMesh;
	}

	template<typename Vector2D, typename Vector3D>
	std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>> ToTrianglePolygonPairsFromModel3DParts(Model3DParts<Vector2D, Vector3D>&& model3Dparts)
	{
		std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>> result{};
		result.reserve(model3Dparts.first.vertices.size() / 3);
		TrianglePolygon<Vector2D, Vector3D> tmpTrianglePolygon{};

		std::size_t vertexOffset = 0;
		for (std::size_t i = 0; i < model3Dparts.first.materialRange.size(); i++)
		{
			for (std::size_t j = 0; j < model3Dparts.first.materialRange[i] / 3; j++)
			{
				tmpTrianglePolygon[0] = std::move(model3Dparts.first.vertices[vertexOffset + j * 3]);
				tmpTrianglePolygon[1] = std::move(model3Dparts.first.vertices[vertexOffset + j * 3 + 1]);
				tmpTrianglePolygon[2] = std::move(model3Dparts.first.vertices[vertexOffset + j * 3 + 2]);

				result.emplace_back(std::make_pair(std::move(tmpTrianglePolygon), model3Dparts.second[i]));
			}

			vertexOffset += model3Dparts.first.materialRange[i];
		}

		return result;
	}

	template<typename Vector2D, typename Vector3D>
	Model3DParts<Vector2D, Vector3D> ToModel3DPartsFromTrianglePolygonPairs(std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>>&& trianglePolygonPairs)
	{
		std::sort(trianglePolygonPairs.begin(), trianglePolygonPairs.end(), [](auto& a, auto& b) {
			return a.second < b.second;
			});

		Model3DParts<Vector2D, Vector3D> result{};
		result.first.vertices.reserve(trianglePolygonPairs.size() * 3);
		std::fill(result.second.begin(), result.second.end(), 0);

		std::size_t materialOffset{};
		for (std::size_t i = 0; i < trianglePolygonPairs.size(); i++)
		{
			if (result.second.size() == 0 || result.second[materialOffset] != trianglePolygonPairs[i].second)
			{
				result.second.push_back(trianglePolygonPairs[i].second);
				result.first.materialRange.push_back(0);
				materialOffset = result.second.size() - 1;
			}

			result.first.vertices.emplace_back(std::move(trianglePolygonPairs[i].first[0]));
			result.first.vertices.emplace_back(std::move(trianglePolygonPairs[i].first[1]));
			result.first.vertices.emplace_back(std::move(trianglePolygonPairs[i].first[2]));

			result.first.materialRange[materialOffset] += 3;
		}

		return result;
	}

	template<typename Vector2D, typename Vector3D>
	std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>> 
		AppendTrianglePolygonPairs(std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>>&& a, std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>>&& b)
	{
		std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>> result{};
		result.reserve(a.size() + b.size());

		std::move(a.begin(), a.end(), std::back_inserter(result));
		std::move(b.begin(), b.end(), std::back_inserter(result));

		return result;
	}

	template<typename Vector2D, typename Vector3D>
	std::vector<std::pair<std::vector<Vertex<Vector2D, Vector3D>>, std::int64_t>> GetVertecesAndMaterialIndecsArray(Model3DParts<Vector2D, Vector3D>&& v)
	{
		std::vector<std::pair<std::vector<Vertex<Vector2D, Vector3D>>, std::int64_t>> result{};

		std::vector<Vertex<Vector2D, Vector3D>> tmpVertex{};
		std::size_t vertexOffset{};

		for (std::size_t i = 0; i < v.second.size(); i++)
		{
			std::move(v.first.vertices.begin() + vertexOffset, v.first.vertices.begin() + vertexOffset + v.first.materialRange[i], std::back_inserter(tmpVertex));
			result.push_back(std::make_pair(std::move(tmpVertex), v.second[i]));
			vertexOffset += v.first.materialRange[i];
		}

		return result;
	}



}