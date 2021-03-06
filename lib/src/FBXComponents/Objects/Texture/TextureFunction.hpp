#pragma once
#include"TextureStruct.hpp"
#include"../../../PrimitiveData/PrimitiveDataFunction.hpp"

namespace FBXL
{
	//Textureの取得
	inline std::pair<Texture, std::int64_t> GetTexture(Node&& textureNode);


	//
	//以下、実装
	//

	inline std::pair<Texture, std::int64_t> GetTexture(Node&& textureNode)
	{
		assert(textureNode.name == "Texture");

		Texture result{};

		auto index = GetProperty<std::int64_t>(&textureNode, 0).value();

		{
			auto node = GetSingleChildrenNode(&textureNode, "FileName");
			result.fileName = GetProperty<std::string>(node.value(), 0).value();
		}

		{
			auto node = GetSingleChildrenNode(&textureNode, "RelativeFilename");
			result.relativeFileName = GetProperty<std::string>(node.value(), 0).value();
		}

		return std::make_pair(std::move(result), index);
	}
}