#pragma once
#include"../FBXComponents/Objects/Geometry/GeometryStruct.hpp"

namespace FBXL
{
	//GeometryMeshにModelMeshのローカルの変換を適用した構造体
	template<typename Vector2D, typename Vector3D>
	using Model3DParts = std::pair<GeometryMesh<Vector2D, Vector3D>, std::vector<std::int64_t>>;
}