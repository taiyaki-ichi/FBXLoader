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

		auto globalSettingsNode = RemoveTopLevelNode(&primitiveData, "GlobalSettings");

		if (!globalSettingsNode)
			throw "global setting node is not found";
		auto globalSettings = GetGlobalSettings(std::move(globalSettingsNode.value()));

		auto objectsNode = RemoveTopLevelNode(&primitiveData, "Objects");
		if (!objectsNode)
			throw "objects node is not found";


		auto objects = GetObjects<Vector2D, Vector3D, CreateVector2DPolicy, CreateVector3DPolicy>(std::move(objectsNode.value()), globalSettings);

		auto connectionsNode = RemoveTopLevelNode(&primitiveData, "Connections");
		if (!connectionsNode)
			throw "connections node is not found";
		auto connections = GetConnections(std::move(connectionsNode.value()));


		auto model3DParts = GetModel3DParts<Vector2D, Vector3D, TranslationVector3DPolicy, RotationVector3DPolicy, ScallingVector3DPolicy>(
			std::move(objects.modelMeshes), std::move(objects.geometryMeshes), objects.materials, connections);

		auto folderPath = std::string{ filePath.begin(),filePath.begin() + filePath.rfind('/') + 1 };
		auto materialWithTextureInfomation = AddTextureInfomation<Vector3D>(std::move(objects.materials), std::move(objects.textures), connections, std::move(folderPath));

		return GetModel3D<Vector2D, Vector3D>(std::move(model3DParts), std::move(materialWithTextureInfomation));
	}

}