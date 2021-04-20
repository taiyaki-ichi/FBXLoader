#pragma once
#include"../FBXComponents/Objects/Geometry/GeometryStruct.hpp"

namespace FBXL
{
	template<typename Vector2D,typename Vector3D>
	struct Model3DParts2
	{
		std::vector<std::size_t> indeces{};
		std::vector<Vertex<Vector2D, Vector3D>> vertices{};
		std::vector<std::size_t> materialRanges{};

		//�Ή�����}�e���A���̃C���f�b�N�X
		std::vector<std::int64_t> materialIndeces{};
	};

}