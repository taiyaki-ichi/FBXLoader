#pragma once
#include<vector>
#include<tuple>
#include<optional>
#include<cassert>
#include<algorithm>
#include"DataStruct.hpp"
#include"DefaultPolicyTraits.hpp"

#include<iostream>

namespace FBXL
{

	//node.mChildren�̃m�[�h�̂���name����v����m�[�h�̎Q�Ƃ̎擾
	std::vector<const Node*> GetChildrenNodes(const Node* node, const std::string& name);

	//�P��̎q�m�[�h�̎擾
	//��������ꍇ��nullopt
	std::optional<const Node*> GetSingleChildrenNode(const Node* node, const std::string& name);

	//Data.mNodes�̃m�[�h�̂���name����v����m�[�h�̎Q�Ƃ̎擾
	std::vector<const Node*> GetNodes(const PrimitiveData* data, const std::string& name);

	//��ȏ゠��ꍇ��nullopt
	std::optional<const Node*> GetSingleNode(const PrimitiveData* data, const std::string& name);

	//Node.mProperties�̏����擾
	template<typename T>
	std::optional<T> GetProperty(const Node* node, std::size_t index);

	//property�̃C���f�b�N�X3���w�肵Vector�RD�𐶐�
	template<typename Vector3D, typename CreateVector3DPolicy = DefaultCreateVector3D<Vector3D>>
	std::optional<Vector3D> GetVector3DProperty(const Node* node, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex);

	std::optional<const Node*> GetProperties70ComponentNode(const Node* prop70, const std::string& name);

	template<typename T>
	std::optional<T> GetProperities70Data(const Node* prop70, const std::string& name, std::int32_t index);

	template<typename Vector3D,typename CreateVector3DPolicy>
	std::optional<Vector3D> GetProperities70Vector3DData(const Node* prop70, const std::string& name, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex);

	std::pair<std::vector<std::int64_t>, std::vector<std::pair<std::int64_t, std::string>>>
		GetConnectionByDestination(const Node* connection, std::int64_t index);



	//GeometyMeshNode����}�e���A���̃C���f�b�N�X�z����擾
	std::optional<std::vector<std::int32_t>> GetMaterialIndeces(const Node* geometyMesh);

	//GeometyMeshNode����@���̔z����擾
	template<typename Vector3D,typename CreateVector3DPolicy>
	std::vector<Vector3D> GetNormals(const Node* geometryMesh);

	//GeometyMeshNode����UV�̎擾
	template<typename Vector2D,typename CreateVector2DPolicy>
	std::vector<Vector2D> GetUVs(const Node* geometryMesh);

	//material�̎w�肪�Ȃ�ver
	template<typename Vector2D, typename Vector3D>
	std::vector<Vertex<Vector2D, Vector3D>> GetVertexStructArrayAndMaterialRange(
		std::vector<Vector3D>&& positions, std::vector<Vector3D>&& normals, std::vector<Vector2D>&& uv);

	template<typename Vector2D,typename Vector3D>
	std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>> GetVertexAndMaterialNumberPairs(const Node* geometryMesh);

	template<typename Vector2D,typename Vector3D>
	std::pair<std::vector<Vertex<Vector2D, Vector3D>>, std::vector<std::int32_t>> 
		GetVerticesAndMaterialRange(std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>>&& pair);


	template<typename Vector3D,typename CreateVector3DPolicy=DefaultCreateVector3D<Vector3D>>
	std::optional<ModelMesh<Vector3D>> GetModelMesh(const Node* node)
	{
		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		if (node->name != "Model" || GetProperty<std::string>(node, 2) != "Mesh")
			return std::nullopt;

		ModelMesh<Vector3D> result;

		result.name = GetProperty<std::string>(node, 1).value();

		auto prop70 = GetSingleChildrenNode(node, "Properties70").value();
		
		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "Lcl Transration", 4, 5, 6);
			if (vec)
				result.localTranslation = vec.value();
			else
				result.localTranslation = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "Lcl Rotation", 4, 5, 6);
			if (vec)
				result.localRotation(vec.value());
			else
				result.localRotation = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto vec = GetProperities70Vector3DData<Vector3D, CreateVector3DPolicy>(prop70, "Lcl Scaling", 4, 5, 6);
			if (vec)
				result.localScaling = vec.value();
			else
				result.localScaling = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		return result;
	}
	

	template<typename Vector2D,typename Vector3D,
		typename CreateVector2DPolicy=DefaultCreateVector2D<Vector2D>,
		typename CreateVector3DPolicy=DefaultCreateVector3D<Vector3D>
	>
	std::optional<GeometryMesh<Vector2D,Vector3D>> GetGeometryMesh(const Node* geormetryMesh)
	{
		static_assert(std::is_invocable_r_v<Vector2D, decltype(CreateVector2DPolicy::Create), double, double>,
			"Vecto2D CreateVector2DPolicy::Create(double,double) is not declared");

		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		if (geormetryMesh->name != "Geometry" || GetProperty<std::string>(geormetryMesh, 2) != "Mesh")
			return std::nullopt;

		GeometryMesh<Vector2D, Vector3D> result{};

		result.name = GetProperty<std::string>(geormetryMesh, 1).value();

		auto vertexAndMaterialNumPairs = GetVertexAndMaterialNumberPairs<Vector2D, Vector3D, CreateVector2DPolicy, CreateVector3DPolicy>(geormetryMesh);
		auto [vertices, materialRange] = GetVerticesAndMaterialRange(std::move(vertexAndMaterialNumPairs));
		result.verteces = std::move(vertices);
		result.materialRange = std::move(materialRange);

		return result;
	}

	template<typename Vector3D, typename CreateVector3DPolicy = DefaultCreateVector3D<Vector3D>>
	std::optional<Material<Vector3D>> GetMaterial(const Node* node)
	{
		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		if (node->name != "Material")
			return std::nullopt;

		Material<Vector3D> result;

		result.name = GetProperty<std::string>(node, 1).value();

		auto prop70 = GetSingleChildrenNode(node, "Properties70").value();

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

		return result;
	}


	template<typename T>
	std::optional<T> GetProperities70Data(const Node* prop70, const std::string& name, std::int32_t index)
	{
		assert(prop70->name == "Properties70");

		auto p = GetProperties70ComponentNode(prop70, name);

		if (p)
			return GetProperty<T>(p.value(), index);
		else
			return std::nullopt;
	}

	template<typename Vector3D, typename CreateVector3DPolicy>
	std::optional<Vector3D> GetProperities70Vector3DData(const Node* prop70, const std::string& name, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex)
	{
		assert(prop70->name == "Properties70");

		auto p = GetProperties70ComponentNode(prop70, name);

		if (p)
			return GetVector3DProperty<Vector3D, CreateVector3DPolicy>(p.value(), xIndex, yIndex, zIndex);
		else
			return std::nullopt;
	}



	//
	//�ȉ��A�֐��e���v���[�g�̎���
	//

	template<typename T>
	std::optional<T> GetProperty(const Node* node,std::size_t index)
	{
		if (std::holds_alternative<T>(node->properties[index]))
			return std::get<T>(node->properties[index]);
		else
			return std::nullopt;
	}

	template<typename Vector3D, typename CreateVector3DPolicy>
	std::optional<Vector3D> GetVector3DProperty(const Node* node, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex)
	{
		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		auto x = GetProperty<double>(node, xIndex);
		auto y = GetProperty<double>(node, yIndex);
		auto z = GetProperty<double>(node, zIndex);

		if (x && y && z)
			return CreateVector3DPolicy::Create(x.value(), y.value(), z.value());
		else
			return std::nullopt;
	}




	template<typename Vector3D, typename CreateVector3DPolicy>
	std::vector<Vector3D> GetPositions(const Node* geometryMesh)
	{
		auto verticesNode = GetSingleChildrenNode(geometryMesh, "Vertices").value();
		auto vertices = GetProperty<std::vector<double>>(verticesNode, 0).value();

		auto indexNode = GetSingleChildrenNode(geometryMesh, "PolygonVertexIndex").value();
		auto indeces = GetProperty<std::vector<std::int32_t>>(indexNode, 0).value();

		std::vector<Vector3D> result{};
		//���Ȃ��Ƃ��C���f�b�N�X�̑傫���ȏ�
		result.reserve(indeces.size());

		std::size_t i = 0;
		std::size_t j = 0;
		while (i < indeces.size())
		{
			j = i + 1;

			while (indeces[j + 1] >= 0)
			{
				result.push_back(CreateVector3DPolicy::Create(vertices[indeces[i] * 3], vertices[indeces[i] * 3 + 1], vertices[indeces[i] * 3 + 2]));
				result.push_back(CreateVector3DPolicy::Create(vertices[indeces[j] * 3], vertices[indeces[j] * 3 + 1], vertices[indeces[j] * 3 + 2]));
				result.push_back(CreateVector3DPolicy::Create(vertices[indeces[j + 1] * 3], vertices[indeces[j + 1] * 3 + 1], vertices[indeces[j + 1] * 3 + 2]));

				j++;
			}

			result.push_back(CreateVector3DPolicy::Create(vertices[indeces[i] * 3], vertices[indeces[i] * 3 + 1], vertices[indeces[i] * 3 + 2]));
			result.push_back(CreateVector3DPolicy::Create(vertices[indeces[j] * 3], vertices[indeces[j] * 3 + 1], vertices[indeces[j] * 3 + 2]));
			result.push_back(CreateVector3DPolicy::Create(vertices[(-indeces[j + 1] - 1) * 3], vertices[(-indeces[j + 1] - 1) * 3 + 1], vertices[(-indeces[j + 1] - 1) * 3 + 2]));
			i = j + 2;
		}
		
		return result;
	}

	template<typename Vector3D, typename CreateVector3DPolicy>
	std::vector<Vector3D> GetNormals(const Node* geometryMesh)
	{
		auto layerElementNormalNode = GetSingleChildrenNode(geometryMesh, "LayerElementNormal");

		auto node = GetSingleChildrenNode(layerElementNormalNode.value(), "Normals");

		auto vec = GetProperty<std::vector<double>>(node.value(), 0).value();
		std::vector<Vector3D> result{};
		result.reserve(vec.size() / 3);
		for (std::size_t i = 0; i < vec.size() / 3; i++)
			result.push_back(CreateVector3DPolicy::Create(vec[i * 3], vec[i * 3 + 1], vec[i * 3 + 2]));

		return result;
	}

	template<typename Vector2D, typename CreateVector2DPolicy>
	std::vector<Vector2D> GetUVs(const Node* geometryMesh)
	{
		auto layerElementUVNode = GetSingleChildrenNode(geometryMesh, "LayerElementUV");

		auto uvNode = GetSingleChildrenNode(layerElementUVNode.value(), "UV");
		auto uvIndexNode = GetSingleChildrenNode(layerElementUVNode.value(), "UVIndex");

		auto uv = GetProperty<std::vector<double>>(uvNode.value(), 0).value();
		auto uvIndex = GetProperty<std::vector<std::int32_t>>(uvIndexNode.value(), 0).value();

		std::vector<Vector2D> result{};
		result.reserve(uvIndex.size());
		for (std::size_t i = 0; i < uvIndex.size(); i++)
			result.push_back(CreateVector2DPolicy::Create(uv[uvIndex[i] * 2], uv[uvIndex[i] * 2 + 1]));

		return result;
	}

	template<typename Vector2D, typename Vector3D>
	std::vector<Vertex<Vector2D, Vector3D>> GetVertexStructArrayAndMaterialRange(std::vector<Vector3D>&& positions, std::vector<Vector3D>&& normals, std::vector<Vector2D>&& uv)
	{
		assert(positions.size() == normals.size() == uv.size());

		std::vector<Vertex<Vector2D, Vector3D>> result(positions.size());

		for (std::size_t i = 0; i < result.size(); i++)
		{
			result[i].position = std::move(positions[i]);
			result[i].normal = std::move(normals[i]);
			result[i].uv = std::move(uv[i]);
		}

		return result;
	}


	template<typename Vector2D, typename Vector3D,
		typename CreateVector2DPolicy=DefaultCreateVector2D<Vector2D>,
		typename CreateVector3DPolicy=DefaultCreateVector3D<Vector3D>
	>
	std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>> GetVertexAndMaterialNumberPairs(const Node* geometryMesh)
	{
		std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>> result{};

		auto verticesNode = GetSingleChildrenNode(geometryMesh, "Vertices");
		auto vertices = GetProperty<std::vector<double>>(verticesNode.value(), 0).value();

		auto indexNode = GetSingleChildrenNode(geometryMesh, "PolygonVertexIndex");
		auto indeces = GetProperty<std::vector<std::int32_t>>(indexNode.value(), 0).value();

		auto normals = GetNormals<Vector3D,CreateVector3DPolicy>(geometryMesh);
		auto uvs = GetUVs<Vector2D, CreateVector2DPolicy>(geometryMesh);

		auto materialIndeces = GetMaterialIndeces(geometryMesh);

		auto pushBack = [&vertices, &normals, &uvs, &result, &materialIndeces](std::size_t index1, std::size_t index2, std::size_t index3, std::size_t offset) {
			Vertex<Vector2D, Vector3D> tmpVec;
			tmpVec.position = CreateVector3DPolicy::Create(vertices[index1], vertices[index2], vertices[index3]);
			tmpVec.normal = normals[offset];
			tmpVec.uv = uvs[offset];
			if (materialIndeces)
				result.emplace_back(std::make_pair(std::move(tmpVec), materialIndeces.value()[offset]));
			else
				result.emplace_back(std::make_pair(std::move(tmpVec), 0));
		};

		std::size_t offset = 0;
		std::size_t i = 0;
		std::size_t j = 0;
		Vertex<Vector2D, Vector3D> tmpVec;
		while (i < indeces.size())
		{
			j = i + 1;

			while (indeces[j + 1] >= 0)
			{
				pushBack(indeces[i] * 3, indeces[i] * 3 + 1, indeces[i] * 3 + 2, offset);
				pushBack(indeces[j] * 3, indeces[j] * 3 + 1, indeces[j] * 3 + 2, offset);
				pushBack(indeces[j + 1] * 3, indeces[j + 1] * 3 + 1, indeces[j + 1] * 3 + 2, offset);

				j++;
			}

			pushBack(indeces[i] * 3, indeces[i] * 3 + 1, indeces[i] * 3 + 2, offset);
			pushBack(indeces[j] * 3, indeces[j] * 3 + 1, indeces[j] * 3 + 2, offset);
			pushBack((-indeces[j + 1] - 1) * 3, (-indeces[j + 1] - 1) * 3 + 1, (-indeces[j + 1] - 1) * 3 + 2, offset);

			i = j + 2;

			offset++;
		}

		return result;
	}

	template<typename Vector2D, typename Vector3D>
	std::pair<std::vector<Vertex<Vector2D, Vector3D>>, std::vector<std::int32_t>> GetVerticesAndMaterialRange(std::vector<std::pair<Vertex<Vector2D, Vector3D>, std::int32_t>>&& pair)
	{
		std::pair<std::vector<Vertex<Vector2D, Vector3D>>, std::vector<std::int32_t>> result{};

		if (pair.size() == 0)
			return result;

		std::sort(pair.begin(), pair.end(), [](auto& a, auto& b) {
			return a.second < b.second;
			});

		result.first.resize(pair.size());
		result.second.resize(pair[pair.size() - 1].second + 1);

		std::fill(result.second.begin(), result.second.end(), 0);

		for (std::size_t i = 0; i < pair.size(); i++) {
			result.first[i] = std::move(pair[i].first);
			result.second[pair[i].second]++;
		}

		return result;
	}

}