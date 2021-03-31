#pragma once
#include"ConnectionsStruct.hpp"
#include"../PrimitiveData/PrimitiveDataFunction.hpp"
#include<cassert>

namespace FBXL
{
	//Node����Connections�̎擾
	inline Connections GetConnections(Node&& connections);

	namespace {
		//offest�Ԗڂ�Object��Index�̎擾
		inline ObjectOrPropertyIndex GetObjectIndex(const Node* node, std::size_t offset);

		//offset�Ԗڂ�Property��Index�̎擾
		inline ObjectOrPropertyIndex GetPropertyIndex(const Node* node, std::size_t offset);
	}

	//���̃C���f�b�N�X�̃I�u�W�F�N�g�Ɏq�m�[�h�Ƃ��Đڑ����Ă���I�u�W�F�N�g�̃C���f�b�N�X�̎擾
	inline std::vector<ObjectOrPropertyIndex> GetConnectionByDestination(const Connections& connections, const ObjectOrPropertyIndex& key);


	//
	//�ȉ��A����
	//

	inline Connections GetConnections(Node&& connections)
	{
		assert(connections.name == "Connections");

		Connections result{};
		result.reserve(connections.children.size());

		std::size_t offset{};
		ObjectOrPropertyIndex src{};
		ObjectOrPropertyIndex dst{};
		for (auto& node : connections.children)
		{
			auto type = GetProperty<std::string>(&node, 0);

			if (type.value()[0] == 'O') {
				src = GetObjectIndex(&node, 1);
				offset = 2;
			}
			else if (type.value()[0] == 'P') {
				offset = 3;
				src = GetPropertyIndex(&node, 1);
			}

			if (type.value()[1] == 'O')
				dst = GetObjectIndex(&node, offset);
			else if (type.value()[1] == 'P')
				dst = GetPropertyIndex(&node, offset);

			result.emplace_back(std::make_pair(std::move(src), std::move(dst)));
		}

		return result;
	}


	namespace {

		inline ObjectOrPropertyIndex GetObjectIndex(const Node* node, std::size_t offset)
		{
			return GetProperty<std::int64_t>(node, offset).value();
		}

		inline ObjectOrPropertyIndex GetPropertyIndex(const Node* node, std::size_t offset)
		{
			return std::make_pair(GetProperty<std::int64_t>(node, offset).value(), GetProperty<std::string>(node, offset + 1).value());
		}
	}


	inline std::vector<ObjectOrPropertyIndex> GetConnectionByDestination(const Connections& connections, const ObjectOrPropertyIndex& key)
	{
		std::vector<ObjectOrPropertyIndex> result{};

		for (const auto& c : connections)
		{

			if ((key.index() == 0 &&
				c.second.index() == 0 &&
				std::get<0>(key) == std::get<0>(c.second)) ||
				(key.index() == 1 &&
					c.second.index() == 1 &&
					std::get<1>(key) == std::get<1>(c.second)))
			{
				result.emplace_back(c.first);
			}
		}

		return result;
	}
}