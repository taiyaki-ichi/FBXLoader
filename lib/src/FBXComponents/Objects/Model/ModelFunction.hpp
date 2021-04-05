#pragma once
#include"ModelStruct.hpp"

namespace FBXL
{
	//modelMeshのノードからデータの取得
	template<typename Vector3D, typename CreateVector3DPolicy>
	std::pair<ModelMesh<Vector3D>, std::int64_t> GetModelMesh(Node&& modelMesh);



	//
	//以下、実装
	//

	template<typename Vector3D, typename CreateVector3DPolicy>
	std::pair<ModelMesh<Vector3D>, std::int64_t> GetModelMesh(Node&& modelMesh)
	{
		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		assert(modelMesh.name == "Model" && GetProperty<std::string>(&modelMesh, 2) == "Mesh");

		ModelMesh<Vector3D> result;

		auto index = GetProperty<std::int64_t>(&modelMesh, 0).value();

		auto prop70 = GetSingleChildrenNode(&modelMesh, "Properties70").value();

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "Lcl Transration", 4, 5, 6);
			if (vec)
				result.localTranslation = vec.value();
		}

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "Lcl Rotation", 4, 5, 6);
			if (vec)
				result.localRotation = vec.value();
		}

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "PreRotation", 4, 5, 6);
			if (vec)
				result.localRotation = vec.value();
		}

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "Lcl Scaling", 4, 5, 6);
			if (vec)
				result.localScaling = vec.value();
		}

		return std::make_pair(std::move(result), index);
	}
}