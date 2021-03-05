#pragma once
#include<vector>
#include<tuple>
#include<optional>
#include"DataStruct.hpp"

namespace FBXL
{

	//node.mChildren�̃m�[�h�̂���name����v����m�[�h�̎Q�Ƃ̎擾
	std::vector<const Node*> GetChildrenNode(const Node* node, const std::string& name);

	//Data.mNodes�̃m�[�h�̂���name����v����m�[�h�̎Q�Ƃ̎擾
	std::vector<const Node*> GetNode(const Data* data, const std::string& name);

	//Node.mProperties�̏����擾
	template<typename T>
	std::optional<T> GetProperty(const Node* node, std::size_t index);




	//std::vector<std::int32_t> GetConnectionToDestination(const Node* node, std::int32_t src);








	//
	//�ȉ��A�֐��e���v���[�g�̎���
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