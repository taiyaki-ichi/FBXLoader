#pragma once
#include<vector>
#include<array>

namespace FBXL
{
	template<typename Vector2D,typename Vector3D>
	using TrianglePolygon = std::array<Vertex<Vector2D, Vector3D>, 3>;


	template<typename Vector2D, typename Vector3D>
	struct GeometryMesh
	{
		std::vector<TrianglePolygon<Vector2D,Vector3D>> trianglePolygons;
		std::vector<std::int32_t> materialRange{};
	};
}