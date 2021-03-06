#pragma once
#include<vector>
#include<tuple>
#include<optional>
#include"DataStruct.hpp"

namespace FBXL
{

	//node.mChildrenのノードのうちnameが一致するノードの参照の取得
	std::vector<const Node*> GetChildrenNode(const Node* node, const std::string& name);

	//Data.mNodesのノードのうちnameが一致するノードの参照の取得
	std::vector<const Node*> GetNode(const Data* data, const std::string& name);

	//Node.mPropertiesの情報を取得
	template<typename T>
	std::optional<T> GetProperty(const Node* node, std::size_t index);

	std::pair<std::vector<std::int64_t>, std::vector<std::pair<std::int64_t, std::string>>>
		GetConnectionByDestination(const Node* connection, std::int64_t index);



	std::optional<const Node*> GetNodeByIndex(const Node* object, std::int64_t index);









	std::optional<NodeAttribute> GetNodeAttribute(const Node* node);
	void Print(const NodeAttribute& na);

	








	//
	//以下、関数テンプレートの実装
	//

	template<typename T>
	std::optional<T> GetProperty(const Node* node,std::size_t index)
	{
		if (std::holds_alternative<T>(node->mProperties[index]))
			return std::get<T>(node->mProperties[index]);
		else
			return std::nullopt;
	}




}