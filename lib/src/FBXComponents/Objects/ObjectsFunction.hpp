#pragma once
#include"ObjectsStruct.hpp"
#include"../GlobalSettings/GlobalSettingsStruct.hpp"
#include"Geometry/GeometryFunction.hpp"
#include"Material/MaterialFunction.hpp"
#include"Model/ModelFunction.hpp"
#include"Texture/TextureFunction.hpp"

namespace FBXL
{
	//NodeÇ©ÇÁObjectsÇÃéÊìæ
	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	Objects<Vector2D, Vector3D> GetObjects(Node&& objects, const GlobalSettings& globalSettings);

	template<typename Vector2D,typename Vector3D,typename CreateVector2DPolicy,typename CreateVector3DPolicy>
	Objects2<Vector2D, Vector3D> GetObjects2(Node&& objects, const GlobalSettings& globalSettings);


	//
	//à»â∫ÅAé¿ëï
	//


	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	Objects<Vector2D, Vector3D> GetObjects(Node&& objects, const GlobalSettings& globalSettings)
	{
		static_assert(std::is_invocable_r_v<Vector2D, decltype(CreateVector2DPolicy::Create), double, double>,
			"Vecto2D CreateVector2DPolicy::Create(double,double) is not declared");

		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		assert(objects.name == "Objects");

		std::unordered_map<std::int64_t, ModelMesh<Vector3D>> modelMeshes{};
		std::unordered_map<std::int64_t, GeometryMesh<Vector2D, Vector3D>> geometryMeshes{};
		std::unordered_map<std::int64_t, Material<Vector3D>> materials{};
		std::unordered_map<std::int64_t, Texture> textures{};

		for (auto& node : objects.children)
		{
			if (node.name == "Model" && GetProperty<std::string>(&node, 2) == "Mesh")
			{
				auto [modelMesh, index] = GetModelMesh<Vector3D, CreateVector3DPolicy>(std::move(node));
				modelMeshes.emplace(index, std::move(modelMesh));
			}
			else if (node.name == "Geometry" && GetProperty<std::string>(&node, 2) == "Mesh")
			{

				auto [geometryMesh, index] = GetGeometryMesh<Vector2D, Vector3D, CreateVector2DPolicy, CreateVector3DPolicy>(std::move(node), globalSettings);
				geometryMeshes.emplace(index, std::move(geometryMesh));
			}
			else if (node.name == "Material")
			{
				auto [material, index] = GetMaterial<Vector3D, CreateVector3DPolicy>(std::move(node));
				materials.emplace(index, std::move(material));
			}
			else if (node.name == "Texture")
			{
				auto [texture, index] = GetTexture(std::move(node));
				textures.emplace(index, std::move(texture));
			}
		}

		return std::make_tuple(std::move(modelMeshes), std::move(geometryMeshes), std::move(materials), std::move(textures));
	}

	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy>
	Objects2<Vector2D, Vector3D> GetObjects2(Node&& objects, const GlobalSettings& globalSettings)
	{
		static_assert(std::is_invocable_r_v<Vector2D, decltype(CreateVector2DPolicy::Create), double, double>,
			"Vecto2D CreateVector2DPolicy::Create(double,double) is not declared");

		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		assert(objects.name == "Objects");

		std::unordered_map<std::int64_t, ModelMesh<Vector3D>> modelMeshes{};
		std::unordered_map<std::int64_t, GeometryMesh2<Vector2D, Vector3D>> geometryMeshes{};
		std::unordered_map<std::int64_t, Material<Vector3D>> materials{};
		std::unordered_map<std::int64_t, Texture> textures{};

		for (auto& node : objects.children)
		{
			if (node.name == "Model" && GetProperty<std::string>(&node, 2) == "Mesh")
			{
				auto [modelMesh, index] = GetModelMesh<Vector3D, CreateVector3DPolicy>(std::move(node));
				modelMeshes.emplace(index, std::move(modelMesh));
			}
			else if (node.name == "Geometry" && GetProperty<std::string>(&node, 2) == "Mesh")
			{
				auto [geometryMesh, index] = GetGeometryMesh2<Vector2D, Vector3D, CreateVector2DPolicy, CreateVector3DPolicy>(std::move(node), globalSettings);
				geometryMeshes.emplace(index, std::move(geometryMesh));
			}
			else if (node.name == "Material")
			{
				auto [material, index] = GetMaterial<Vector3D, CreateVector3DPolicy>(std::move(node));
				materials.emplace(index, std::move(material));
			}
			else if (node.name == "Texture")
			{
				auto [texture, index] = GetTexture(std::move(node));
				textures.emplace(index, std::move(texture));
			}
		}

		return std::make_tuple(std::move(modelMeshes), std::move(geometryMeshes), std::move(materials), std::move(textures));
	}



}