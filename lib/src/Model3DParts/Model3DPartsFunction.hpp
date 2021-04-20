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


	//GeometryMEshにModelMeshを適用
	template<typename Vector2D, typename Vector3D, typename TranslationVector3DPolicy, typename RotationVector3DPolicy, typename ScalinngVector3DPolicy>
	GeometryMesh<Vector2D, Vector3D> TransformGeometryMesh(GeometryMesh<Vector2D, Vector3D>&& geometryMesh, ModelMesh<Vector3D>&& modelMesh);

	template<typename Vector2D,typename Vector3D,typename Func>
	TrianglePolygon<Vector2D, Vector3D> TransformTrianglePolygon(Func&&, TrianglePolygon<Vector2D, Vector3D>&&, const Vector3D&);

	template<typename Vector2D, typename Vector3D>
	std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>> ToTrianglePolygonPairsFromModel3DParts(Model3DParts<Vector2D, Vector3D>&&);

	template<bool NormalFlag,typename Vector2D, typename Vector3D>
	Model3DParts<Vector2D, Vector3D> ToModel3DPartsFromTrianglePolygonPairs(std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>>&&);

	template<typename Vector2D, typename Vector3D>
	std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>> AppendTrianglePolygonPairs(
		std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>>&&, std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>>&&);


	template<typename Vector2D,typename Vector3D, typename TranslationVector3DPolicy, typename RotationVector3DPolicy, typename ScallingVector3DPolicy>
	Model3DParts2<Vector2D, Vector3D> GetModel3DParts2(
		std::unordered_map<std::int64_t, ModelMesh<Vector3D>>&& modelMeshes,
		std::unordered_map<std::int64_t, GeometryMesh2<Vector2D, Vector3D>>&& geometryMeshes,
		const std::unordered_map<std::int64_t, Material<Vector3D>>& materials,
		const Connections& connections);


	std::vector<std::pair<TrianglePolygonIndex, std::int64_t>> 
		AddMaterialIndexInfomation(std::vector<std::pair<TrianglePolygonIndex, std::int64_t>>&& geometryMeshMaterialIndeces, std::vector<std::int64_t>&& materialIndex);

	template<typename Vector2D,typename Vector3D, typename TranslationVector3DPolicy, typename RotationVector3DPolicy, typename ScalinngVector3DPolicy>
	std::vector<Vertex<Vector2D, Vector3D>> TransformVertices(std::vector<Vertex<Vector2D, Vector3D>>&&, ModelMesh<Vector3D>&& modelMesh);

	template<typename Vector2D,typename Vector3D>
	GeometryMesh2<Vector2D, Vector3D> AppendGeometryMesh2(GeometryMesh2<Vector2D, Vector3D>&&, GeometryMesh2<Vector2D, Vector3D>&&);

	template<typename Vector2D,typename Vector3D>
	Model3DParts2<Vector2D, Vector3D> GetModel3DPartsFromGeometryMesh2(GeometryMesh2<Vector2D, Vector3D>&&);



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
				assert(geometryMesh.value().materialRange.size() == materialIndex.size());

				auto g = TransformGeometryMesh<Vector2D, Vector3D, TranslationVector3DPolicy, RotationVector3DPolicy, ScallingVector3DPolicy>(
					std::move(geometryMesh.value()), std::move(modelMesh.second));
				auto tmp = ToTrianglePolygonPairsFromModel3DParts(std::make_pair(std::move(g), std::move(materialIndex)));
				trianglePolygonPairs = AppendTrianglePolygonPairs(std::move(trianglePolygonPairs), std::move(tmp));

				i++;
			}
		}

		return ToModel3DPartsFromTrianglePolygonPairs(std::move(trianglePolygonPairs));
	}


	template<typename Vector2D, typename Vector3D, typename TranslationVector3DPolicy, typename RotationVector3DPolicy, typename ScalinngVector3DPolicy>
	GeometryMesh<Vector2D, Vector3D> TransformGeometryMesh(GeometryMesh<Vector2D, Vector3D>&& geometryMesh, ModelMesh<Vector3D>&& modelMesh)
	{
		for (std::size_t i = 0; i < geometryMesh.trianglePolygons.size(); i++)
		{
			if (modelMesh.localScaling)
				geometryMesh.trianglePolygons[i] = TransformTrianglePolygon<false>(ScalinngVector3DPolicy::Scalling, std::move(geometryMesh.trianglePolygons[i]), modelMesh.localScaling.value());
			if (modelMesh.localRotation)
				geometryMesh.trianglePolygons[i] = TransformTrianglePolygon<true>(RotationVector3DPolicy::Rotation, std::move(geometryMesh.trianglePolygons[i]), modelMesh.localRotation.value());
			if (modelMesh.localTranslation)
				geometryMesh.trianglePolygons[i] = TransformTrianglePolygon<false>(TranslationVector3DPolicy::Translation, std::move(geometryMesh.trianglePolygons[i]), modelMesh.localTranslation.value());

		}

		return geometryMesh;
	}

	template<bool NormalFlag,typename Vector2D, typename Vector3D, typename Func>
	TrianglePolygon<Vector2D, Vector3D> TransformTrianglePolygon(Func&& func, TrianglePolygon<Vector2D, Vector3D>&& trianglePolygon, const Vector3D& vector3D)
	{
		TrianglePolygon<Vector2D, Vector3D> result{};
		for (std::size_t i = 0; i < 3; i++)
		{
			result[i].position = func(std::move(trianglePolygon[i].position), vector3D);

			if constexpr (NormalFlag)
				result[i].normal = func(std::move(trianglePolygon[i].normal), vector3D);
			else
				result[i].normal = std::move(trianglePolygon[i].normal);

			result[i].uv = std::move(trianglePolygon[i].uv);
		}

		return result;
	}

	template<typename Vector2D, typename Vector3D>
	std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>> ToTrianglePolygonPairsFromModel3DParts(Model3DParts<Vector2D, Vector3D>&& model3Dparts)
	{
		std::vector<std::pair<TrianglePolygon<Vector2D, Vector3D>, std::int64_t>> result{};
		result.reserve(model3Dparts.first.trianglePolygons.size());

		std::size_t vertexOffset = 0;
		for (std::size_t i = 0; i < model3Dparts.first.materialRange.size(); i++)
		{
			for (std::size_t j = 0; j < model3Dparts.first.materialRange[i]; j++)
			{
				result.emplace_back(std::make_pair(std::move(model3Dparts.first.trianglePolygons[vertexOffset + j]), model3Dparts.second[i]));
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
		result.first.trianglePolygons.reserve(trianglePolygonPairs.size());
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

			result.first.trianglePolygons.emplace_back(std::move(trianglePolygonPairs[i].first));

			result.first.materialRange[materialOffset]++;
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


	template<typename Vector2D, typename Vector3D, typename TranslationVector3DPolicy, typename RotationVector3DPolicy, typename ScallingVector3DPolicy>
	Model3DParts2<Vector2D, Vector3D> GetModel3DParts2(
		std::unordered_map<std::int64_t, ModelMesh<Vector3D>>&& modelMeshes,
		std::unordered_map<std::int64_t, GeometryMesh2<Vector2D, Vector3D>>&& geometryMeshes,
		const std::unordered_map<std::int64_t, Material<Vector3D>>& materials,
		const Connections& connections)
	{
		GeometryMesh2<Vector2D, Vector3D> geometryMesh{};

		std::size_t i = 0;
		for (auto&& modelMesh : modelMeshes)
		{
			auto cs = GetConnectionByDestination(connections, modelMesh.first);

			std::optional<GeometryMesh2<Vector2D, Vector3D>> geometryMeshOptional = std::nullopt;
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
							geometryMeshOptional = std::move(iter->second);
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

			if (geometryMeshOptional)
			{
				geometryMeshOptional.value().trianglePolygonIndeces =
					AddMaterialIndexInfomation(std::move(geometryMeshOptional.value().trianglePolygonIndeces), std::move(materialIndex));

				geometryMeshOptional.value().vertices =
					TransformVertices<Vector2D, Vector3D, TranslationVector3DPolicy, RotationVector3DPolicy, ScallingVector3DPolicy>(
						std::move(geometryMeshOptional.value().vertices), std::move(modelMesh.second));

				geometryMesh = AppendGeometryMesh2(std::move(geometryMesh), std::move(geometryMeshOptional.value()));

				i++;
			}
		}

		return GetModel3DPartsFromGeometryMesh2(std::move(geometryMesh));
	}


	std::vector<std::pair<TrianglePolygonIndex, std::int64_t>> AddMaterialIndexInfomation(std::vector<std::pair<TrianglePolygonIndex, std::int64_t>>&& geometryMeshMaterialIndeces, std::vector<std::int64_t>&& materialIndex)
	{
		for (std::size_t i = 0; i < geometryMeshMaterialIndeces.size(); i++)
			geometryMeshMaterialIndeces[i].second = materialIndex[geometryMeshMaterialIndeces[i].second];

		return geometryMeshMaterialIndeces;
	}

	template<typename Vector2D, typename Vector3D, typename TranslationVector3DPolicy, typename RotationVector3DPolicy, typename ScalinngVector3DPolicy>
	std::vector<Vertex<Vector2D, Vector3D>> TransformVertices(std::vector<Vertex<Vector2D, Vector3D>>&& vertices, ModelMesh<Vector3D>&& modelMesh)
	{
		for (std::size_t i = 0; i < vertices.size(); i++)
		{
			if (modelMesh.localScaling) 
				vertices[i].position = ScalinngVector3DPolicy::Scalling(std::move(vertices[i].position), modelMesh.localScaling.value());
			if (modelMesh.localRotation) {
				vertices[i].position = RotationVector3DPolicy::Rotation(std::move(vertices[i].position), modelMesh.localRotation.value());
				vertices[i].normal = RotationVector3DPolicy::Rotation(std::move(vertices[i].normal), modelMesh.localRotation.value());
			}
			if (modelMesh.localTranslation)
				vertices[i].position = TranslationVector3DPolicy::Translation(std::move(vertices[i].position), modelMesh.localTranslation.value());
		}

		return vertices;
	}

	template<typename Vector2D, typename Vector3D>
	GeometryMesh2<Vector2D, Vector3D> AppendGeometryMesh2(GeometryMesh2<Vector2D, Vector3D>&& a, GeometryMesh2<Vector2D, Vector3D>&& b)
	{
		a.trianglePolygonIndeces.reserve(a.trianglePolygonIndeces.size() + b.trianglePolygonIndeces.size());
		std::move(b.trianglePolygonIndeces.begin(), b.trianglePolygonIndeces.end(), std::back_inserter(a.trianglePolygonIndeces));

		a.vertices.reserve(a.vertices.size() + b.vertices.size());
		std::move(b.vertices.begin(), b.vertices.end(), std::back_inserter(a.vertices));

		return a;
	}

	template<typename Vector2D, typename Vector3D>
	Model3DParts2<Vector2D, Vector3D> GetModel3DPartsFromGeometryMesh2(GeometryMesh2<Vector2D, Vector3D>&& geometryMesh)
	{
		std::sort(geometryMesh.trianglePolygonIndeces.begin(), geometryMesh.trianglePolygonIndeces.end(), [](auto& a, auto& b) {
			return a.second < b.second;
			});

		Model3DParts2<Vector2D, Vector3D> result{};
		result.indeces.reserve(geometryMesh.trianglePolygonIndeces.size() * 3);
		result.vertices = std::move(geometryMesh.vertices);

		std::size_t materialOffset = 0;
		for (std::size_t i = 0; i < geometryMesh.trianglePolygonIndeces.size(); i++)
		{
			if (result.materialRanges.size() == 0 || result.materialIndeces[materialOffset] != geometryMesh.trianglePolygonIndeces[i].second)
			{
				result.materialIndeces.push_back(geometryMesh.trianglePolygonIndeces[i].second);

				result.materialRanges.push_back(0);
				materialOffset = result.materialRanges.size() - 1;
			}

			result.indeces.push_back(std::get<0>(geometryMesh.trianglePolygonIndeces[i].first));
			result.indeces.push_back(std::get<1>(geometryMesh.trianglePolygonIndeces[i].first));
			result.indeces.push_back(std::get<2>(geometryMesh.trianglePolygonIndeces[i].first));

			result.materialRanges[materialOffset] += 3;

		}

		return result;
	}
}