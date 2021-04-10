#pragma once
#include"DataStruct.hpp"
#include"DefaultPolicyTraits.hpp"

namespace FBXL
{
	template<typename Vector2D, typename Vector3D,
		typename CreateVector2DPolicy = DefaultCreateVector2D<Vector2D>,
		typename CreateVector3DPolicy = DefaultCreateVector3D<Vector3D>,
		typename TranslationVector3DPolicy = DefaultTranslationVector3DPolicy<Vector3D>,
		typename RotationVector3DPolicy = DefaultRotationVector3DPolicy<Vector3D>,
		typename ScallingVector3DPolicy = DefaultScallingVector3DPolicy<Vector3D>>
	std::optional<Model3D<Vector2D, Vector3D>> LoadModel3D(const std::string& filePath);

}


#include"src/LoaderImpl.hpp"