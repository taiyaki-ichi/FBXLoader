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

	std::optional<const Node*> GetProperties70Component(const Node* prop70, const std::string& name);

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
			auto p = GetProperties70Component(prop70[0], "Lcl Transration");
			if (p)
				result.localTranslation = CreateVector3DPolicy::Create(
					GetProperty<double>(p.value(), 4).value(),
					GetProperty<double>(p.value(), 5).value(),
					GetProperty<double>(p.value(), 6).value()
				);
			else
				result.localTranslation = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto p = GetProperties70Component(prop70[0], "Lcl Rotation");
			if (p)
				result.localRotation = CreateVector3DPolicy::Create(
					GetProperty<double>(p.value(), 4).value(),
					GetProperty<double>(p.value(), 5).value(),
					GetProperty<double>(p.value(), 6).value()
				);
			else
				result.localRotation = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
		}

		{
			auto p = GetProperties70Component(prop70[0], "Lcl Scaling");
			if (p)
				result.localScaling = CreateVector3DPolicy::Create(
					GetProperty<double>(p.value(), 4).value(),
					GetProperty<double>(p.value(), 5).value(),
					GetProperty<double>(p.value(), 6).value()
				);
			else
				result.localScaling = CreateVector3DPolicy::Create(0.0, 0.0, 0.0);
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




}