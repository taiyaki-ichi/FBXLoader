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


	using TrianglePolygonIndex = std::tuple<std::size_t, std::size_t, std::size_t>;

	//とりあえずマテリアルのインデックスのみ
	//多分ボーンのインデックスの情報が追加されそう
	struct PolygonInformation {
		std::int32_t materialIndex{};
	};

	template<typename Vector2D,typename Vector3D>
	struct GeometryMesh2 {
		std::vector<TrianglePolygonIndex> trianglePolygonIndeces{};
		std::vector<Vertex<Vector2D, Vector3D>> verteces{};
		std::vector<PolygonInformation> materialInformations{};
	};

}