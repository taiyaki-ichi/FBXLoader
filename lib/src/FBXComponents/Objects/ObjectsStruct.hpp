#pragma once
#include<unordered_map>
#include"Model/ModelStruct.hpp"
#include"Geometry/GeometryStruct.hpp"
#include"include/DataStruct.hpp"
#include"Texture/TextureStruct.hpp"

namespace FBXL
{
	template<typename Vector2D,typename Vector3D>
	struct Objects {
		std::unordered_map<std::int64_t, ModelMesh<Vector3D>> modelMeshes{};
		std::unordered_map<std::int64_t, GeometryMesh<Vector2D, Vector3D>> geometryMeshes{};
		std::unordered_map<std::int64_t, Material<Vector3D>> materials{};
		std::unordered_map<std::int64_t, Texture> textures{};
	};
}