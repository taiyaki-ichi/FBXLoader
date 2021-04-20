#pragma once
#include<vector>
#include<array>

namespace FBXL
{
	using TrianglePolygonIndex = std::tuple<std::size_t, std::size_t, std::size_t>;

	template<typename Vector2D,typename Vector3D>
	struct GeometryMesh {
		std::vector<std::pair<TrianglePolygonIndex, std::int64_t>> trianglePolygonIndeces{};
		std::vector<Vertex<Vector2D, Vector3D>> vertices{};
	};

}