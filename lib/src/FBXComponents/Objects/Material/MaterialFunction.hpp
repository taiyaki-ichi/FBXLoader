#pragma once

namespace FBXL
{
	//materialのノードからデータを取得
	template<typename Vector3D, typename CreateVector3DPolicy>
	std::pair<Material<Vector3D>, std::int64_t> GetMaterial(Node&& modelMesh);


	//
	//以下、実装
	//

	template<typename Vector3D, typename CreateVector3DPolicy>
	std::pair<Material<Vector3D>, std::int64_t> GetMaterial(Node&& modelMesh)
	{
		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		assert(modelMesh.name == "Material");

		Material<Vector3D> result;

		auto index = GetProperty<std::int64_t>(&modelMesh, 0).value();

		auto prop70 = GetSingleChildrenNode(&modelMesh, "Properties70").value();

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "DiffuseColor", 4, 5, 6);
			if (vec)
				result.diffuseColor = vec.value();
			else
				result.diffuseColor = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto fac = GetProperities70Data<double>(prop70, "DiffuseFactor", 4);
			if (fac)
				result.diffuseFactor = fac.value();
			else
				result.diffuseFactor = 0.0;
		}

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "AmbientColor", 4, 5, 6);
			if (vec)
				result.ambientColor = vec.value();
			else
				result.ambientColor = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto fac = GetProperities70Data<double>(prop70, "AmbientFactor", 4);
			if (fac)
				result.ambientFactor = fac.value();
			else
				result.ambientFactor = 0.0;
		}

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "SpecularColor", 4, 5, 6);
			if (vec)
				result.specularColor = vec.value();
			else
				result.specularColor = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto fac = GetProperities70Data<double>(prop70, "SpecularFactor", 4);
			if (fac)
				result.specularFactor = fac.value();
			else
				result.specularFactor = 0.0;
		}

		{
			auto shi = GetProperities70Data<double>(prop70, "Shininess", 4);
			if (shi)
				result.shininess = shi.value();
			else
				result.shininess = 0.0;
		}

		{
			auto shiFac = GetProperities70Data<double>(prop70, "ShininessFactor", 4);
			if (shiFac)
				result.shininessFactor = shiFac.value();
			else
				result.shininessFactor = 0.0;
		}

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "Emissive", 4, 5, 6);
			if (vec)
				result.emissive = vec.value();
			else
				result.emissive = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto f = GetProperities70Data<double>(prop70, "EmissiveFactor", 4);
			if (f)
				result.emissiveFactor = f.value();
			else
				result.emissiveFactor = 0.0;
		}

		{
			auto t = GetProperities70Data<double>(prop70, "Transparent", 4);
			if (t)
				result.transparent = t.value();
			else
				result.transparentFactor = 0.0;
		}

		{
			auto tf = GetProperities70Data<double>(prop70, "TransparentFactor", 4);
			if (tf)
				result.transparentFactor = tf.value();
			else
				result.transparentFactor = 0.0;
		}

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "ReflectionColor", 4, 5, 6);
			if (vec)
				result.reflectionColor = vec.value();
			else
				result.reflectionColor = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto rf = GetProperities70Data<double>(prop70, "ReflectionFactor", 4);
			if (rf)
				result.reflectionFactor = rf.value();
			else
				result.reflectionFactor = 0.0;
		}

		return std::make_pair(std::move(result), index);
	}

}