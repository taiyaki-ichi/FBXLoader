#pragma once
#include"Model3DPartsStruct.hpp"
#include"../FBXComponents/Connections/ConnectionsFunction.hpp"
#include"../FBXComponents/Objects/ObjectsFunction.hpp"
#include<iterator>

namespace FBXL
{

	template<typename Vector2D,typename Vector3D, typename TranslationVector3DPolicy, typename RotationVector3DPolicy, typename ScallingVector3DPolicy>
	Model3DParts<Vector2D, Vector3D> GetModel3DParts(
		std::unordered_map<std::int64_t, ModelMesh<Vector3D>>&& modelMeshes,
		std::unordered_map<std::int64_t, GeometryMesh<Vector2D, Vector3D>>&& geometryMeshes,
		const std::unordered_map<std::int64_t, Material<Vector3D>>& materials,
		const Connections& connections);


	std::vector<std::pair<TrianglePolygonIndex, std::int64_t>> 
		AddMaterialIndexInfomation(std::vector<std::pair<TrianglePolygonIndex, std::int64_t>>&& geometryMeshMaterialIndeces, std::vector<std::int64_t>&& materialIndex);

	template<typename Vector2D, typename Vector3D, typename TranslationVector3DPolicy, typename RotationVector3DPolicy, typename ScalinngVector3DPolicy>
	std::vector<Vertex<Vector2D, Vector3D>> TransformVertices(std::vector<Vertex<Vector2D, Vector3D>>&&, ModelMesh<Vector3D>&& modelMesh);

	template<typename Vector2D,typename Vector3D>
	GeometryMesh<Vector2D, Vector3D> AppendGeometryMesh(GeometryMesh<Vector2D, Vector3D>&&, GeometryMesh<Vector2D, Vector3D>&&);

	template<typename Vector2D,typename Vector3D>
	Model3DParts<Vector2D, Vector3D> GetModel3DPartsFromGeometryMesh(GeometryMesh<Vector2D, Vector3D>&&);



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
		GeometryMesh<Vector2D, Vector3D> geometryMesh{};

		std::size_t i = 0;
		for (auto&& modelMesh : modelMeshes)
		{
			auto cs = GetConnectionByDestination(connections, modelMesh.first);

			std::optional<GeometryMesh<Vector2D, Vector3D>> geometryMeshOptional = std::nullopt;
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

				geometryMesh = AppendGeometryMesh(std::move(geometryMesh), std::move(geometryMeshOptional.value()));

				i++;
			}
		}

		return GetModel3DPartsFromGeometryMesh(std::move(geometryMesh));
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
	GeometryMesh<Vector2D, Vector3D> AppendGeometryMesh(GeometryMesh<Vector2D, Vector3D>&& a, GeometryMesh<Vector2D, Vector3D>&& b)
	{
		a.trianglePolygonIndeces.reserve(a.trianglePolygonIndeces.size() + b.trianglePolygonIndeces.size());
		std::move(b.trianglePolygonIndeces.begin(), b.trianglePolygonIndeces.end(), std::back_inserter(a.trianglePolygonIndeces));

		a.vertices.reserve(a.vertices.size() + b.vertices.size());
		std::move(b.vertices.begin(), b.vertices.end(), std::back_inserter(a.vertices));

		return a;
	}

	template<typename Vector2D, typename Vector3D>
	Model3DParts<Vector2D, Vector3D> GetModel3DPartsFromGeometryMesh(GeometryMesh<Vector2D, Vector3D>&& geometryMesh)
	{
		std::sort(geometryMesh.trianglePolygonIndeces.begin(), geometryMesh.trianglePolygonIndeces.end(), [](auto& a, auto& b) {
			return a.second < b.second;
			});

		Model3DParts<Vector2D, Vector3D> result{};
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