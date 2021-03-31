#pragma once
#include"LoaderImpl.hpp"
#include"PrimitiveData/PrimitiveDataLoader.hpp"
#include"FBXComponents/GlobalSettings/GlobalSettingsFunction.hpp"
#include"FBXComponents/Objects/ObjectsFunction.hpp"
#include"Model3DFunction.hpp"

namespace FBXL
{
	template<typename Vector2D, typename Vector3D, typename CreateVector2DPolicy, typename CreateVector3DPolicy,
		typename TranslationVector3DPolicy, typename RotationVector3DPolicy, typename ScallingVector3DPolicy>
	std::optional<Model3D<Vector2D, Vector3D>> LoadModel3D(const std::string& filePath)
	{
		auto primitiveData = LoadPrimitiveData(filePath);

		std::optional<Objects<Vector2D, Vector3D>> objects = std::nullopt;
		std::optional<Connections> connections = std::nullopt;

		auto globalSettingsNode = RemoveTopLevelNode(&primitiveData, "GlobalSettings");
		GlobalSettings globalSettings{};
		if (globalSettingsNode)
			globalSettings = GetGlobalSettings(std::move(globalSettingsNode.value()));


		//Sign

		for (auto& node : primitiveData.nodes)
		{
			if (node.name == "Objects")
				objects = GetObjects<Vector2D, Vector3D, CreateVector2DPolicy, CreateVector3DPolicy>(std::move(node), globalSettings);
			else if (node.name == "Connections")
				connections = GetConnections(std::move(node));
		}

		if (objects && connections)
		{
			auto&& [modelMeshes, geometryMeshes, materials, textures] = std::move(objects.value());

			auto model3DParts = GetModel3DParts<Vector2D, Vector3D, TranslationVector3DPolicy, RotationVector3DPolicy, ScallingVector3DPolicy>(
				std::move(modelMeshes), std::move(geometryMeshes), materials, connections.value());

			auto m = AddTextureInfomarion<Vector3D>(std::move(materials), std::move(textures), connections.value());

			return  GetModel3D<Vector2D, Vector3D>(std::move(model3DParts), std::move(m));
		}

		return std::nullopt;
	}

}