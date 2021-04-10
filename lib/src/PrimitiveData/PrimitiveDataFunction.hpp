#pragma once
#include"PrimitiveDataStruct.hpp"
#include<optional>
#include<algorithm>
#include<cassert>

namespace FBXL
{

	//PrimitiveData.nodesからnodeNameが一致するノードを切り取る
	inline std::optional<Node> RemoveTopLevelNode(PrimitiveData* primitiveData, const std::string& nodeName);

	//node.mChildrenのノードのうちnameが一致するノードの参照の取得
	inline std::vector<const Node*> GetChildrenNodes(const Node* modelMesh, const std::string& name);

	//単一の子ノードの取得
	//複数ある場合はnullopt
	inline std::optional<const Node*> GetSingleChildrenNode(const Node* modelMesh, const std::string& name);

	//Node.mPropertiesのindex番目のT型の情報を取得
	template<typename T>
	std::optional<T> GetProperty(const Node* modelMesh, std::size_t index);

	//propertyのインデックス3つを指定しVector３Dを生成
	template<typename Vector3D, typename CreateVector3DPolicy>
	std::optional<Vector3D> GetVector3DProperty(const Node* modelMesh, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex);

	//Propeties70の子ノードの0番目の名前が一致するnodeの取得
	inline std::optional<const Node*> GetProperties70ComponentNode(const Node* prop70, const std::string& name);

	//Propeties70の子ノードの0番目の名前が一致するnodeのindex番目の型Tのデータを取得
	template<typename T>
	std::optional<T> GetProperities70Data(const Node* prop70, const std::string& name, std::int32_t index);

	//Propeties70の子ノードの0番目の名前が一致するnodeのVector3Dのデータを取得
	template<typename Vector3D, typename CreateVector3DPolicy>
	std::optional<Vector3D> GetProperities70Vector3DData(const Node* prop70, const std::string& name, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex);



	//
	//以下、実装
	//

	inline std::optional<Node> RemoveTopLevelNode(PrimitiveData* primitiveData, const std::string& nodeName)
	{
		for (auto iter = primitiveData->nodes.begin(); iter != primitiveData->nodes.end(); iter++)
		{
			if (iter->name == nodeName) {
				auto result = std::move(*iter);
				primitiveData->nodes.erase(iter);
				return result;
			}
		}

		return std::nullopt;
	}

	inline std::vector<const Node*> GetChildrenNodes(const Node* node, const std::string& name)
	{
		std::vector<const Node*> object{};
		std::for_each(node->children.begin(), node->children.end(),
			[&name, &object](const Node& n) {if (n.name == name)object.push_back(&n); }
		);
		return object;
	}

	std::optional<const Node*> GetSingleChildrenNode(const Node* node, const std::string& name)
	{
		std::optional<const Node*> resultIndex = std::nullopt;
		for (std::size_t i = 0; i < node->children.size(); i++)
		{
			if (!resultIndex && node->children[i].name == name)
				resultIndex = &node->children[i];
			else if (resultIndex && node->children[i].name == name) {
				resultIndex = std::nullopt;
				break;
			}
		}
		return resultIndex;
	}

	template<typename T>
	std::optional<T> GetProperty(const Node* modelMesh, std::size_t index)
	{
		if (std::holds_alternative<T>(modelMesh->properties[index]))
			return std::get<T>(modelMesh->properties[index]);
		else
			return std::nullopt;
	}

	template<typename Vector3D, typename CreateVector3DPolicy>
	std::optional<Vector3D> GetVector3DProperty(const Node* modelMesh, std::int32_t xIndex, std::int32_t yIndex, std::int32_t zIndex)
	{
		static_assert(std::is_invocable_r_v<Vector3D, decltype(CreateVector3DPolicy::Create), double, double, double>,
			"Vector3D CreateVector3DPolicy::Create(double,double,double) is not declared");

		auto x = GetProperty<double>(modelMesh, xIndex);
		auto y = GetProperty<double>(modelMesh, yIndex);
		auto z = GetProperty<double>(modelMesh, zIndex);

		if (x && y && z)
			return CreateVector3DPolicy::Create(x.value(), y.value(), z.value());
		else
			return std::nullopt;
	}

	inline std::optional<const Node*> GetProperties70ComponentNode(const Node* prop70, const std::string& name)
	{
		assert(prop70->name == "Properties70");

		auto ps = GetChildrenNodes(prop70, "P");

		for (auto p : ps)
		{
			if (GetProperty<std::string>(p, 0).value() == name)
				return p;
		}

		return std::nullopt;
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

}