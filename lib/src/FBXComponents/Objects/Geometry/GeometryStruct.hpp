#pragma once
#include<vector>

namespace FBXL
{
	template<typename Vector2D, typename Vector3D>
	struct GeometryMesh
	{
		std::vector<Vertex<Vector2D, Vector3D>> vertices;
		std::vector<std::int32_t> materialRange{};
	};
}