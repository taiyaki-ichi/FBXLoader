#include"Helper.hpp"
#include<algorithm>

#include<cassert>
#include<stdexcept>

#include<iostream>

namespace FBXL
{

	std::vector<const Node*> GetChildrenNode(const Node* node, const std::string& name)
	{
		std::vector<const Node*> object{};
		std::for_each(node->children.begin(), node->children.end(),
			[&name, &object](const Node& n) {if (n.name == name)object.push_back(&n); }
		);
		return object;
	}

	std::vector<const Node*> GetNode(const Data* data, const std::string& name)
	{
		std::vector<const Node*> object{};
		std::for_each(data->nodes.begin(), data->nodes.end(),
			[&object, &name](const Node& n) {if (n.name == name)object.push_back(&n); }
		);
		return object;
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


	std::optional<const Node*> GetNodeByIndex(const Node* object, std::int64_t index)
	{
		assert(object->name == "Objects");

		for (auto& n : object->children)
		{
			auto i = GetProperty<std::int64_t>(&n, 0).value();
			if (i == index)
				return &n;
		}

		return std::nullopt;
	}

	std::optional<NodeAttribute> GetNodeAttribute(const Node* node)
	{
		if (node->name != "NodeAttribute")
			return std::nullopt;

		NodeAttribute object{};

		{
			auto r = GetProperty<std::int64_t>(node, 0);
			if (r)
				object.index = r.value();
		}

		{
			auto r = GetProperty<std::string>(node, 1);
			if (r)
				object.magic = r.value();
		}

		{
			auto r = GetProperty<std::string>(node, 2);
			if (r)
				object.name = r.value();
		}

		auto prop70 = GetChildrenNode(node, "Properties70");
		
		if (prop70.size() == 1)
		{
			NodeAttributeOption option{};

			auto p = GetChildrenNode(prop70[0], "P");
			assert(p.size() == 1);


			for (int i = 0; i < 3; i++)
			{
				auto r = GetProperty<std::string>(p[0], i);
				if (r)
					option.prop70Strings[i] = r.value();
			}
			
			{
				auto r = GetProperty<double>(p[0], 4);
				if (r)
					option.prop70Double = r.value();
			}

			object.option = std::move(option);
		}

		auto typeFlag = GetChildrenNode(node, "TypeFlags");
		assert(typeFlag.size() == 1);

		{
			auto r = GetProperty<std::string>(typeFlag[0], 0);
			if (r)
				object.typeFlag = r.value();
		}

		return object;
	}

	void Print(const NodeAttribute& na)
	{
		std::cout << "NodeAttribute\n";
		std::cout << "index : " << na.index << "\n";
		std::cout << "magic : " << na.magic << "\n";
		std::cout << "name : " << na.name << "\n";
		if (na.option)
		{
			for (int i = 0; i < 3; i++)
				std::cout << "prop70 " << i << " : " << na.option.value().prop70Strings[i] << "\n";
			std::cout << "prop70D : " << na.option.value().prop70Double << "\n";
		}
		std::cout << "typeflag : " << na.typeFlag << "\n";

		std::cout << "\n";
	}



}