#pragma once
#include"Model3DParts/Model3DPartsFunction.hpp"

namespace FBXL
{

	//マテリアエルにテクスチャの情報を加える
	template<typename Vector3D>
	std::unordered_map<std::int64_t, Material<Vector3D>> AddTextureInfomation(
		std::unordered_map<std::int64_t, Material<Vector3D>>&& materials,
		std::unordered_map<std::int64_t, Texture>&& textures,
		const Connections& connectios,
		std::string&& folderPath);

	//Model3Dの取得
	template<typename Vector2D, typename Vector3D>
	Model3D<Vector2D, Vector3D> GetModel3D(Model3DParts<Vector2D, Vector3D>&& model3DPair, std::unordered_map<std::int64_t, Material<Vector3D>>&& materials);

	template<typename Vector2D, typename Vector3D>
	Model3D2<Vector2D, Vector3D> GetModel3D2(Model3DParts2<Vector2D, Vector3D>&& model3DParts, std::unordered_map<std::int64_t, Material<Vector3D>>&& materials);

	//
	//以下、実装
	//




	template<typename Vector3D>
	std::unordered_map<std::int64_t, Material<Vector3D>> AddTextureInfomation(
		std::unordered_map<std::int64_t, Material<Vector3D>>&& materials,
		std::unordered_map<std::int64_t, Texture>&& textures,
		const Connections& connectios,
		std::string&& folderPath)
	{
		for (auto& [index, material] : materials)
		{
			auto cs = GetConnectionByDestination(connectios, std::make_pair(index, "DiffuseColor"));

			for (auto& c : cs)
			{
				//とりあえずプロパティとしてマテリアルとつながっているテクスチャのみ
				//Diffuseのテクスチャのみ
				if (c.index() == 0)
				{
					auto& index = std::get<0>(c);

					auto textureIter = textures.find(index);
					if (textureIter != textures.end())
						material.diffuseColorTexturePath = folderPath + textureIter->second.relativeFileName;
				}
			}
		}

		return materials;
	}



	template<typename Vector2D, typename Vector3D>
	Model3D<Vector2D, Vector3D> GetModel3D(Model3DParts<Vector2D, Vector3D>&& model3DPair, std::unordered_map<std::int64_t, Material<Vector3D>>&& materials)
	{
		Model3D<Vector2D, Vector3D> result{};
		result.vertices.reserve(model3DPair.first.trianglePolygons.size() * 3);
		for (std::size_t i = 0; i < model3DPair.first.trianglePolygons.size(); i++)
		{
			result.vertices.emplace_back(std::move(model3DPair.first.trianglePolygons[i][0]));
			result.vertices.emplace_back(std::move(model3DPair.first.trianglePolygons[i][1]));
			result.vertices.emplace_back(std::move(model3DPair.first.trianglePolygons[i][2]));
		}
		result.materialRange = std::move(model3DPair.first.materialRange);
		for (std::size_t i = 0; i < result.materialRange.size(); i++)
			result.materialRange[i] *= 3;

		result.material.resize(model3DPair.second.size());
		for (std::size_t i = 0; i < model3DPair.second.size(); i++)
		{
			auto iter = materials.find(model3DPair.second[i]);
			if (iter != materials.end())
				result.material[i] = std::move(iter->second);
		}

		return result;
	}

	template<typename Vector2D, typename Vector3D>
	Model3D2<Vector2D, Vector3D> GetModel3D2(Model3DParts2<Vector2D, Vector3D>&& model3DParts, std::unordered_map<std::int64_t, Material<Vector3D>>&& materials)
	{
		Model3D2<Vector2D, Vector3D> result{};

		result.indeces = std::move(model3DParts.indeces);
		result.vertices = std::move(model3DParts.vertices);
		result.materialRange = std::move(model3DParts.materialRanges);

		result.material.resize(model3DParts.materialIndeces.size());
		for (std::size_t i = 0; i < model3DParts.materialIndeces.size(); i++)
			result.material[i] = std::move(materials[model3DParts.materialIndeces[i]]);
		
		return result;
	}
}