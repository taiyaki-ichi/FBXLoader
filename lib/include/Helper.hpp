#pragma once
#include<vector>
#include<tuple>
#include<optional>
#include<cassert>
#include"DataStruct.hpp"
#include"DefaultPolicyTraits.hpp"

#include<iostream>

namespace FBXL
{

	//node.mChildrenのノードのうちnameが一致するノードの参照の取得
	std::vector<const Node*> GetChildrenNode(const Node* node, const std::string& name);

	//Data.mNodesのノードのうちnameが一致するノードの参照の取得
	std::vector<const Node*> GetNode(const Data* data, const std::string& name);

	//Node.mPropertiesの情報を取得
	template<typename T>
	std::optional<T> GetProperty(const Node* node, std::size_t index);

	//propertyのインデックス3つを指定しVector３Dを生成
	template<typename Vector3D, typename CreateVector3DPolicy = DefaultCreateVector3D<Vector3D>>
	std::optional<Vector3D> GetVector3DProperty(const Node* node, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex);

	std::optional<const Node*> GetProperties70ComponentNode(const Node* prop70, const std::string& name);

	std::pair<std::vector<std::int64_t>, std::vector<std::pair<std::int64_t, std::string>>>
		GetConnectionByDestination(const Node* connection, std::int64_t index);



	std::optional<const Node*> GetNodeByIndex(const Node* object, std::int64_t index);


	//nodeのindexも返さねば
	template<
		typename VertexContainer,
		typename IndexContainer,
		typename VertexContainerPushBackPolicy = DefaultVertexContainerPushBack<VertexContainer>,
		typename IndexContainerPushBackPolicy = DefaultIndexContainerPushBack<IndexContainer>
	>
	std::optional<GeometryMesh<VertexContainer,IndexContainer>> GetGeometryMesh(const Node* node)
	{

		static_assert(std::is_invocable_v<decltype(VertexContainerPushBackPolicy::PushBack), VertexContainer&, double, double, double>,
			"auto VertexContainerPushBackPolicy::PushBack(VertexxContainer&,double,double,double) is not declared");

		static_assert(std::is_invocable_v<decltype(IndexContainerPushBackPolicy::PushBack), IndexContainer&, std::int32_t>,
			"auto IndexContainerPushBackPolicy::PushBack(IndexContainer&,std::int32_t) is not declared");


		if (node->name != "Geometry" || GetProperty<std::string>(node, 2) != "Mesh")
			return std::nullopt;


		GeometryMesh<VertexContainer, IndexContainer> result{};
		//indexが有効かどうか判定する際に使用
		std::int64_t vetexSize{};

		//name
		result.name = GetProperty<std::string>(node, 1).value();

		//vertex
		{
			auto verticesNode = GetChildrenNode(node, "Vertices");
			auto vertices = GetProperty<std::vector<double>>(verticesNode[0], 0).value();

			vetexSize = vertices.size() / 3;

			for (size_t i = 0; i < vertices.size() / 3; i++)
				VertexContainerPushBackPolicy::PushBack(result.vertices, vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
		}

		//index
		{
			auto indexNode = GetChildrenNode(node, "PolygonVertexIndex");
			auto indeces = GetProperty<std::vector<std::int32_t>>(indexNode[0], 0).value();

			std::size_t i = 0;
			std::size_t j = 0;
			while (i < indeces.size())
			{
				j = i + 1;

				while (indeces[j + 1] >= 0)
				{
					if (0 <= indeces[i] && indeces[i] < vetexSize &&
						0 <= indeces[j] && indeces[j] < vetexSize &&
						0 <= indeces[j + 1] && indeces[j + 1] < vetexSize)
					{

						IndexContainerPushBackPolicy::PushBack(result.indeces, indeces[i]);
						IndexContainerPushBackPolicy::PushBack(result.indeces, indeces[j]);
						IndexContainerPushBackPolicy::PushBack(result.indeces, indeces[j + 1]);
					}
					j++;
				}

				assert(indeces[j + 1] < 0);

				if (0 <= indeces[i] && indeces[i] < vetexSize &&
					0 <= indeces[j] && indeces[j] < vetexSize &&
					0 <= -indeces[j + 1] - 1 && -indeces[j + 1] - 1 < vetexSize)
				{
					IndexContainerPushBackPolicy::PushBack(result.indeces, indeces[i]);
					IndexContainerPushBackPolicy::PushBack(result.indeces, indeces[j]);
					IndexContainerPushBackPolicy::PushBack(result.indeces, -indeces[j + 1] - 1);
				}

				i = j + 2;
			}
		}

		return result;
	}


	template<typename Vector3D,typename CreateVector3DPolicy=DefaultCreateVector3D<Vector3D>>
	std::optional<ModelMesh<Vector3D>> GetModelMesh(const Node* node)
	{
		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		if (node->name != "Model" || GetProperty<std::string>(node, 2) != "Mesh")
			return std::nullopt;

		ModelMesh<Vector3D> result;

		result.name = GetProperty<std::string>(node, 1).value();

		auto prop70 = GetChildrenNode(node, "Properties70");
		
		{
			auto p = GetProperties70ComponentNode(prop70[0], "Lcl Transration");

			if (p)
				result.localTranslation = GetVector3DProperty<Vector3D, CreateVector3DPolicy>(p.value(), 4, 5, 6).value();
			else
				result.localTranslation = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto p = GetProperties70ComponentNode(prop70[0], "Lcl Rotation");
			
			if (p)
				result.localRotation = GetVector3DProperty<Vector3D, CreateVector3DPolicy>(p.value(), 4, 5, 6).value();
			else
				result.localRotation = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto p = GetProperties70ComponentNode(prop70[0], "Lcl Scaling");

			if (p)
				result.localScaling = GetVector3DProperty<Vector3D, CreateVector3DPolicy>(p.value(), 4, 5, 6).value();
			else
				result.localScaling = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

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

		auto prop70 = GetChildrenNode(node, "Properties70");

		{
			auto p = GetProperties70ComponentNode(prop70[0], "DiffuseColor");

			if (p)
				result.diffuseColor = GetVector3DProperty<Vector3D, CreateVector3DPolicy>(p.value(), 4, 5, 6).value();
			else
				result.diffuseColor = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto p = GetProperties70ComponentNode(prop70[0], "DiffuseFactor");

			if (p)
				result.diffuseFactor = GetProperty<double>(p.value(), 4).value();
			else
				result.diffuseFactor = 0.0;
		}

		{
			auto p = GetProperties70ComponentNode(prop70[0], "AmbientColor");
			if (p)
				result.ambientColor = GetVector3DProperty<Vector3D, CreateVector3DPolicy>(p.value(), 4, 5, 6).value();
			else
				result.ambientColor = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto p = GetProperties70ComponentNode(prop70[0], "AmbientFactor");
			if (p)
				result.ambientFactor = GetProperty<double>(p.value(), 4).value();
			else
				result.ambientFactor = 0.0;
		}
		
		{
			auto p = GetProperties70ComponentNode(prop70[0], "SpecularColor");
			if (p)
				result.specularColor = GetVector3DProperty<Vector3D, CreateVector3DPolicy>(p.value(), 4, 5, 6).value();
			else
				result.specularColor = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto p = GetProperties70ComponentNode(prop70[0], "SpecularFactor");
			if (p)
				result.specularFactor = GetProperty<double>(p.value(), 4).value();
			else
				result.specularFactor = 0.0;
		}

		{
			auto p = GetProperties70ComponentNode(prop70[0], "Shininess");
			if (p)
				result.shininess = GetProperty<double>(p.value(), 4).value();
			else
				result.shininess = 0.0;
		}

		{
			auto p = GetProperties70ComponentNode(prop70[0], "ShininessFactor");
			if (p)
				result.shininessFactor = GetProperty<double>(p.value(), 4).value();
			else
				result.shininessFactor = 0.0;
		}

		{
			auto p = GetProperties70ComponentNode(prop70[0], "Emissive");
			if (p)
				result.emissive = GetVector3DProperty<Vector3D, CreateVector3DPolicy>(p.value(), 4, 5, 6).value();
			else
				result.emissive = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto p = GetProperties70ComponentNode(prop70[0], "EmissiveFactor");
			if (p)
				result.emissiveFactor = GetProperty<double>(p.value(), 4).value();
			else
				result.emissiveFactor = 0.0;
		}

		{
			auto p = GetProperties70ComponentNode(prop70[0], "Transparent");
			if (p)
				result.transparent = GetProperty<double>(p.value(), 4).value();
			else
				result.transparent = 0.0;
		}

		{
			auto p = GetProperties70ComponentNode(prop70[0], "TransparentFactor");
			if (p)
				result.transparentFactor = GetProperty<double>(p.value(), 4).value();
			else
				result.transparentFactor = 0.0;
		}

		{
			auto p = GetProperties70ComponentNode(prop70[0], "ReflectionColor");
			if (p)
				result.reflectionColor = GetVector3DProperty<Vector3D, CreateVector3DPolicy>(p.value(), 4, 5, 6).value();
			else
				result.reflectionColor = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto p = GetProperties70ComponentNode(prop70[0], "ReflectionFactor");
			if (p)
				result.reflectionFactor = GetProperty<double>(p.value(), 4).value();
			else
				result.reflectionFactor = 0.0;

		}

		return result;
	}





	//
	//以下、関数テンプレートの実装
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






}