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
		std::for_each(node->mChildren.begin(), node->mChildren.end(),
			[&name, &object](const Node& n) {if (n.mName == name)object.push_back(&n); }
		);
		return object;
	}

	std::vector<const Node*> GetNode(const Data* data, const std::string& name)
	{
		std::vector<const Node*> object{};
		std::for_each(data->mNodes.begin(), data->mNodes.end(),
			[&object, &name](const Node& n) {if (n.mName == name)object.push_back(&n); }
		);
		return object;
	}

	std::pair<std::vector<std::int64_t>, std::vector<std::pair<std::int64_t, std::string>>>
		GetConnectionByDestination(const Node* connection, std::int64_t index)
	{
		assert(connection->mName == "Connections");

		std::vector<std::int64_t> object{};
		std::vector<std::pair<std::int64_t, std::string>> prop{};

		std::int64_t src;
		std::string srcStr;
		std::int64_t dst;
		std::size_t offset{};
		for (auto& n : connection->mChildren)
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
		assert(object->mName == "Objects");

		for (auto& n : object->mChildren)
		{
			auto i = GetProperty<std::int64_t>(&n, 0).value();
			if (i == index)
				return &n;
		}

		return std::nullopt;
	}

	std::optional<NodeAttribute> GetNodeAttribute(const Node* node)
	{
		if (node->mName != "NodeAttribute")
			return std::nullopt;

		NodeAttribute object{};

		{
			auto r = GetProperty<std::int64_t>(node, 0);
			if (r)
				object.mIndex = r.value();
		}

		{
			auto r = GetProperty<std::string>(node, 1);
			if (r)
				object.mMagic = r.value();
		}

		{
			auto r = GetProperty<std::string>(node, 2);
			if (r)
				object.mName = r.value();
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
					option.mProp70Strings[i] = r.value();
			}
			
			{
				auto r = GetProperty<double>(p[0], 4);
				if (r)
					option.mProp70Double = r.value();
			}

			object.mOption = std::move(option);
		}

		auto typeFlag = GetChildrenNode(node, "TypeFlags");
		assert(typeFlag.size() == 1);

		{
			auto r = GetProperty<std::string>(typeFlag[0], 0);
			if (r)
				object.mTypeFlag = r.value();
		}

		return object;
	}

	void Print(const NodeAttribute& na)
	{
		std::cout << "NodeAttribute\n";
		std::cout << "index : " << na.mIndex << "\n";
		std::cout << "magic : " << na.mMagic << "\n";
		std::cout << "name : " << na.mName << "\n";
		if (na.mOption)
		{
			for (int i = 0; i < 3; i++)
				std::cout << "prop70 " << i << " : " << na.mOption.value().mProp70Strings[i] << "\n";
			std::cout << "prop70D : " << na.mOption.value().mProp70Double << "\n";
		}
		std::cout << "typeflag : " << na.mTypeFlag << "\n";

		std::cout << "\n";
	}



}