#pragma once
#include"../FBXComponents/Objects/Geometry/GeometryStruct.hpp"

namespace FBXL
{
	//GeometryMesh��ModelMesh�̃��[�J���̕ϊ���K�p�����\����
	template<typename Vector2D, typename Vector3D>
	using Model3DParts = std::pair<GeometryMesh<Vector2D, Vector3D>, std::vector<std::int64_t>>;

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