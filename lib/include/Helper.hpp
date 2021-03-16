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
	std::vector<const Node*> GetChildrenNodes(const Node* node, const std::string& name);

	//単一の子ノードの取得
	//複数ある場合はnullopt
	std::optional<const Node*> GetSingleChildrenNode(const Node* node, const std::string& name);

	//Data.mNodesのノードのうちnameが一致するノードの参照の取得
	std::vector<const Node*> GetNodes(const PrimitiveData* data, const std::string& name);

	//二つ以上ある場合もnullopt
	std::optional<const Node*> GetSingleNode(const PrimitiveData* data, const std::string& name);

	//Node.mPropertiesの情報を取得
	template<typename T>
	std::optional<T> GetProperty(const Node* node, std::size_t index);

	//propertyのインデックス3つを指定しVector３Dを生成
	template<typename Vector3D, typename CreateVector3DPolicy = DefaultCreateVector3D<Vector3D>>
	std::optional<Vector3D> GetVector3DProperty(const Node* node, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex);

	std::optional<const Node*> GetProperties70ComponentNode(const Node* prop70, const std::string& name);

	template<typename T>
	std::optional<T> GetProperities70Data(const Node* prop70, const std::string& name, std::int32_t index);

	template<typename Vector3D,typename CreateVector3DPolicy>
	std::optional<Vector3D> GetProperities70Vector3DData(const Node* prop70, const std::string& name, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex);

	std::pair<std::vector<std::int64_t>, std::vector<std::pair<std::int64_t, std::string>>>
		GetConnectionByDestination(const Node* connection, std::int64_t index);

	//インデックスの配列と対応するマテリアルの配列を受け取り
	//マテリアル順に並べ替えたインデックスの配列とそれぞれのマテリアルの適用する長さを返す
	std::pair<std::vector<std::int32_t>, std::vector<std::int32_t>> 
		GetIndeces(const std::vector<std::int32_t>& indecse, const std::vector<std::int32_t>& material);


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
			auto verticesNode = GetSingleChildrenNode(node, "Verticse").value();
			auto vertices = GetProperty<std::vector<double>>(verticesNode, 0).value();

			vetexSize = vertices.size() / 3;

			for (size_t i = 0; i < vertices.size() / 3; i++)
				VertexContainerPushBackPolicy::PushBack(result.vertices, vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
		}

		//index
		{
			auto indexNode = GetSingleChildrenNode(node, "PolygonVertexIndex").value();
			auto indeces = GetProperty<std::vector<std::int32_t>>(indexNode, 0).value();

			

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