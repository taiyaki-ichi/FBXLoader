#pragma once
#include<unordered_map>
#include"Model/ModelStruct.hpp"
#include"Geometry/GeometryStruct.hpp"
#include"include/DataStruct.hpp"
#include"Texture/TextureStruct.hpp"

namespace FBXL
{
	template<typename Vector2D, typename Vector3D>
	using Objects = std::tuple<
		std::unordered_map<std::int64_t, ModelMesh<Vector3D>>,
		std::unordered_map<std::int64_t, GeometryMesh<Vector2D, Vector3D>>,
		std::unordered_map<std::int64_t, Material<Vector3D>>,
		std::unordered_map<std::int64_t, Texture>
	>;

	template<typename Vector2D,typename Vector3D>
	using Objects2 = std::tuple<
		std::unordered_map<std::int64_t, ModelMesh<Vector3D>>,
		std::unordered_map<std::int64_t, GeometryMesh2<Vector2D, Vector3D>>,
		std::unordered_map<std::int64_t, Material<Vector3D>>,
		std::unordered_map<std::int64_t, Texture>
	>;
}