#pragma once
#include"TextureStruct.hpp"
#include"../../../PrimitiveData/PrimitiveDataFunction.hpp"

namespace FBXL
{
	//Texture�̎擾
	inline std::pair<Texture, std::int64_t> GetTexture(Node&& textureNode);


	//
	//�ȉ��A����
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