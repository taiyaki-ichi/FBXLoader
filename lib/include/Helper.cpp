#include"Helper.hpp"
#include<algorithm>

#include<cassert>
#include<stdexcept>

#include<iostream>

namespace FBXL
{

	std::vector<const Node*> GetChildrenNodes(const Node* node, const std::string& name)
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


	std::optional<const Node*> GetProperties70ComponentNode(const Node* prop70, const std::string& name)
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

	std::pair<std::vector<std::int64_t>, std::vector<std::pair<std::int64_t, std::string>>>
		GetConnectionByDestination(const Node* connection, std::int64_t index)
	{
		assert(connection->name == "Connections");

		std::vector<std::int64_t> object{};
		std::vector<std::pair<std::int64_t, std::string>> prop{};

		std::int64_t src;
		std::string srcStr;
		std::int64_t dst;
		std::size_t offset{};
		for (auto& n : connection->children)
		{
			auto type = GetProperty<std::string>(&n, 0);

			src = GetProperty<std::int64_t>(&n, 1).value();

			if (type.value()[0] == 'O')
				offset = 2;
			else if (type.value()[0] == 'P')
				offset = 3;

			if (type.value()[1] == 'O')
			{
				dst = GetProperty<std::int64_t>(&n, offset).value();
				if (dst == index)
					object.push_back(src);
			}
			else if (type.value()[1] == 'P')
			{
				dst = GetProperty<std::int64_t>(&n, offset).value();
				srcStr = GetProperty<std::string>(&n, offset + 1).value();

				if (dst == index)
					prop.push_back(std::make_pair(src, srcStr));
			}
		}

		return std::make_pair(object, prop);
	}

	std::optional<std::vector<std::int32_t>> GetMaterialIndeces(const Node* geometyMesh)
	{
		auto layerElementMaterialNode = GetSingleChildrenNode(geometyMesh, "LayerElementMaterial");

		if (layerElementMaterialNode)
		{
			auto materialNode = GetSingleChildrenNode(layerElementMaterialNode.value(), "Materials").value();

			return GetProperty<std::vector<std::int32_t>>(materialNode, 0).value();
		}
		else
			return std::nullopt;
	}
	namespace
	{
		ObjectOrPropertyIndex GetObjectIndex(const Node* node, std::size_t offset)
		{
			return GetProperty<std::int64_t>(node, offset).value();
		}

		ObjectOrPropertyIndex GetPropertyIndex(const Node* node, std::size_t offset)
		{
			return std::make_pair(GetProperty<std::int64_t>(node, offset).value(), GetProperty<std::string>(node, offset + 1).value());
		}
	}
	
	Connections GetConnections(Node&& connections)
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
	
}