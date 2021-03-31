#pragma once
#include<optional>

namespace FBXL
{
	template<typename Vector3D>
	struct ModelMesh
	{
		std::optional<Vector3D> localTranslation = std::nullopt;
		std::optional<Vector3D> localRotation = std::nullopt;
		std::optional<Vector3D> localScaling = std::nullopt;
	};
}