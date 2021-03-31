#pragma once
#include"../FBXComponents/Objects/Geometry/GeometryStruct.hpp"

namespace FBXL
{
	//GeometryMesh��ModelMesh�̃��[�J���̕ϊ���K�p�����\����
	template<typename Vector2D, typename Vector3D>
	using Model3DParts = std::pair<GeometryMesh<Vector2D, Vector3D>, std::vector<std::int64_t>>;
}