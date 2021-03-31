#pragma once
#include"Model3DParts/Model3DPartsFunction.hpp"

namespace FBXL
{

	//マテリアエルにテクスチャの情報を加える
	template<typename Vector3D>
	std::unordered_map<std::int64_t, Material<Vector3D>> AddTextureInfomarion(
		std::unordered_map<std::int64_t, Material<Vector3D>>&& materials,
		std::unordered_map<std::int64_t, Texture>&& textures,
		const Connections& connectios);

	//Model3Dの取得
	template<typename Vector2D, typename Vector3D>
	Model3D<Vector2D, Vector3D> GetModel3D(Model3DParts<Vector2D, Vector3D>&& model3DPair, std::unordered_map<std::int64_t, Material<Vector3D>>&& materials);

	//
	//以下、実装
	//




	template<typename Vector3D>
	std::unordered_map<std::int64_t, Material<Vector3D>> AddTextureInfomarion(
		std::unordered_map<std::int64_t, Material<Vector3D>>&& materials,
		std::unordered_map<std::int64_t, Texture>&& textures,
		const Connections& connectios)
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
						material.diffuseColorTexturePath = textureIter->second.relativeFileName;
				}
			}
		}

		return materials;
	}



	template<typename Vector2D, typename Vector3D>
	Model3D<Vector2D, Vector3D> GetModel3D(Model3DParts<Vector2D, Vector3D>&& model3DPair, std::unordered_map<std::int64_t, Material<Vector3D>>&& materials)
	{
		Model3D<Vector2D, Vector3D> result{};
		result.vertices = std::move(model3DPair.first.vertices);
		result.materialRange = std::move(model3DPair.first.materialRange);

		result.material.resize(model3DPair.second.size());
		for (std::size_t i = 0; i < model3DPair.second.size(); i++)
		{
			auto iter = materials.find(model3DPair.second[i]);
			if (iter != materials.end())
				result.material[i] = std::move(iter->second);
		}

		return result;
	}


}